#include "casehandler.h"

CaseHandler::CaseHandler(QObject *parent) :
    QObject(parent)
{
}

void CaseHandler::handleException(ExceptionSeverity severity, ExceptionType type, QString message)
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
