import LogReader as lr
from datetime import datetime

def extract_well_names(variables):
    """
    Extract the names of all wells defined in the provided list of variables.
    This works by looking at the variable naming pattern for spline-defined
    wells: var_name#well_name#heel/toe#x/y/z.
    :param variables: The list of all variables for a case
    :return: List of all unique well names found in the variable list.
    """
    names = []
    for var in variables:
        name = var.name.split('#')[1]
        if name not in names:
            names.append(name)
    return names


def extract_well_endpoints(variables, well_name):
    """
    Extract the heel and toe coordinates for a well with the specified name.
    :param variables: The list of all variables for a case (or just the variables for the well)
    :param well_name: The name of the well to extract the endpoints for.
    :return: A tuple (heel, toe) containing the [x, y, z] coordinates for the well's heel and toe.
    """
    heel = [0.0, 0.0, 0.0]
    toe = [0.0, 0.0, 0.0]
    for var in variables:
        if var.name.split('#')[1] != well_name:
            continue
        if var.name.split('#')[2] == 'heel':
            if var.name.split('#')[3] == 'x':
                heel[0] = var.value
            elif var.name.split('#')[3] == 'y':
                heel[1] = var.value
            elif var.name.split('#')[3] == 'z':
                heel[2] = var.value
        if var.name.split('#')[2] == 'toe':
            if var.name.split('#')[3] == 'x':
                toe[0] = var.value
            elif var.name.split('#')[3] == 'y':
                toe[1] = var.value
            elif var.name.split('#')[3] == 'z':
                toe[2] = var.value
    return (heel, toe)

class Variable:
    """
    The Variable class holds the information about the state of a variable as
    logged by FieldOpt. It includes the UUID of the variable; the name of
    the variable; and the value of the variable.
    """
    def __init__(self, id, value, property_uuid_name_map):
        self.id = id
        self.value = float(value)
        self.name = property_uuid_name_map.name(id)


class Well:
    def __init__(self, variables, well_name):
        self.name = well_name
        (self.heel, self.toe) = extract_well_endpoints(variables, well_name)


class Case:
    def __init__(self, caselog_row, property_uuid_name_map):
        self.timestamp = datetime.strptime(caselog_row[0], "%Y-%m-%dT%H:%M:%SZ")
        self.uuid = caselog_row[1]
        self.objective_function_value = float(caselog_row[3])
        self.variables = []
        i = 4
        while i < len(caselog_row):
            self.variables.append(Variable(caselog_row[i], caselog_row[i+1], property_uuid_name_map))
            i = i+2
        self.well_names = extract_well_names(self.variables)
        self.wells = []
        for name in self.well_names:
            self.wells.append(Well(self.variables, name))

    def get_well(self, name):
        for well in self.wells:
            if well.name == name:
                return well


class CaseContainer:
    def __init__(self, case_log, property_uuid_name_map):
        self.evaluated_case_rows = []
        self.evaluated_cases = []
        for row in case_log.rows:
            if row[2] == 'true':
                self.evaluated_case_rows.append(row)
                self.evaluated_cases.append(Case(row, property_uuid_name_map))

    def get_case(self, id):
        for case in self.evaluated_cases:
            if case.uuid == id:
                return case

class Optimizer:
    def __init__(self, optimizer_log, case_container):
        self.best_case_id_pr_iteration = []
        cur_iter = 1
        for i in range(len(optimizer_log.rows)):
            if int(optimizer_log.rows[i][1]) != cur_iter: # Reached next iteration
                self.best_case_id_pr_iteration.append(optimizer_log.rows[i-1][5])
                cur_iter = cur_iter + 1
            if i == len(optimizer_log.rows ) - 1:
                self.best_case_id_pr_iteration.append(optimizer_log.rows[i][5])

        self.best_case_pr_iteration = []
        for i in range(len(self.best_case_id_pr_iteration)):
            id = self.best_case_id_pr_iteration[i]
            self.best_case_pr_iteration.append(case_container.get_case(id))
