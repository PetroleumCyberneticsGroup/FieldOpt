/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "eclsummaryreader.h"
#include "ertwrapper_exceptions.h"
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>

namespace ERTWrapper {
    namespace ECLSummary {

        ECLSummaryReader::ECLSummaryReader(string file_name)
        {
            file_name_ = file_name;
            ecl_sum_ = ecl_sum_fread_alloc_case(file_name_.c_str(), "");
            if (ecl_sum_ == NULL) throw SummaryFileNotFoundAtPathException(file_name);
            populateKeyLists();
            initializeVectors();
        }

        ECLSummaryReader::~ECLSummaryReader()
        {
            ecl_sum_free(ecl_sum_);
        }

        double ECLSummaryReader::GetMiscVar(string var_name, int time_index)
        {
            if (!hasMiscVar(var_name))
                throw SummaryVariableDoesNotExistException("Misc variable " + std::string(var_name) + " does not exist.");
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            return ecl_sum_get_misc_var(ecl_sum_, time_index, var_name.c_str());
        }

        double ECLSummaryReader::GetFieldVar(string var_name, int time_index)
        {
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            if (!hasFieldVar(var_name)) {
                throw SummaryVariableDoesNotExistException("Field variable" + var_name + " does not exist.");
            }
            return ecl_sum_get_field_var(ecl_sum_, time_index, var_name.c_str());
        }

        double ECLSummaryReader::GetWellVar(string well_name, string var_name, int time_index)
        {
            if (!hasWellVar(well_name, var_name))
                throw SummaryVariableDoesNotExistException("Well variable " + std::string(well_name) + ":"
                                                           + std::string(var_name) + " does not exist.");
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
            return ecl_sum_get_well_var(ecl_sum_, time_index, well_name.c_str(), var_name.c_str());
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

        bool ECLSummaryReader::hasWellVar(string well_name, string var_name) {
            return ecl_sum_has_well_var(ecl_sum_, well_name.c_str(), var_name.c_str());
        }

        bool ECLSummaryReader::hasGroupVar(string group_name, string var_name) {
            return ecl_sum_has_group_var(ecl_sum_, group_name.c_str(), var_name.c_str());
        }

        bool ECLSummaryReader::hasFieldVar(string var_name) {
            return ecl_sum_has_field_var(ecl_sum_, var_name.c_str());
        }

        bool ECLSummaryReader::hasBlockVar(int block_nr, string var_name) {
            return ecl_sum_has_block_var(ecl_sum_, var_name.c_str(), block_nr);
        }

        bool ECLSummaryReader::hasMiscVar(string var_name) {
            return ecl_sum_has_misc_var(ecl_sum_, var_name.c_str());
        }

        void ECLSummaryReader::populateKeyLists() {
            stringlist_type * keys = ecl_sum_alloc_matching_general_var_list(ecl_sum_, NULL);
            stringlist_type * wells = ecl_sum_alloc_well_list(ecl_sum_, NULL);
            stringlist_type * field_keys = ecl_sum_alloc_matching_general_var_list(ecl_sum_, "F*");
            stringlist_type * well_keys = ecl_sum_alloc_well_var_list(ecl_sum_);

            for (int i = 0; i < stringlist_get_size(keys); ++i)
                keys_.insert(stringlist_safe_iget(keys, i));
            for (int j = 0; j < stringlist_get_size(wells); ++j)
                wells_.insert(stringlist_safe_iget(wells, j));
            for (int k = 0; k < stringlist_get_size(field_keys); ++k)
                field_keys_.insert(stringlist_safe_iget(field_keys, k));
            for (int l = 0; l < stringlist_get_size(well_keys); ++l)
                well_keys_.insert(stringlist_safe_iget(well_keys, l));

            stringlist_free(keys);
            stringlist_free(wells);
            stringlist_free(field_keys);
            stringlist_free(well_keys);

            cout << "Found wells: " << boost::algorithm::join(wells_, ", ") << endl;
            cout << "Found keys: " << boost::algorithm::join(keys_, ", ") << endl;
            cout << "Found field keys: " << boost::algorithm::join(field_keys_, ", ") << endl;
            cout << "Found well keys: " << boost::algorithm::join(well_keys_, ", ") << endl;
        }

        void ECLSummaryReader::initializeVectors() {
            initializeTimeVector();
            initializeWellRates();
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

        void ECLSummaryReader::initializeWellRates() {
            const ecl_smspec_type * smspec = ecl_sum_get_smspec(ecl_sum_);
            for (auto wname : wells_) {
                wopr_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WOPR")) {
                    int index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WOPR");
                    auto * data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                    assert(double_vector_size(data) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wopr_[wname][i] = double_vector_safe_iget(data, i);
                    }
                    wopr_[wname][0] = GetWellVar(wname, "WOPR", 0);
                    double_vector_free(data);
                }

                wwpr_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WWPR")) {
                    int index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WWPR");
                    auto * data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                    assert(double_vector_size(data) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwpr_[wname][i] = double_vector_safe_iget(data, i);
                    }
                    wwpr_[wname][0] = GetWellVar(wname, "WWPR", 0);
                    double_vector_free(data);
                }

                wgpr_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WGPR")) {
                    int index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WGPR");
                    auto * data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                    assert(double_vector_size(data) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wgpr_[wname][i] = double_vector_safe_iget(data, i);
                    }
                    wgpr_[wname][0] = GetWellVar(wname, "WGPR", 0);
                    double_vector_free(data);
                }

                wwir_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WWIR")) {
                    int index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WWIR");
                    auto * data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                    assert(double_vector_size(data) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwir_[wname][i] = double_vector_safe_iget(data, i);
                    }
                    wgpr_[wname][0] = GetWellVar(wname, "WGPR", 0);
                    double_vector_free(data);
                }

