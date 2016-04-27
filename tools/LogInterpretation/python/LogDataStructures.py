import LogReader as lr
from datetime import datetime

"""
This Package contains data structures and methods used to facilitate
easier access to the data stored in the FieldOpt logs.
"""

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
    def __init__(self, uuid, value, property_uuid_name_map_log):
        """
        Initialize a variable.
        :param uuid: The variable's UUID.
        :param value: The value of the variable.
        :param property_uuid_name_map_log: The PropertyUuidNameMapLog object from the LogReader, used
        to find variable names.
        """
        self.uuid = uuid
        self.value = float(value)
        self.name = property_uuid_name_map_log.name(uuid)


class Well:
    """
    The Well class holds the name of a well, as well as the (x, y, z) coordinates for its heel and toe.
    """
    def __init__(self, variables, well_name):
        """
        Initialize a well.
        :param variables: All variables from a case.
        :param well_name: The name of the well.
        """
        self.name = well_name
        (self.heel, self.toe) = extract_well_endpoints(variables, well_name)


class Case:
    """
    The Case class holds all information about a case, including timestamp; case UUID; objective
    function value; a list of all variables; and a list of all the wells defined by the case
    variables and their names.
    """
    def __init__(self, caselog_row, property_uuid_name_map_log):
        """
        Initialize a case.
        :param caselog_row: A single row from the CaseLog, defining a Case.
        :param property_uuid_name_map_log: The PropertyUuidNameMapLog object from the LogReader,
        used to get variable names.
        """
        self.timestamp = datetime.strptime(caselog_row[0], "%Y-%m-%dT%H:%M:%SZ")
        self.uuid = caselog_row[1]
        self.objective_function_value = float(caselog_row[3])
        self.variables = []
        i = 4
        while i < len(caselog_row):
            self.variables.append(Variable(caselog_row[i], caselog_row[i+1], property_uuid_name_map_log))
            i = i+2
        self.well_names = extract_well_names(self.variables)
        self.wells = []
        for name in self.well_names:
            self.wells.append(Well(self.variables, name))

    def get_well(self, name):
        """
        Get a Well object, containing structured information about a well.
        :param name: The name of the well to get.
        :return: A Well object representing the requested well.
        """
        for well in self.wells:
            if well.name == name:
                return well


class CaseContainer:
    """
    A container class for all Cases.
    """
    def __init__(self, case_log, property_uuid_name_map_log):
        """
        Initialize the Case Container. This only need to be called once per run. This will
        also initialize all Cases, Variables and Wells for the run.
        :param case_log: The CaseLog object from the LogReader.
        :param property_uuid_name_map_log: The PropertyUuidNameMapLog object from the LogReader, used
        to get the names of variables from their UUIDs.
        """
        self.evaluated_case_rows = []
        self.evaluated_cases = []
        for row in case_log.rows:
            if row[2] == 'true':
                self.evaluated_case_rows.append(row)
                self.evaluated_cases.append(Case(row, property_uuid_name_map_log))
            elif row == case_log.rows[-1]:
                self.evaluated_cases.append(Case(row, property_uuid_name_map_log))
                print('WARNING: Appended an unevaluated latest case.')

    def get_case(self, uuid):
        """
        Get the Case object with the specified UUID.
        :param uuid: The UUID of the case.
        :return: A Case object.
        """
        for case in self.evaluated_cases:
            if case.uuid == uuid:
                return case

class Optimizer:
    """
    The Optimizer object holds the best cases for each iteration in the optimization process.
    """
    def __init__(self, optimizer_log, case_container):
        """
        Initialize the Optimizer object. This must be initialized _after_ the CaseContainer object.
        :param optimizer_log: The OptimizerLog from the LogReader.
        :param case_container: The CaseContainer object.
        """
        self.current_iteration = optimizer_log.rows[-1][1]
        self.number_of_evaluated_cases = optimizer_log.rows[-1][2]
        self.number_of_queued_cases_in_iteration = optimizer_log.rows[-1][3]
        self.number_of_evaluated_cases_in_iteration = optimizer_log.rows[-1][4]
        self.current_step_length = optimizer_log.rows[-1][7]

        self.best_case_uuid_pr_iteration = []
        cur_iter = 1
        for i in range(len(optimizer_log.rows)):
            if int(optimizer_log.rows[i][1]) != cur_iter: # Reached next iteration
                self.best_case_uuid_pr_iteration.append(optimizer_log.rows[i - 1][5])
                cur_iter = cur_iter + 1
            if i == len(optimizer_log.rows ) - 1:
                self.best_case_uuid_pr_iteration.append(optimizer_log.rows[i][5])

        self.best_case_pr_iteration = []
        for i in range(len(self.best_case_uuid_pr_iteration)):
            id = self.best_case_uuid_pr_iteration[i]
            self.best_case_pr_iteration.append(case_container.get_case(id))

class Simulator:
    """
    The Simulator class holds information about simulator execution time for each simulated case.
    """
    def __init__(self, simulation_log):
        """
        Initialize a Simulator object.
        :param simulation_log: The SimulationLog from the LogReader package.
        """
        self.durations = []
        self.case_uuids = []
        for row in simulation_log.rows:
            self.case_uuids.append(row[-1])
            self.durations.append(int(row[-2]))
