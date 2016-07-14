#include "eclsummaryreader.h"
#include "ertwrapper_exceptions.h"
#include <iostream>
#include <assert.h>

namespace ERTWrapper {
    namespace ECLSummary {

        ECLSummaryReader::ECLSummaryReader(QString file_name)
        {
            file_name_ = file_name;
            ecl_sum_ = ecl_sum_fread_alloc_case(file_name_.toLatin1().constData(), "");
            if (ecl_sum_ == NULL) throw SummaryFileNotFoundAtPathException(file_name.toLatin1().constData());
            populateKeyLists();
            initializeVectors();
        }

        ECLSummaryReader::~ECLSummaryReader()
        {
            ecl_sum_free(ecl_sum_);
        }

        double ECLSummaryReader::GetMiscVar(QString var_name, int time_index)
        {
            if (!hasMiscVar(var_name))
                throw SummaryVariableDoesNotExistException("Misc variable " + std::string(var_name.toLatin1().constData()) + " does not exist.");
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            return ecl_sum_get_misc_var(ecl_sum_, time_index, var_name.toLatin1().constData());
        }

        double ECLSummaryReader::GetFieldVar(QString var_name, int time_index)
        {
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            if (!hasFieldVar(var_name)) {
                throw SummaryVariableDoesNotExistException("Field variable" + var_name.toStdString() + " does not exist.");
            }
            return ecl_sum_get_field_var(ecl_sum_, time_index, var_name.toLatin1().constData());
        }

        double ECLSummaryReader::GetWellVar(QString well_name, QString var_name, int time_index)
        {
            if (!hasWellVar(well_name, var_name))
                throw SummaryVariableDoesNotExistException("Well variable " + std::string(well_name.toLatin1().constData()) + ":"
                                                           + std::string(var_name.toLatin1().constData()) + " does not exist.");
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            return ecl_sum_get_well_var(ecl_sum_, time_index, well_name.toLatin1().constData(), var_name.toLatin1().constData());
        }

        int ECLSummaryReader::GetLastReportStep()
        {
            int last_step = ecl_sum_get_last_report_step(ecl_sum_);
            return ecl_sum_iget_report_end(ecl_sum_, last_step);
        }

        int ECLSummaryReader::GetFirstReportStep()
        {
            int first_step = ecl_sum_get_first_report_step(ecl_sum_);
            return ecl_sum_iget_report_start(ecl_sum_, first_step);
        }

        bool ECLSummaryReader::HasReportStep(int report_step) {
            return report_step <= GetLastReportStep() && report_step >= GetFirstReportStep();
        }

        bool ECLSummaryReader::hasWellVar(QString well_name, QString var_name) {
            return ecl_sum_has_well_var(ecl_sum_,
                                        well_name.toLatin1().constData(),
                                        var_name.toLatin1().constData());
        }

        bool ECLSummaryReader::hasGroupVar(QString group_name, QString var_name) {
            return ecl_sum_has_group_var(ecl_sum_,
                                         group_name.toLatin1().constData(),
                                         var_name.toLatin1().constData());
        }

        bool ECLSummaryReader::hasFieldVar(QString var_name) {
            return ecl_sum_has_field_var(ecl_sum_,
                                         var_name.toLatin1().constData());
        }

        bool ECLSummaryReader::hasBlockVar(int block_nr, QString var_name) {
            return ecl_sum_has_block_var(ecl_sum_,
                                         var_name.toLatin1().constData(),
                                         block_nr);
        }

        bool ECLSummaryReader::hasMiscVar(QString var_name) {
            return ecl_sum_has_misc_var(ecl_sum_,
                                        var_name.toLatin1().constData());
        }

