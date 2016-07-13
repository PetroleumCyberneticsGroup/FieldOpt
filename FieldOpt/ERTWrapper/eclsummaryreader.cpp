#include "eclsummaryreader.h"
#include "ertwrapper_exceptions.h"
#include <iostream>

namespace ERTWrapper {
    namespace ECLSummary {

        ECLSummaryReader::ECLSummaryReader(QString file_name)
        {
            file_name_ = file_name;
            ecl_sum_ = ecl_sum_fread_alloc_case(file_name_.toLatin1().constData(), "");
            if (ecl_sum_ == NULL) throw SummaryFileNotFoundAtPathException(file_name.toLatin1().constData());
            populateKeyLists();
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
            if (!hasFieldVar(var_name))
                throw SummaryVariableDoesNotExistException("Field variable " + std::string(var_name.toLatin1().constData()) + " does not exist.");
            if (!HasReportStep(time_index))
                throw SummaryTimeStepDoesNotExistException("Time step does not exist");
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
            const stringlist_type *keys = ecl_sum_alloc_matching_general_var_list(ecl_sum_, NULL);
            std::cout << "Number of keys: " << stringlist_back(keys) << std::endl;
//            for (int i = 0; i < stringlist_get_size(keys); ++i) {
//                const char* key = stringlist_safe_iget(keys, i);
//                std::cout << key << std::endl;
//            }
        }

    }
}
