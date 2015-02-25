#ifndef WELLPATH_H
#define WELLPATH_H

#include <tr1/memory>
#include <QVector>
#include <QPair>
#include <QString>

using std::tr1::shared_ptr;

class IntVariable;

/*!
 * \brief Class representing a well path. Well paths are used by the MRST reservoir simulator.
 */
class WellPath
{
private:
    shared_ptr<IntVariable> p_toe_i;   //!< Pointer to the varible representing the i-coordinate of the toe.
    shared_ptr<IntVariable> p_toe_j;   //!< Pointer to the varible representing the j-coordinate of the toe.
    shared_ptr<IntVariable> p_toe_k;   //!< Pointer to the varible representing the k-coordinate of the toe.
    shared_ptr<IntVariable> p_heel_i;  //!< Pointer to the varible representing the i-coordinate of the heel.
    shared_ptr<IntVariable> p_heel_j;  //!< Pointer to the varible representing the j-coordinate of the heel.
    shared_ptr<IntVariable> p_heel_k;  //!< Pointer to the varible representing the k-coordinate of the heel.

    bool m_lock_heel_i;  //!< Determines whether or not the i-coordinate of the heel is locked (i.e. non-variable).
    bool m_lock_heel_j;  //!< Determines whether or not the j-coordinate of the heel is locked (i.e. non-variable).
    bool m_lock_heel_k;  //!< Determines whether or not the k-coordinate of the heel is locked (i.e. non-variable).

    QVector<shared_ptr<IntVariable>> m_options_var;   //!< Holds variable options.
    QVector<QPair<QString, double>> m_options_const;  //!< Holds constant options.

public:
    WellPath();  //!< Default constructor. Set lock-variables to false.
    WellPath(const WellPath &wp);  //!< Copy constructor.
    ~WellPath() {}

    /*!
     * \brief Initialize the WellPath.
     *
     * 1. Get name of parent well and use it while setting variable names for the toes.
     *
     * 2. Set locking and names for heels.
     *
     * 3. Set variable-names in the m_options_var array.
     * \return Returns false if any of the toe-coordinates are unset. Returns true if initialization was successful.
     */
    bool initialize();
    void updateConstantOption(int i, double value);  //!< Update the value of the constant option at position i.

    void setToeI(shared_ptr<IntVariable> i) {p_toe_i = i;}    //!< Set function for the p_toe_i variable.
    void setToeJ(shared_ptr<IntVariable> j) {p_toe_j = j;}    //!< Set function for the p_toe_j variable.
    void setToeK(shared_ptr<IntVariable> k) {p_toe_k = k;}    //!< Set function for the p_toe_k variable.
    void setHeelI(shared_ptr<IntVariable> i) {p_heel_i = i;}  //!< Set function for the p_heel_i variable.
    void setHeelJ(shared_ptr<IntVariable> j) {p_heel_j = j;}  //!< Set function for the p_heel_j variable.
    void setHeelK(shared_ptr<IntVariable> k) {p_heel_k = k;}  //!< Set function for the p_heel_k variable.
    void setHeelILocked(bool b) {m_lock_heel_i = b;}          //!< Set function for the m_lock_heel_i variable.
    void setHeelJLocked(bool b) {m_lock_heel_j = b;}          //!< Set function for the m_lock_heel_j variable.
    void setHeelKLocked(bool b) {m_lock_heel_k = b;}          //!< Set function for the m_lock_heel_k variable.

    QVector<shared_ptr<IntVariable> > variables();      //!< Get _all_ variables, i.e. m_options_var, and heel- and toe-variables.
    shared_ptr<IntVariable> toeI() {return p_toe_i;}    //!< Get function for p_toe_i
    shared_ptr<IntVariable> toeJ() {return p_toe_j;}    //!< Get function for p_toe_j
    shared_ptr<IntVariable> toeK() {return p_toe_k;}    //!< Get function for p_toe_k
    shared_ptr<IntVariable> heelI() {return p_heel_i;}  //!< Get function for p_heel_i
    shared_ptr<IntVariable> heelJ() {return p_heel_j;}  //!< Get function for p_heel_j
    shared_ptr<IntVariable> heelK() {return p_heel_k;}  //!< Get function for p_heel_k
    bool heelILocked() {return m_lock_heel_i;}          //!< Get function for m_lock_heel_j
    bool heelJLocked() {return m_lock_heel_j;}          //!< Get function for m_lock_heel_j
    bool heelKLocked() {return m_lock_heel_k;}          //!< Get function for m_lock_heel_j


    int numberOfVariableOptions() {return m_options_var.size();}                  //!< Get the size of the m_options_var vector.
    shared_ptr<IntVariable> variableOption(int i) {return m_options_var.at(i);}   //!< Get variable option at position i in the m_options_var vector.
    int numberOfConstantOptions() {return m_options_const.size();}                //!< Get size of the m_options_const vector.
    QPair<QString, double> constantOption(int i) {return m_options_const.at(i);}  //!< Get variable constant at position i in the m_options_const vector.

    void addOptionConstant(const QString &name, double value) {m_options_const.push_back(QPair<QString, double>(name, value));}  //!< Add an option to the m_options_const vector.
    void addOptionVariable(shared_ptr<IntVariable> v) {m_options_var.push_back(v);}                                              //!< Add a variable to the m_options_var vector.
};

#endif // WELLPATH_H
