#ifndef INTEGERBOUNDARYCONSTRAINT_H
#define INTEGERBOUNDARYCONSTRAINT_H

class IntegerBoundaryConstraint
{
private:
    int min;
    int max;
public:
    IntegerBoundaryConstraint() {}
    IntegerBoundaryConstraint(int min, int max) { this->min = min; this->max = max;}

    int getMin() const { return min; }
    void setMin(int value) { min = value; }
    int getMax() const { return max; }
    void setMax(int value) { max = value; }
};

#endif // INTEGERBOUNDARYCONSTRAINT_H
