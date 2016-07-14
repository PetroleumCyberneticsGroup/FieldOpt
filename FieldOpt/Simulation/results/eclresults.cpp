#include "eclresults.h"
#include "ERTWrapper/ertwrapper_exceptions.h"
#include <QVector>

namespace Simulation { namespace Results {

        ECLResults::ECLResults()
                : Results()
        {
            summary_reader_ = 0;
        }

        void ECLResults::ReadResults(QString file_path)
        {
            file_path_ = file_path;
            if (summary_reader_ != 0) delete summary_reader_;
            try {
                summary_reader_ = new ERTWrapper::ECLSummary::ECLSummaryReader(file_path);
            }
            catch (ERTWrapper::SummaryFileNotFoundAtPathException) {
                throw ResultFileNotFoundException(file_path.toLatin1().constData());
            }

            setAvailable();
        }

        void ECLResults::ReadResults(QString file_path, QString build_dir)
        {

        }

        void ECLResults::DumpResults()
        {
            if (summary_reader_ != 0) delete summary_reader_;
            summary_reader_ = 0;
            setUnavailable();
        }

        double ECLResults::GetValue(Results::Property prop)
        {
            if (!isAvailable()) throw ResultsNotAvailableException();
            if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");

            int last_report_step = summary_reader_->GetLastReportStep();

            if (misc_keys_.contains(prop))
                return summary_reader_->GetMiscVar(keys_.value(prop), last_report_step);
            else
                return summary_reader_->GetFieldVar(keys_.value(prop), last_report_step);
        }

        double ECLResults::GetValue(Results::Property prop, int time_index)
        {
            if (!isAvailable()) throw ResultsNotAvailableException();
            if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");
            if (!summary_reader_->HasReportStep(time_index)) throw ResultTimeIndexInvalidException(time_index);

            if (misc_keys_.contains(prop))
                return summary_reader_->GetMiscVar(keys_.value(prop), time_index);
            else
                return summary_reader_->GetFieldVar(keys_.value(prop), time_index);
        }

        double ECLResults::GetValue(Results::Property prop, QString well)
        {
            if (!isAvailable()) throw ResultsNotAvailableException();
            if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");

            int last_report_step = summary_reader_->GetLastReportStep();
            return summary_reader_->GetWellVar(well, keys_.value(prop), last_report_step);
        }

        double ECLResults::GetValue(Results::Property prop, QString well, int time_index)
        {
            if (!isAvailable()) throw ResultsNotAvailableException();
            if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");
            if (!summary_reader_->HasReportStep(time_index)) throw ResultTimeIndexInvalidException(time_index);
            return summary_reader_->GetWellVar(well, keys_.value(prop), time_index);
        }

        std::vector<double> ECLResults::GetValueVector(Results::Property prop)
        {
            if (!isAvailable()) throw ResultsNotAvailableException();
            if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");
            QVector<double> values = QVector<double>();
            if (misc_keys_.contains(prop)) {
                for (int t = summary_reader_->GetFirstReportStep(); t <= summary_reader_->GetLastReportStep(); ++t) {
                    if (summary_reader_->HasReportStep(t))
                        values.append(summary_reader_->GetMiscVar(keys_[prop], t));
                }
            }
            else {
                for (int t = summary_reader_->GetFirstReportStep(); t <= summary_reader_->GetLastReportStep(); ++t) {
                    if (summary_reader_->HasReportStep(t))
                        values.append(summary_reader_->GetFieldVar(keys_[prop], t));
                }
            }
            return values.toStdVector();
        }

    }}
