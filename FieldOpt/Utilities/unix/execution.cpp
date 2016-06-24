/******************************************************************************
 *
 *
 *
 * Created: 23.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "execution.h"
#include "Utilities/file_handling/filehandling.h"
#include "Utilities/file_handling/filehandling_exceptions.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

namespace Utilities {
    namespace Unix {

        QString Exec(QString directory, QString command)
        {
            return Exec(directory, QStringList(command));
        }

        QString Exec(QString directory, QStringList commands)
        {
            return Exec(directory, commands, false);
        }

        QString Exec(QString directory, QString command, bool verbose)
        {
            return Exec(directory, QStringList(command), verbose);
        }

        QString Exec(QString directory, QStringList commands, bool verbose)
        {
            if (!Utilities::FileHandling::DirectoryExists(directory))
                throw Utilities::FileHandling::DirectoryNotFoundException("Directory for command execution not found", directory);

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

        void ExecShellScript(QString script_path, QStringList args)
        {
            if (!Utilities::FileHandling::FileExists(script_path))
                throw ::Utilities::FileHandling::FileNotFoundException(script_path);
            QString command = script_path + " " + args.join(" ");
            system(command.toLatin1().constData());
        }

        bool ExecShellScriptTimeout(QString script_path, QStringList args, int timeout) {
            if (!Utilities::FileHandling::FileExists(script_path))
                throw ::Utilities::FileHandling::FileNotFoundException(script_path);
            assert(args.length() == 2);

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
                    else if (errno = EINVAL) {
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

        pid_t helpers::fork_child(QString script_path, QStringList arglist) {
            QStringList cmdl = script_path.split("/");
            QString cmd = cmdl.last();
            QString arg1 = arglist[0];
            QString arg2 = arglist[1];
            auto barg1 = arg1.toLatin1();
            auto barg2 = arg2.toLatin1();
            int p = fork();
            if (p == -1) {
                perror("fork");
                exit(1);
            }
            if (p == 0) {
                char *carg1 = barg1.data();
                char *carg2 = barg2.data();
                char *ccmnd = cmd.toLatin1().data();
                char *cscrp = script_path.toLatin1().data();
                char *cargs[] = {ccmnd, carg1, carg2, (char *)0};
                execvp(cscrp, cargs);
                delete cargs, carg1, carg2, ccmnd, cscrp;
                exit(0);
            }
            return p;
        }

        bool ::Utilities::Unix::helpers::is_pid_running(int pid) {
            while(waitpid(-1, 0, WNOHANG) > 0) {
                // Wait for defunct....
            }
            if (0 == kill(pid, 0))
                return 1; // Process exists
            return 0;
        }

        void ::Utilities::Unix::helpers::terminate_process(int pid) {
            if (is_pid_running(pid)) {
                kill(pid, SIGKILL);
            }
        }

    }
}
