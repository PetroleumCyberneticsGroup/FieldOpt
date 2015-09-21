#ifndef OPTIMIZERHANDLER_H
#define OPTIMIZERHANDLER_H

#include <QObject>
#include "exceptionhandler.h"

class OptimizerHandler : public QObject, ExceptionHandler
{
    Q_OBJECT
public:
    explicit OptimizerHandler(QObject *parent = 0);

signals:

public slots:
    void handleException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // OPTIMIZERHANDLER_H
