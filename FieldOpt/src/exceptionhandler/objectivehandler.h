#ifndef OBJECTIVEHANDLER_H
#define OBJECTIVEHANDLER_H

#include <QObject>
#include "exceptionhandler.h"

class ObjectiveHandler : public QObject, ExceptionHandler
{
    Q_OBJECT
public:
    explicit ObjectiveHandler(QObject *parent = 0);
    ~ObjectiveHandler();

public slots:
    void handleException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // OBJECTIVEHANDLER_H
