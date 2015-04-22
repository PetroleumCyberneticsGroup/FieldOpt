#include "broker.h"

Broker::Broker(QObject *parent) :
    QObject(parent)
{
}


Broker::Broker(mpi::communicator *comm, Model* m)
{
    world = comm;
    for (int i = 1; i < world->size(); ++i)
        process_busy[i] = false;

    model = m;
    printer = new ParallelPrinter(comm->rank());
}


void Broker::setPerturbations(const QVector<Case *> &value, QVector<int> &ids)
{
    for (int i = 0; i < value.size(); ++i) {
        Perturbation* pert = new Perturbation(value.at(i), ids.at(i));
        perturbations[ids.at(i)] = pert;
        perturbation_evaluated[ids.at(i)] = false;
    }
}


void Broker::evaluatePerturbations()
{
    assert(perturbations.size() != 0);
    while (getNextPerturbationId() != -1 && getFreeProcessId() != -1)  // Send work to all processes initially.
        sendNextPerturbation();

    recvResult();  // Wait for first result. This is to allow for the ordering in the while-loop below.

    while (!isFinished()) {
        printer->print(processStatusString(), true);
        printer->print(perturbationStatusString(), true);
        if (perturbationsRemaining() > 0)
            sendNextPerturbation();
        recvResult();
    }
    printer->print("Broker is done evealuating current batch of perturbations.", false);
}


void Broker::reset()
{
    for (int i = 1; i < world->size(); ++i)
        process_busy[i] = false;
    perturbation_evaluated.clear();
    perturbations.clear();
    results.clear();
}


QVector<Case *> Broker::getResults() const
{
    QVector<Case*> cases;
    foreach (int key, perturbations.keys()) {
        Case* c = perturbations[key]->getCase(model);
        c->setObjectiveValue(results[key]->getResult());
        cases.push_back(c);
    }
    return cases;
}


bool Broker::isFinished()
{
    foreach (int key, perturbation_evaluated.keys()) {
        if (perturbation_evaluated[key] == false)
            return false;
    }
    foreach (int key, process_busy.keys()) {
        if (process_busy[key] == true)
            return false;
    }
    return true;
}


int Broker::getNextPerturbationId()
{
    foreach (int key, perturbation_evaluated.keys()) {
        if (perturbation_evaluated[key] == false)
            return key;
    }
    return -1;
}


int Broker::getFreeProcessId()
{
    foreach (int key, process_busy.keys()) {
        if (process_busy[key] == false)
            return key;
    }
    return -1;
}


void Broker::sendNextPerturbation()
{
    Perturbation* next = perturbations[getNextPerturbationId()];
    int destination = getFreeProcessId();
    std::vector<int> header = next->getSendHeader();
    std::vector<double> binaries = next->getBinaryVariables();
    std::vector<int> integers = next->getIntegerVariables();
    std::vector<double> reals = next->getRealVariables();
    MPI_Send(&header[0], 4, MPI_INT, destination, 10, MPI_COMM_WORLD);
    if (header[1] > 0)
        MPI_Send(&binaries[0], binaries.size(), MPI_DOUBLE, destination, 11, MPI_COMM_WORLD);
    if (header[2] > 0)
        MPI_Send(&integers[0], integers.size(), MPI_INT, destination, 12, MPI_COMM_WORLD);
    if (header[3] > 0)
        MPI_Send(&reals[0], reals.size(), MPI_DOUBLE, destination, 13, MPI_COMM_WORLD);
    process_busy[destination] = true;
    perturbation_evaluated[header[0]] = true;
}


void Broker::recvResult()
{
    MPI_Status status;
    int id;
    double result;
    MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, 20, MPI_COMM_WORLD, &status);
    MPI_Recv(&result, 1, MPI_INT, status.MPI_SOURCE, 21, MPI_COMM_WORLD, &status);
    process_busy[status.MPI_SOURCE] = false;
    Result* newResult = new Result(id, result);
    results[id] = newResult;
}


int Broker::perturbationsRemaining()
{
    int remaining = 0;
    foreach (int key, perturbation_evaluated.keys()) {
        if (perturbation_evaluated[key] == false)
            remaining++;
    }
    return remaining;
}


QString Broker::processStatusString()
{
    QString status = "";
    foreach (int key, process_busy.keys()) {
        if (process_busy[key])
            status.append(QString("Process %1: busy\n").arg(key));
        else
            status.append(QString("Process %1: free\n").arg(key));
    }
    return status;
}


QString Broker::perturbationStatusString()
{
    QString status = "";
    foreach (int key, perturbation_evaluated.keys()) {
        if (perturbation_evaluated[key])
            status.append(QString("Perturbation %1: evaluated\n").arg(key));
        else
            status.append(QString("Perturbation %1: not evaluated\n").arg(key));
    }
    return status;
}
