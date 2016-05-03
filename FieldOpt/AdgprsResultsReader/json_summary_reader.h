#ifndef JSON_SUMMARY_READER_H
#define JSON_SUMMARY_READER_H

#include <QString>
#include <QVector>
#include <QHash>
#include <QJsonObject>

namespace AdgprsResultsReader {

/*!
 * \brief The JsonSummaryReader class reads the JSON summary files created
 * by the AdgprsSummaryConverter pythnon script. This class should not be instantiated directly.
 *
 * \note All field values are absolute values. I.e. borth production and injection numbers will
 * be positive.
 */
class JsonSummaryReader
{
public:
    /*!
     * \brief JsonSummaryReader reads the summary file and stores the data in arrays.
     * \param file_path Path to a valid .json file.
     */
    JsonSummaryReader(QString file_path);
    ~JsonSummaryReader();

    /*!
     * \brief GetNumberOfWells Returns the number of wells found in the summary.
     */
    int GetNumberOfWells() const { return num_wells_; }

    /*!
     * \brief GetTimeVector Get the time vector for the summary data.
     */
    QVector<double> *GetTimeVector() const { return time_; }

    /*!
     * \brief GetAvalableFieldPropertyNames Get a list of all the valid field property names.
     */
    QList<QString> GetAvalableFieldPropertyNames() const;

    /*!
     * \brief GetFieldProperty Returns a data vector for a specific property.
     * \param prop_name name of the property, using the ECLIPSE summary naming scheme (e.g. FOPT, FWPR).
     */
    QVector<double> *GetFieldProperty(QString prop_name) const;

    /*!
     * \brief GetAvailableWellPropertyNames Get a list of all the valid well property names.
     */
    QList<QString> GetAvailableWellPropertyNames() const;

    /*!
     * \brief GetWellProperty Returns a data vector for a specific property for a specific well.
     * \param well The number of the well to get data from.
     * \param prop_name The name of the property to get.
     */
    QVector<double> *GetWellProperty(int well, QString prop_name) const;

    /*!
     * \brief IsInjector Check if a well is an injector.
     * \param well The number of the well to check.
     */
    bool IsInjector(int well) const;

    /*!
     * \brief GetNumberOfPerforations Get the number of perforations in a specific well.
     * \param well The number of the well to check.
     */
    int GetNumberOfPerforations(int well) const;


private:
    struct WellData {
        bool is_injector;
        int num_perforations;
        QHash<QString, QVector<double> *> properties_;
    };

    QVector<double> *time_;
    int num_wells_;

    QHash<QString, QVector<double> *> field_properties_;
    QList<WellData *> wells_;
    WellData *getWell(int well) const;

    void readGlobal(QJsonObject field);
    void readFieldProps(QJsonObject field_props);
    void readWellProps(QJsonArray well_props);
};

}

#endif // JSON_SUMMARY_READER_H