                wgir_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WGIR")) {
                    int index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WGIR");
                    auto * data = ecl_sum_alloc_data_vector(ecl_sum_, index, true);
                    assert(double_vector_size(data) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wgir_[wname][i] = double_vector_safe_iget(data, i);
                    }
                    wgir_[wname][0] = GetWellVar(wname, "WGIR", 0);
                    double_vector_free(data);
                }
            }
        }

        void ECLSummaryReader::initializeWellCumulatives() {
            const ecl_smspec_type * smspec = ecl_sum_get_smspec(ecl_sum_);
            for (auto wname : wells_) {
                wopt_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WOPT")) {
                    int wopt_index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WOPT");
                    double_vector_type * wopt = ecl_sum_alloc_data_vector(ecl_sum_, wopt_index, true);
                    assert(double_vector_size(wopt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wopt_[wname][i] = double_vector_safe_iget(wopt, i);
                    }
                    wopt_[wname][0] = 0.0;
                    double_vector_free(wopt);
                }

                wwpt_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WWPT")) {
                    int wwpt_index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WWPT");
                    double_vector_type * wwpt = ecl_sum_alloc_data_vector(ecl_sum_, wwpt_index, true);
                    assert(double_vector_size(wwpt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwpt_[wname][i] = double_vector_safe_iget(wwpt, i);
                    }
                    wwpt_[wname][0] = 0.0;
                    double_vector_free(wwpt);
                }

                wgpt_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WGPT")) {
                    int wgpt_index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WGPT");
                    double_vector_type * wgpt = ecl_sum_alloc_data_vector(ecl_sum_, wgpt_index, true);
                    assert(double_vector_size(wgpt) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wgpt_[wname][i] = double_vector_safe_iget(wgpt, i);
                    }
                    wgpt_[wname][0] = 0.0;
                    double_vector_free(wgpt);
                }

                wwit_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WWIT")) {
                    int wwit_index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WWIT");
                    double_vector_type * wwit = ecl_sum_alloc_data_vector(ecl_sum_, wwit_index, true);
                    assert(double_vector_size(wwit) == time_.size());
                    for (int i = 0; i < time_.size(); ++i) {
                        wwit_[wname][i] = double_vector_safe_iget(wwit, i);
                    }
                    wwit_[wname][0] = 0.0;
                    double_vector_free(wwit);
                }

                wgit_[wname] = std::vector<double>(time_.size(), 0.0);
                if (hasWellVar(wname, "WGIT")) {
                    int wgit_index = ecl_smspec_get_well_var_params_index(smspec, wname.c_str(), "WGIT");
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

        const std::vector<double> ECLSummaryReader::wopt(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            if (wopt_.at(well_name).back() == 0.0)
                warnPropertyZero(well_name, "WOPT");
            return wopt_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wwpt(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            if (wwpt_.at(well_name).back() == 0.0)
                warnPropertyZero(well_name, "WWPT");
            return wwpt_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wgpt(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            if (wgpt_.at(well_name).back() == 0.0)
                warnPropertyZero(well_name, "WGPT");
            return wgpt_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wwit(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            if (wwit_.at(well_name).back() == 0.0)
                warnPropertyZero(well_name, "WWIT");
            return wwit_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wgit(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            if (wgit_.at(well_name).back() == 0.0)
                warnPropertyZero(well_name, "WGIT");
            return wgit_.at(well_name);
        }

        void ECLSummaryReader::warnPropertyZero(string wname, string propname) const {
            std::cerr << "WARNING: Returning cumulative vector with final falue 0.0 for "
                                 "property " + propname + " for well " + wname +
                                 ". The vector may be uninitialized because "
                                 "the property is not found for the well, or because the phase "
                                 "does not exist in the system." << std::endl;
        }

        void ECLSummaryReader::warnPropertyNotFound(string propname) const {
            std::cerr << "WARNING: The field property " + propname + " was not found in "
                                 "the summary. Attempting to calculate it from corresponding well property." << std::endl;
        }

        void ECLSummaryReader::warnPropertyZero(string propname) const {
            std::cerr << "WARNING: Returning cumulative vector with final falue 0.0 for "
                         "property " + propname +
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

        const std::vector<double> ECLSummaryReader::wopr(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            return wopr_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wwpr(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            return wwpr_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wgpr(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            return wgpr_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wwir(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            return wwir_.at(well_name);
        }

        const std::vector<double> ECLSummaryReader::wgir(const string well_name) const {
            if (wells_.find(well_name) == wells_.end())
                throw SummaryVariableDoesNotExistException("The well " + well_name + " was not found in the summary.");
            return wgir_.at(well_name);
        }


    }
}
