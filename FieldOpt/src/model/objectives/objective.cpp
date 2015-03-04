#include "objective.h"

void Objective::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    ObjectiveHandler* oh = new ObjectiveHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            oh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               oh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}