        void ECLSummaryReader::populateKeyLists() {
            stringlist_type * keys = ecl_sum_alloc_matching_general_var_list(ecl_sum_, NULL);
            stringlist_type * wells = ecl_sum_alloc_well_list(ecl_sum_, NULL);
            stringlist_type * field_keys = ecl_sum_alloc_matching_general_var_list(ecl_sum_, "F*");
            stringlist_type * well_keys = ecl_sum_alloc_well_var_list(ecl_sum_);

            for (int i = 0; i < stringlist_get_size(keys); ++i)
                keys_.push_back(QString::fromLatin1(stringlist_safe_iget(keys, i)));
            for (int j = 0; j < stringlist_get_size(wells); ++j)
                wells_.push_back(QString::fromLatin1(stringlist_safe_iget(wells, j)));
            for (int k = 0; k < stringlist_get_size(field_keys); ++k)
                field_keys_.push_back(QString::fromLatin1(stringlist_safe_iget(field_keys, k)));
            for (int l = 0; l < stringlist_get_size(well_keys); ++l)
                well_keys_.push_back(QString::fromLatin1(stringlist_safe_iget(well_keys, l)));

            stringlist_free(keys);
            stringlist_free(wells);
            stringlist_free(field_keys);
            stringlist_free(well_keys);
        }

        void ECLSummaryReader::initializeVectors() {
            initializeTimeVector();
            initializeWellCumulatives();
            initializeFieldCumulatives();
        }

        void ECLSummaryReader::initializeTimeVector() {
            int days_var_index = ecl_sum_get_misc_var_index(ecl_sum_, "TIME");
            double_vector_type * time = ecl_sum_alloc_data_vector(ecl_sum_, days_var_index, true);
            time_.resize(double_vector_size(time));
            for (int i = 0; i < double_vector_size(time); ++i) {
                time_[i] = (double_vector_safe_iget(time, i));
            }
            time_[0] = 0; // For some reason this is often not 0, but something > 1e8. Probably the time since epoch.
            double_vector_free(time);
        }

