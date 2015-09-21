#ifndef CASEHANDLER_H
#define CASEHANDLER_H

#include <QObject>
#include "exceptionhandler.h"

class CaseHandler : public QObject, ExceptionHandler
{
    Q_OBJECT
public:
    explicit CaseHandler(QObject *parent = 0);

signals:

public slots:
    void handleException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // CASEHANDLER_H
