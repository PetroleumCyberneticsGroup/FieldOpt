#include "objectivehandler.h"

ObjectiveHandler::ObjectiveHandler(QObject *parent) : QObject(parent)
{

}

ObjectiveHandler::~ObjectiveHandler()
{

}

void ObjectiveHandler::handleException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    if (severity == ExceptionSeverity::WARNING)
    {
        printWarning(message, type);
        return;
    }
    else if (severity == ExceptionSeverity::ERROR)
    {
        printError(message, type);
        exit(1);
        return;
    }
    else
    {
        return;
    }
}


