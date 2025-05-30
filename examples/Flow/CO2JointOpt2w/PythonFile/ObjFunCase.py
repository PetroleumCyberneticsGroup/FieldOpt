import sys
from ObjFunClass import ObjFunCla,NPVCla
from dataclasses import dataclass


@dataclass(frozen=True)
class NPVEntry:
    datatype: str
    wellname: str
    fluidtype: str
    flowtype: str
    interval: str
    fluidprice: float
    discountfactor: float


optimal_model_dir = sys.argv[1]
py_result_dir = sys.argv[2]
unsmary_dir = '{}/CO2OPT2W'.format(optimal_model_dir)

NPVcomponent = (
    NPVEntry('Field', 'INJ1', 'Gas',  'Injection',  'yearly',  15,   0.08),
    NPVEntry('Field', 'PROD1', 'Oil', 'Production', 'yearly', -10,   0.08),
    NPVEntry('Field', 'PROD1', 'Gas', 'Production', 'yearly', -500,  0.08)
)


thipar = foupar = fifpar = 0
NPVcase = NPVCla(unsmary_dir, NPVcomponent, thipar, foupar, fifpar)
Objcase = ObjFunCla(unsmary_dir, NPVcomponent, thipar, foupar, fifpar)

# call the NPV calculation property.
NPVcase.NPVsumcal
Objcase.objfunvalcal()

objective_result = Objcase.objfunvalcal()

# write out the objective function result to a txt file
py_result_path = '{}/PythonObjeResult.txt'.format(py_result_dir)
with open(py_result_path, 'w') as file:
     file.write(objective_result)
