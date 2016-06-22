#ifndef EXECUTION_H
#define EXECUTION_H

#include <QString>
#include <QStringList>

namespace Utilities {
    namespace Unix {

        /*!
         * \brief Exec Execute a command using a POSIX pipe.
         * \param directory The directory in which to execute the command.
         * \param command The command to execute.
         * \return The output from the command.
         */
        QString Exec(QString directory, QString command);

        /*!
         * \brief Exec Execute a list of commands using a POSIX pipe.
         * \param directory The directory in which to execute the command.
         * \param commands The commands to be executed.
         * \return The output from the command.
         */
        QString Exec(QString directory, QStringList commands);

        /*!
         * \brief Exec Execute a command using a POSIX pipe.
         * \param directory The directory in which to execute the command.
         * \param command The command to execute.
         * \param verbose Whether or not the output of the command should be displayed in the terminal.
         * \return The output from the command.
         */
        QString Exec(QString directory, QString command, bool verbose);

        /*!
         * \brief Exec Execute a list of commands using a POSIX pipe.
         * \param directory The directory in which to execute the command.
         * \param commands The commands to be executed.
         * \param verbose Whether or not the output of the command should be displayed in the terminal.
         * \return The output from the command.
         */
        QString Exec(QString directory, QStringList commands, bool verbose);

        /*!
         * \brief ExecShellScript Executes a shell script with the given set of parameters.
         * \param script_path Absolute path to the shell script.
         * \param args Arguments to be passed to the script.
         */
        void ExecShellScript(QString script_path, QStringList args);

        /*!
         * @brief ExecShellScriptTimeout execututes a shell script with the given set of parameters, and
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
        bool ExecShellScriptTimeout(QString script_path, QStringList args, int timeout);

    }
}
#endif // EXECUTION_H

