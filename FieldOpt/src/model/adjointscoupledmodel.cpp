#include "adjointscoupledmodel.h"

AdjointsCoupledModel::AdjointsCoupledModel() :
    m_perturbation(0.001),
    p_results(0)
{}

AdjointsCoupledModel::AdjointsCoupledModel(const AdjointsCoupledModel &m)
    : CoupledModel(m)
{
    m_perturbation = m.m_perturbation;
    p_results = 0;

    for(int i = 0; i < numberOfWells(); ++i)
    {
        for(int j = 0; j < well(i)->numberOfControls(); ++j)
        {
            AdjointCollection *ac = new AdjointCollection();
            ac->setVariable(well(i)->control(j)->controlVar());

            for(int k = 0; k < numberOfMasterScheduleTimes(); ++k)
            {
                for(int l = 0; l < numberOfWells(); ++l)
                {
                    ac->addAdjoint(new Adjoint(well(l), k));
                }
            }
            m_adjoint_collections.push_back(ac);
        }
    }
}

AdjointsCoupledModel::~AdjointsCoupledModel()
{
    if(p_results != 0)
        delete p_results;

    for(int i = 0; i < m_adjoint_collections.size(); ++i)
        delete m_adjoint_collections.at(i);
}


void AdjointsCoupledModel::initialize()
{
    CoupledModel::initialize();

    for(int i = 0; i < numberOfWells(); ++i)
    {
        for(int j = 0; j < well(i)->numberOfControls(); ++j)
        {
            AdjointCollection *ac = new AdjointCollection();
            ac->setVariable(well(i)->control(j)->controlVar());

            for(int k = 0; k < numberOfMasterScheduleTimes(); ++k)
            {
                for(int l = 0; l < numberOfWells(); ++l)
                {
                    ac->addAdjoint(new Adjoint(well(l), k));
                }
            }
            m_adjoint_collections.push_back(ac);
        }
    }
}


void AdjointsCoupledModel::process()
{
    emitException(ExceptionSeverity::WARNING, ExceptionType::PROGRESS, "Processing Adjoints Coupled Model.");
    // Run perturbations
    QVector<Case*> cases;
    for(int i = 0; i < realVariables().size(); ++i)
    {
        Case *c = processPerturbation(realVariables().at(i));
        cases.push_back(c);
    }

    // Run base case
    Case *base_case = processBaseCase();

    // Set up empty derivatives in the case
    base_case->setObjectiveDerivative(new Derivative());
    for(int i = 0; i < constraints().size(); ++i)
    {
        base_case->addConstraintDerivative(new Derivative(constraints().at(i)->id()));
    }

    // Calculate derivatives from perturbed cases
    for(int i = 0; i < numberOfRealVariables(); ++i)
    {
        Case *case_perturb = cases.at(i);
        int var_id = realVariables().at(i)->id();

        // Calculate partial derivatives for constraints
        for(int j = 0; j < numberOfConstraints(); ++j)
        {
            // dC_j / dx_i
            double dcdx = (case_perturb->constraintValue(j) - base_case->constraintValue(j)) / (case_perturb->realVariableValue(i) - base_case->realVariableValue(i));

            // Add to base case
            base_case->constraintDerivative(j)->addPartial(var_id, dcdx);
        }

        // Calculate partial derivative for objective
        double eps_x = (realVariables().at(i)->max() - realVariables().at(i)->min()) * m_perturbation;
        double dfdx = (case_perturb->objectiveValue() - base_case->objectiveValue()) / eps_x;
        //double dfdx = (case_perturb->objectiveValue() - base_case->objectiveValue()) / (case_perturb->realVariableValue(i) - base_case->realVariableValue(i));
        base_case->objectiveDerivative()->addPartial(var_id, dfdx);
    }

    p_results = base_case;

    // Delete perturbation cases
    for(int i = 0; i < cases.size(); ++i)
        delete cases.at(i);

    setUpToDate(true);
}


AdjointCollection* AdjointsCoupledModel::adjointCollection(shared_ptr<RealVariable> v)
{
    for(int i = 0; i < m_adjoint_collections.size(); ++i)
    {
        if(m_adjoint_collections.at(i)->variable() == v)
            return m_adjoint_collections.at(i);
    }
    return 0;
}


Case* AdjointsCoupledModel::processPerturbation(shared_ptr<RealVariable> v)
{
    double eps_x = (v->max() - v->min()) * m_perturbation;  // Calculate the perturbation size of the variable

    AdjointCollection *ac = adjointCollection(v);

    if(ac != 0)  // Set the pertrubed values for all streams that have adjoints wrt. the variable
    {
        ac->perturbStreams(eps_x);
    }

    if(v != 0)
    {
        /// Change the value of the variable to the pertrubed value. \todo could be variable connected to separator or booster...
        v->setValue(v->value() + eps_x);
    }

    // Process the model with the perturbed values
    updateStreams();
    calculatePipePressures();
    updateConstraints();
    updateObjectiveValue();


    Case *c = new Case(this, true);  // Generate case based on current model state
    v->setValue(v->value() - eps_x);  // Reset the variable value

    if(ac != 0)  // Reset streams if changed
        ac->perturbStreams(-eps_x);
    return c;
}


Case* AdjointsCoupledModel::processBaseCase()
{
    updateStreams();
    calculatePipePressures();
    updateConstraints();
    updateObjectiveValue();
    return new Case(this, true);
}
