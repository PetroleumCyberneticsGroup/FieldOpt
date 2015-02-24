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

    QVector<shared_ptr<IntVariable> > m_options_var;
    QVector<QPair<QString, double> > m_options_const;

public:
    WellPath();
    WellPath(const WellPath &wp);  //!< Copy constructor.
    ~WellPath() {}

    void setToeI(shared_ptr<IntVariable> i) {p_toe_i = i;}    //!< Set function for the p_toe_i variable.
    void setToeJ(shared_ptr<IntVariable> j) {p_toe_j = j;}    //!< Set function for the p_toe_j variable.
    void setToeK(shared_ptr<IntVariable> k) {p_toe_k = k;}    //!< Set function for the p_toe_k variable.
    void setHeelI(shared_ptr<IntVariable> i) {p_heel_i = i;}  //!< Set function for the p_heel_i variable.
    void setHeelJ(shared_ptr<IntVariable> j) {p_heel_j = j;}  //!< Set function for the p_heel_j variable.
    void setHeelK(shared_ptr<IntVariable> k) {p_heel_k = k;}  //!< Set function for the p_heel_k variable.
    void setHeelILocked(bool b) {m_lock_heel_i = b;}          //!< Set function for the m_lock_heel_i variable.
    void setHeelJLocked(bool b) {m_lock_heel_j = b;}          //!< Set function for the m_lock_heel_j variable.
    void setHeelKLocked(bool b) {m_lock_heel_k = b;}          //!< Set function for the m_lock_heel_k variable.
};

#endif // WELLPATH_H
