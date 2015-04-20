#include "optimizer.h"

void Optimizer::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    OptimizerHandler* sh = new OptimizerHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            sh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               sh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}

void Optimizer::emitProgress(QString message)
{
    OptimizerHandler* sh = new OptimizerHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            sh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(ExceptionSeverity::PROGRESS, ExceptionType::PROGRESS, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               sh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}
