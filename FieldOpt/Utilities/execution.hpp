/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#ifndef EXECUTION_H
#define EXECUTION_H

#include <QString>
#include <QStringList>
#include "Utilities/filehandling.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

namespace Utilities {
namespace Unix {

/*!
 * \brief Exec Execute a list of commands using a POSIX pipe.
 * \param directory The directory in which to execute the command.
 * \param commands The commands to be executed.
 * \param verbose Whether or not the output of the command should be displayed in the terminal.
 * \return The output from the command.
 */
inline QString Exec(QString directory, QStringList commands, bool verbose=false)
{
    if (!Utilities::FileHandling::DirectoryExists(directory))
        throw std::runtime_error("Directory for command execution not found: "
                                     + directory.toStdString());

    QString cmd = "cd " + directory + "; " + commands.join("; ");
    FILE* pipe = popen(cmd.toStdString().c_str(), "r");
    if (!pipe) throw std::runtime_error("Unable to open pipe");
    char buffer[256];
    QString result = "";
    while(!feof(pipe)) {
        if (fgets(buffer, 256, pipe) != NULL) {
            result.append(QString(buffer));
            if (verbose)
                std::cout << buffer << std::endl;
        }
    }
    pclose(pipe);
    return result;
}

/*!
 * \brief ExecShellScript Executes a shell script with the given set of parameters.
 * \param script_path Absolute path to the shell script.
 * \param args Arguments to be passed to the script.
 */
inline void ExecShellScript(QString script_path, QStringList args)
{
    if (!Utilities::FileHandling::FileExists(script_path)) {
        Utilities::FileHandling::ThrowRuntimeError("File not found: " + script_path.toStdString());
    }

    QString command = script_path + " " + args.join(" ");

//    if (settings_->verb_vector()[2] == 1) { // idx:2 => sim verbose
        std::cout << "Current dir:-----------" << Utilities::FileHandling::GetCurrentDirectoryPath().toStdString() << endl;
        std::cout << "Run command:-----------" << command.toStdString() << endl;
//    }

    system(command.toLatin1().constData());
}

namespace helpers {
/*!
 * Create a child-process and execute the given script with the given arguments in it.
 * @param script_path Path to the script to be executed (must take 2 args).
 * @param arglist List of arguments to be passed to the script (must be length 2).
 * @return The PID of the child process.
 */
inline pid_t fork_child(QString script_path, QStringList arglist)
{
    QStringList cmdl = script_path.split("/");
    QString cmd = cmdl.last();
    QString arg1 = arglist[0];
    QString arg2 = arglist[1];
    QString arg3 = arglist[2];
    auto barg1 = arg1.toLatin1();
    auto barg2 = arg2.toLatin1();
    auto barg3 = arg3.toLatin1();
    int p = fork();
    if (p == -1) {
        perror("fork");
        exit(1);
    }
    if (p == 0) {
        char *carg1 = barg1.data();
        char *carg2 = barg2.data();
        char *carg3 = barg3.data();
        char *ccmnd = cmd.toLatin1().data();
        char *cscrp = script_path.toLatin1().data();
        char *cargs[] = {ccmnd, carg1, carg2, carg3, (char *)0};
        execvp(cscrp, cargs);
        delete carg1, carg2, carg3, ccmnd, cscrp;
        exit(0);
    }
    return p;
}

/*!
 * Check if a process with the given PID is running (after waiting for defunct processes to terminate properly).
 * @param pid The PID of the process to check.
 * @return True if the process is running; otherwise false.
 */
inline bool is_pid_running(int pid)
{
    while(waitpid(-1, 0, WNOHANG) > 0) {
        // Wait for defunct....
    }
    if (0 == kill(pid, 0))
        return 1; // Process exists
    return 0;
}

/*!
 * After checking if the process is running using the is_pid_running function, terminate it.
 * @param pid The PID of the process to terminate.
 */
inline void terminate_process(int pid)
{
    if (is_pid_running(pid)) {
        kill(pid, SIGKILL);
    }
}
}

/*!
 * @brief ExecShellScriptTimeout executes a shell script with the given set of parameters, and
 * terminates the process after a set time has passed if it has not returned by then.
 *
 * \todo In this function, in the else block, we can also, at a later stage, monitor the output files
 * of a simulation and use them to decide whether we should abort.
 *
 * @param script_path Absolute path to the shell script.
 * @param args Arguments to be passed to the script.
 * @param timeout Seconds before the execution will be terminated.
 * @return True if the script returned _before_ the timeout, otherwise false.
 */
inline bool ExecShellScriptTimeout(QString script_path, QStringList args, int timeout)
{
    if (!Utilities::FileHandling::FileExists(script_path))
        throw std::runtime_error("File not found: " + script_path.toStdString());
    assert(args.length() == 3);

    pid_t pid;
    sigset_t mask;
    sigset_t orig_mask;
    struct timespec to;
    sigemptyset (&mask);
    sigaddset (&mask, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror ("sigprocmask");
        return false;
    }

    pid  = helpers::fork_child(script_path, args);
    to.tv_sec = timeout;
    to.tv_nsec = 0;

    std::cout << "Monitoring child process with pid " << pid << ". Timeout set to " << to.tv_sec << std::endl;
    do {
        if (!helpers::is_pid_running(pid)) // If the child no longer exists, return true
            return true;
        int ret = sigtimedwait(&mask, NULL, &to);
        std::cout << "SIGTIMEDWAIT returned " << ret << " with errno " << errno << std::endl;
        if (ret < 0) {
            if (errno == EINTR) {
                std::cout << "Interrupted by a signal other than sigchild." << std::endl;
                helpers::terminate_process(pid);
                return false;
            }
            else if (errno == EAGAIN) {
                std::cout << "Timeout, killing child " << pid << std::endl;
                if (helpers::is_pid_running(pid)) { // Ensure that child still exists
                    kill(pid, SIGKILL);
                    return false;
                }
                else {
                    std::cout << "The process terminated successfully before the set timeout." << std::endl;
                    return true;
                }
            }
            else if (errno == EINVAL) {
                std::cout << "Got error EINVAL from process, terminating it." << std::endl;
                helpers::terminate_process(pid);
                return false;
            }
            else {
                std::cout << "Got error sigtimedwait from process, terminating it." << std::endl;
                helpers::terminate_process(pid);
                return false;
            }
        }
        else { // sigtimedwait returned something >= 0
            if (helpers::is_pid_running(pid)) {
                std::cout << "The process returned a non-error value, but it is still running - continue waiting ... " << std::endl;
                continue;
            }
            else {
                std::cout << "The process terminated successfully before the set timeout." << std::endl;
                return true;
            }
        }
    } while (1);
}
}
}
#endif // EXECUTION_H

