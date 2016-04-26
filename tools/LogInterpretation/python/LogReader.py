import csv

"""
Here we define functions and classes used to read the csv-formatted logs
written by FieldOpt. The logs are read into separate objects, which structure
the logs into headers and rows.
"""

def read_log(file_path):
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
    col = []
    for row in rows:
        col.append(row[col_nr])
    return col

def get_header_index(headers, header):
    for i in range(len(headers)):
        if headers[i] == header:
            return i

class CaseLog:
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class PropertyUuidNameMap:
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

    def name(self, id):
        for row in self.rows:
            if row[0] == id:
                return row[1]

class SimulationLog:
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class OptimizationLog:
    headers = []
    rows = []

    def __init__(self, file_path):
        self.file_path = file_path
        (self.headers, self.rows) = read_log(file_path)

    def column(self, col_name):
        index = get_header_index(self.headers, col_name)
        return extract_column(self.rows, index)

class LogReader:
    def __init__(self, output_dir_path):
        self.output_dir_path = output_dir_path
        self.property_uuid_name_map = PropertyUuidNameMap(output_dir_path + "/log_property_uuid_name_map.csv")
        self.case_log = CaseLog(output_dir_path + "/log_cases.csv")
        self.simulation_log = SimulationLog(output_dir_path + "/log_simulation.csv")
        self.optimization_log = OptimizationLog(output_dir_path + "/log_optimization.csv")

# test_path = "/home/einar/Documents/GitHub/PCG/fieldopt_output/adgprs3"
# reader = LogReader(test_path)
