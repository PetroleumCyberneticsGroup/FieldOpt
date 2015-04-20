#include "optimizerhandler.h"

OptimizerHandler::OptimizerHandler(QObject *parent) :
    QObject(parent)
{
}

void OptimizerHandler::handleException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    if (severity == ExceptionSeverity::WARNING)
    {
        message.prepend("An exception has occured in a Simulator class.\n");
        printWarning(message, type);
        return;
    }
    if (severity == ExceptionSeverity::PROGRESS) {
        printProgress(message);
        return;
    }
    else if (severity == ExceptionSeverity::ERROR)
    {
        message.prepend("An exception has occured in a Simulator class.\n");
        printError(message, type);
        exit(1);
        return;
    }
    else
    {
        return;
    }
}
