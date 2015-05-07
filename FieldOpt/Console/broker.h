#ifndef BROKER_H
#define BROKER_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <boost/mpi.hpp>
#include <vector>
#include <QDateTime>

#include "transferobjects/perturbation.h"
#include "transferobjects/result.h"
#include "optimizers/case.h"
#include "model/model.h"
#include "parallelprinter.h"
#include "resultslogger.h"
#include "batchlogger.h"

namespace mpi = boost::mpi;

class Broker : public QObject
{
    Q_OBJECT
private:
    mpi::communicator* world;
    QHash<int, bool> process_busy;            //!< Hashmap to keep track of which processes are busy. <int, bool> = <rank, status>
    QHash<int, bool> perturbation_evaluated;  //!< Hashmap to keep track of which perturbations have been evaluated. <int, bool> = <perturbation_id, status>
    QHash<int, Perturbation*> perturbations;  //!< Hashmap containing all perturbations. <int, Perturbation> = <perturbation_id, Perturbation object>
    QHash<int, Result*> results;              //!< Hashmap containing all results. <int, Result> = <perturbation_id, Result object>
    QHash<int, QVector<QDateTime>> timestamps;//!< Hashmap to keep track of start/stop times for simulations.
    Model* model;
    ParallelPrinter* printer;
    ResultsLogger* logger;
    BatchLogger* batch_logger;

    bool isFinished();                    //!< Returns true if at least one perturbation has not yet been evaluated or if at least one process is currently busy.
    int getNextPerturbationId();  //!< Finds the next perturbation to be evaluated. Returns -1 if all perturbations are evaluated or currently being evaluated.
    int getFreeProcessId();               //!< Finds the id of a non-busy process. Returns -1 if all processes are busy.
    void sendNextPerturbation();
    void recvResult();
    int perturbationsRemaining();
    int getTimeSpan(int id);

    QString processStatusString();
    QString perturbationStatusString();

    void printProgress();

public:
    explicit Broker(QObject *parent = 0);
    Broker(mpi::communicator *comm, Model* m, ResultsLogger* l);

    void setPerturbations(const QVector<Case *> &value, QVector<int> &ids);
    void evaluatePerturbations();
    void reset();

    QVector<Case *> getResults() const;
signals:

public slots:

};

#endif // BROKER_H