        void ECLSummaryReader::initializeWellCumulatives() {
            const ecl_smspec_type * smspec = ecl_sum_get_smspec(ecl_sum_);
            for (auto wname : wells_) {
                wopt_.insert(wname, std::vector<double>(time_.size(), 0.0));
                if (hasWellVar(wname, "WOPT")) {
                    int wopt_index = ecl_smspec_get_well_var_params_index(smspec, wname.toLatin1(), "WOPT");
                    double_vector_type * wopt = ecl_sum_alloc_data_vector(ecl_sum_, wopt_index, true);
                    assert(double_vector_size(wopt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wopt_[wname][i] = double_vector_safe_iget(wopt, i);
                    }
                    wopt_[wname][0] = 0.0;
                    double_vector_free(wopt);
                }

                wwpt_.insert(wname, std::vector<double>(time_.size(), 0.0));
                if (hasWellVar(wname, "WWPT")) {
                    int wwpt_index = ecl_smspec_get_well_var_params_index(smspec, wname.toLatin1(), "WWPT");
                    double_vector_type * wwpt = ecl_sum_alloc_data_vector(ecl_sum_, wwpt_index, true);
                    assert(double_vector_size(wwpt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwpt_[wname][i] = double_vector_safe_iget(wwpt, i);
                    }
                    wwpt_[wname][0] = 0.0;
                    double_vector_free(wwpt);
                }

                wgpt_.insert(wname, std::vector<double>(time_.size(), 0.0));
                if (hasWellVar(wname, "WGPT")) {
                    int wgpt_index = ecl_smspec_get_well_var_params_index(smspec, wname.toLatin1(), "WGPT");
                    double_vector_type * wgpt = ecl_sum_alloc_data_vector(ecl_sum_, wgpt_index, true);
                    assert(double_vector_size(wgpt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wgpt_[wname][i] = double_vector_safe_iget(wgpt, i);
                    }
                    wgpt_[wname][0] = 0.0;
                    double_vector_free(wgpt);
                }

                wwit_.insert(wname, std::vector<double>(time_.size(), 0.0));
                if (hasWellVar(wname, "WWIT")) {
                    int wwit_index = ecl_smspec_get_well_var_params_index(smspec, wname.toLatin1(), "WWIT");
                    double_vector_type * wwit = ecl_sum_alloc_data_vector(ecl_sum_, wwit_index, true);
                    assert(double_vector_size(wwit) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwit_[wname][i] = double_vector_safe_iget(wwit, i);
                    }
                    wwit_[wname][0] = 0.0;
                    double_vector_free(wwit);
                }

                wgit_.insert(wname, std::vector<double>(time_.size(), 0.0));
                if (hasWellVar(wname, "WGIT")) {
                    int wgit_index = ecl_smspec_get_well_var_params_index(smspec, wname.toLatin1(), "WGIT");
                    double_vector_type * wgit = ecl_sum_alloc_data_vector(ecl_sum_, wgit_index, true);
                    assert(double_vector_size(wgit) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wgit_[wname][i] = double_vector_safe_iget(wgit, i);
                    }
                    wwit_[wname][0] = 0.0;
                    double_vector_free(wgit);
                }
            }
        }

        void ECLSummaryReader::initializeFieldCumulatives() {
            const ecl_smspec_type * smspec = ecl_sum_get_smspec(ecl_sum_);

            fopt_ = std::vector<double>(time_.size(), 0.0);
            if (hasFieldVar("FOPT")) {
                int index = ecl_smspec_get_field_var_params_index(smspec, "FOPT");
                auto data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                assert(double_vector_size(data) == time_.size());
                for (int i = 0; i < time_.size(); ++i) {
                    fopt_[i] = double_vector_safe_iget(data, i);
                }
                fopt_[0] = 0.0;
                double_vector_free(data);
            }
            else {
                warnPropertyNotFound("FOPT");
                for (auto wname : wells_) {
                    for (int i = 0; i < time_.size(); ++i) {
                        fopt_[i] += wopt_[wname][i];
                    }
                }
            }

            fwpt_ = std::vector<double>(time_.size(), 0.0);
            if (hasFieldVar("FWPT")) {
                int index = ecl_smspec_get_field_var_params_index(smspec, "FWPT");
                auto data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                assert(double_vector_size(data) == time_.size());
                for (int i = 0; i < time_.size(); ++i) {
                    fwpt_[i] = double_vector_safe_iget(data, i);
                }
                fwpt_[0] = 0.0;
                double_vector_free(data);
            }
            else {
                warnPropertyNotFound("FWPT");
                for (auto wname : wells_) {
                    for (int i = 0; i < time_.size(); ++i) {
                        fwpt_[i] += wwpt_[wname][i];
                    }
                }
            }

            fgpt_ = std::vector<double>(time_.size(), 0.0);
            if (hasFieldVar("FGPT")) {
                int index = ecl_smspec_get_field_var_params_index(smspec, "FGPT");
                auto data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                assert(double_vector_size(data) == time_.size());
                for (int i = 0; i < time_.size(); ++i) {
                    fgpt_[i] = double_vector_safe_iget(data, i);
                }
                fgpt_[0] = 0.0;
                double_vector_free(data);
            }
            else {
                warnPropertyNotFound("FGPT");
                for (auto wname : wells_) {
                    for (int i = 0; i < time_.size(); ++i) {
                        fgpt_[i] += wgpt_[wname][i];
                    }
                }
            }

            fwit_ = std::vector<double>(time_.size(), 0.0);
            if (hasFieldVar("FWIT")) {
                int index = ecl_smspec_get_field_var_params_index(smspec, "FWIT");
                auto data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                assert(double_vector_size(data) == time_.size());
                for (int i = 0; i < time_.size(); ++i) {
                    fwit_[i] = double_vector_safe_iget(data, i);
                }
                fwit_[0] = 0.0;
                double_vector_free(data);
            }
            else {
                warnPropertyNotFound("FWIT");
                for (auto wname : wells_) {
                    for (int i = 0; i < time_.size(); ++i) {
                        fwit_[i] += wwit_[wname][i];
                    }
                }
            }

            fgit_ = std::vector<double>(time_.size(), 0.0);
            if (hasFieldVar("FGIT")) {
                int index = ecl_smspec_get_field_var_params_index(smspec, "FGIT");
                auto data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                assert(double_vector_size(data) == time_.size());
                for (int i = 0; i < time_.size(); ++i) {
                    fgit_[i] = double_vector_safe_iget(data, i);
                }
                fgit_[0] = 0.0;
                double_vector_free(data);
            }
            else {
                warnPropertyNotFound("FGIT");
                for (auto wname : wells_) {
                    for (int i = 0; i < time_.size(); ++i) {
                        fgit_[i] += wgit_[wname][i];
                    }
                }
            }
        }

        const std::vector<double> ECLSummaryReader::wopt(const QString well_name) const {
            if (!wells_.contains(well_name))
                throw SummaryVariableDoesNotExistException("The well " + well_name.toStdString() + " was not found in the summary.");
            if (wopt_[well_name].back() == 0.0)
                warnPropertyNotFound(well_name, "WOPT");
            return wopt_[well_name];
        }

        const std::vector<double> ECLSummaryReader::wwpt(const QString well_name) const {
            if (!wells_.contains(well_name))
                throw SummaryVariableDoesNotExistException("The well " + well_name.toStdString() + " was not found in the summary.");
            if (wwpt_[well_name].back() == 0.0)
                warnPropertyNotFound(well_name, "WWPT");
            return wwpt_[well_name];
        }

        const std::vector<double> ECLSummaryReader::wgpt(const QString well_name) const {
            if (!wells_.contains(well_name))
                throw SummaryVariableDoesNotExistException("The well " + well_name.toStdString() + " was not found in the summary.");
            if (wgpt_[well_name].back() == 0.0)
                warnPropertyNotFound(well_name, "WGPT");
            return wgpt_[well_name];
        }

        const std::vector<double> ECLSummaryReader::wwit(const QString well_name) const {
            if (!wells_.contains(well_name))
                throw SummaryVariableDoesNotExistException("The well " + well_name.toStdString() + " was not found in the summary.");
            if (wwit_[well_name].back() == 0.0)
                warnPropertyNotFound(well_name, "WWIT");
            return wwit_[well_name];
        }

        const std::vector<double> ECLSummaryReader::wgit(const QString well_name) const {
            if (!wells_.contains(well_name))
                throw SummaryVariableDoesNotExistException("The well " + well_name.toStdString() + " was not found in the summary.");
            if (wgit_[well_name].back() == 0.0)
                warnPropertyNotFound(well_name, "WGIT");
            return wgit_[well_name];
        }

        void ECLSummaryReader::warnPropertyNotFound(QString wname, QString propname) const {
            std::cerr << "WARNING: Returning cumulative vector with final falue 0.0 for "
                                 "property " + propname.toStdString() +
                                 " for well " + wname.toStdString() +
                                 ". The vector may be uninitialized because "
                                 "the property is not found for the well, or because the phase "
                                 "does not exist in the system." << std::endl;
        }

        void ECLSummaryReader::warnPropertyNotFound(QString propname) const {
            std::cerr << "WARNING: The field property " + propname.toStdString() + " was not found in "
                                 "the summary. Attempting to calculate it from corresponding well property." << std::endl;
        }

        void ECLSummaryReader::warnPropertyZero(QString propname) const {
            std::cerr << "WARNING: Returning cumulative vector with final falue 0.0 for "
                         "property " + propname.toStdString() +
                         ". The vector may be uninitialized because "
                         "the property is not found for the well, or because the phase "
                         "does not exist in the system." << std::endl;
        }

        const std::vector<double> &ECLSummaryReader::fopt() const {
            if (fopt_.back() == 0.0)
                warnPropertyZero("FOPT");
            return fopt_;
        }

        const std::vector<double> &ECLSummaryReader::fwpt() const {
            if (fwpt_.back() == 0.0)
                warnPropertyZero("WOPT");
            return fwpt_;
        }

        const std::vector<double> &ECLSummaryReader::fgpt() const {
            if (fgpt_.back() == 0.0)
                warnPropertyZero("FGPT");
            return fgpt_;
        }

        const std::vector<double> &ECLSummaryReader::fwit() const {
            if (fgpt_.back() == 0.0)
                warnPropertyZero("FWIT");
            return fwit_;
        }

        const std::vector<double> &ECLSummaryReader::fgit() const {
            if (fgit_.back() == 0.0)
                warnPropertyZero("FGIT");
            return fgit_;
        }


    }
}
