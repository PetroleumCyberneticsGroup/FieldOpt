import csv

"""
In this package we define functions and classes used to read the csv-formatted logs
written by FieldOpt. The logs are read into separate objects, which structure
the logs into headers and rows.
"""

def read_log(file_path):
    """
    Read a CSV log file, extracting the headers as a 1D array,
    and the rows as a 2D array.
    :param file_path: Path to CSV file.
    :return: Headers and rows as arrays.
    """
    headers = []
    rows = []
    with open(file_path, newline='') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=',')
        for row in csvreader:
            if (len(row) > 1):
                rows.append(row)
        headers = rows.pop(0)
    return (headers, rows)

def extract_column(rows, col_nr):
    """
    Extract a column from a CSV file.
    :param rows: A 2D array of rows from which to extract a column.
    :param col_nr: The integer index of the column to extract.
    :return: A 1D array containing the values in the column.
    """
    col = []
    for row in rows:
        col.append(row[col_nr])
    return col

def get_header_index(headers, header):
    """
    Get the integer index of the column with the specified header.
    :param headers: The 1D array containing the headers.
    :param header: The specific header to get the index for.
    :return:
    """
    for i in range(len(headers)):
        if headers[i] == header:
            return i

class CaseLog:
    """
    Reads the log_cases.csv file, and separates it into headers and rows.
    """
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class PropertyUuidNameMapLog:
    """
    Reads the log_property_uuid_name_map.csv file, and separates it into headers and rows.
    """
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

    def name(self, uuid):
        """
        Get the name of the variable with the specified UUID.
        :param uuid: The UUID of the variable to get the name of.
        :return: The name of the variable.
        """
        for row in self.rows:
            if row[0] == uuid:
                return row[1]

class SimulationLog:
    """
    Reads the log_simulation.csv file and splits it into headers and rows.
    """
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class OptimizationLog:
    """
    Reads the log_optimization.csv file and splits it into heades and rows.
    """
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class LogReader:
    """
    Read all the log files and create the appropriate objects for them.
    """
    def __init__(self, output_dir_path):
        self.output_dir_path = output_dir_path
        self.property_uuid_name_map = PropertyUuidNameMapLog(output_dir_path + "/log_property_uuid_name_map.csv")
        self.case_log = CaseLog(output_dir_path + "/log_cases.csv")
        self.simulation_log = SimulationLog(output_dir_path + "/log_simulation.csv")
        self.optimization_log = OptimizationLog(output_dir_path + "/log_optimization.csv")
