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
            char *script = script_path.toLatin1().data();
            char *arg1 = args[0].toLatin1().data();
            char *arg2 = args[1].toLatin1().data();

            pid_t pid, wpid;
            pid  = fork();
            if (pid < 0) {
                std::cout << "Error forking." << std::endl;
                return false;
            }
            if (pid == 0) {
                execlp(script, script, arg1, arg2, NULL); // Execute script as child
                exit(0);
            }
            else { // Monitor execution of child
                int waittime = 0;
                int status;
                do {
                    wpid = waitpid(pid, &status, WNOHANG);
                    if (wpid == 0) {
                        if (waittime < timeout) {
                            sleep(1);
                            waittime++;
                        }
                        else {
                            kill(pid, SIGKILL);
                            sleep(1);
                        }
                    }
                } while (wpid == 0 && waittime <= timeout);
                if (WIFEXITED(status)) {
                    std::cout << "Script with pid " << pid
                              << " returned with status " << status
                              << " after " << waittime
                              << " which is before the allowed " << timeout << " seconds."
                              << std::endl;
                    return true;
                }
                else if (WIFSIGNALED(status)) {
                    std::cout << "Script with pid " << pid
                              << " was terminated with status " << WTERMSIG(status)
                              << " after " << waittime << " seconds"
                              << " which is more than the allowed " << timeout << " seconds."

                              << std::endl;
                    return false;
                }
            }

            return false;
        }

    }
}
