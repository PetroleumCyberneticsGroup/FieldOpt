#ifndef DOUBLEBOUNDARYCONSTRAINT_H
#define DOUBLEBOUNDARYCONSTRAINT_H

class DoubleBoundaryConstraint
{
private:
    double min;
    double max;
public:
    DoubleBoundaryConstraint() {}
    DoubleBoundaryConstraint(double min, double max) { this->min = min; this->max = max; }

    double getMin() const { return min; }
    void setMin(double value) { min = value; }
    double getMax() const { return max; }
    void setMax(double value) { max = value; }
};

#endif // DOUBLEBOUNDARYCONSTRAINT_H
