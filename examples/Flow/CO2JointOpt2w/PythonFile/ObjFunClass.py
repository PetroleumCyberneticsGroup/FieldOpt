# ------------------------------------------------------------------------------------
# This is a code for calculating the objective function. If you want to change
# the objective function calculations, this is the file to update.
# ------------------------------------------------------------------------------------

from resdata.summary import rd_sum
import numpy as np


# -------------------------------------------------------------------------------------
# Objective class and child class code for NPV calculation
# -------------------------------------------------------------------------------------

# objective function class
class ObjFunCla:
    # objective function value
    objfunval = 0
    # NPV value 
    NPV = 0
    # Well cost value. This variable should be updated if you want to include a well-cost.
    wellcost = 0
    # new objective function term result
    newvari = 0

    def __init__(self, simsummary, npvcomponent, wellsimdata, wellcostdata, newvari):
        """
        Initialize the objective function class. This is a prototype for this class.
        :param simsummary: reservoir summary case
        :param npvcomponent: all data needed for the NPV calculation, tuple or list type
        :param wellsimdata: simulation data that define the well (eg:toe-heel coordinates)
        :param wellcostdata: well cost data
        :param newvari: other variables
        """
        self.simsumdata = rd_sum.Summary(f'{simsummary}.UNSMRY')
        self.npvcomdata = npvcomponent
        self.wellsimdata = wellsimdata
        self.wellcostdata = wellcostdata
        self.newvari = newvari

    @classmethod
    # Calculate the final result of the objective function value.
    def objfunvalcal(cls):
        cls.objfunval = cls.NPV  # + cls.wellcost + cls.newvari( wait for update to int)
        return cls.objfunval


# NPV class
class NPVCla(ObjFunCla):
    def __init__(self, simsummary, npvcomponent, wellsimdata, wellcostdata, initaldata):
        """
        Child class of ObjFunCal, defined for NPV calculation (only the fluid flow part).
        :param simsummary:
        :param npvcomponent:
        :param wellsimdata:
        :param wellcostdata:
        :param initaldata:
        """
        super().__init__(simsummary, npvcomponent, wellsimdata, wellcostdata, initaldata)
        self.simsumdata = rd_sum.Summary(f'{simsummary}.UNSMRY')
        self.npvcomdata = npvcomponent
        self.NPVprolist = []  # NPV property list, e.g.: wellgasprod
        self.NPVprokeylist = []  # NPV property keyword list, e.g.: 'WGIT:INJ1'
        self.NPVintlist = []
        self.NPVfluprilist = []
        self.NPVdislist = []

    # NPV component data process for later calculation.
    def NPVcompro(self):

        self.NPVprolist = []  # NPV property list, e.g.:wellgasprod
        self.NPVprokeylist = []  # NPV property keyword list, e.g.: 'WGIT:INJ1'/'FGIT'
        self.NPVintlist = []
        self.NPVfluprilist = []
        self.NPVdislist = []

        for i in range(0, len(self.npvcomdata)):
            NPVpro = '{}_{}_{}'.format(self.npvcomdata[i].wellname, \
                                       self.npvcomdata[i].fluidtype.lower(), \
                                       self.npvcomdata[i].flowtype[:4].lower())
            if self.npvcomdata[i].datatype[0].upper() == 'W':
                NPVprokey = '{}{}{}T:{}'.format(self.npvcomdata[i].datatype[0].upper(), \
                                                self.npvcomdata[i].fluidtype[0].upper(), \
                                                self.npvcomdata[i].flowtype[0].upper(), \
                                                self.npvcomdata[i].wellname)
            else:
                NPVprokey = '{}{}{}T'.format(self.npvcomdata[i].datatype[0].upper(), \
                                             self.npvcomdata[i].fluidtype[0].upper(), \
                                             self.npvcomdata[i].flowtype[0].upper())

            NPVint = '1{}'.format(self.npvcomdata[i].interval[0].upper())
            NPVflupri = self.npvcomdata[i].fluidprice
            NPVdis = self.npvcomdata[i].discountfactor

            self.NPVprolist.append(NPVpro)
            self.NPVprokeylist.append(NPVprokey)
            self.NPVintlist.append(NPVint)
            self.NPVfluprilist.append(NPVflupri)
            self.NPVdislist.append(NPVdis)

        return self.NPVprolist, self.NPVprokeylist, self.NPVintlist, self.NPVfluprilist, self.NPVdislist

    @property
    def NPVsumcal(self):
        # NPV component processing method. This will assign the first values to the list.
        NPVCla.NPVcompro(self)
        NPVvalue = 0
        for numcomp in range(len(self.NPVprolist)):
            timerange = self.simsumdata.time_range(interval=self.NPVintlist[numcomp])
            NPVtimelist = []
            discoufactlist = []
            time_index = 1
            for date in timerange:
                if date <= self.simsumdata.end_date:
                    NPVtimelist.append(date)
            while time_index < len(NPVtimelist):
                if self.NPVintlist[numcomp] == '1Y':
                    yeardiscountfactor = 1 / ((1 + self.NPVdislist[numcomp]) ** (time_index - 1))
                    discoufactlist.append(yeardiscountfactor)
                    time_index += 1

                elif self.NPVintlist[numcomp] == '1M':
                    monthdiscountfactor = ((1 + self.NPVdislist[numcomp]) ** 0.082192) - 1
                    discountfactor = 1 / ((1 + monthdiscountfactor) ** (time_index - 1))
                    discoufactlist.append(discountfactor)
                    time_index += 1

            flowvolume = self.simsumdata.numpy_vector(key=self.NPVprokeylist[numcomp], time_index=NPVtimelist)
            cashflow = 0
            for numoftime in range(1, len(NPVtimelist)):
                flowvolumediff = flowvolume[numoftime] - flowvolume[numoftime - 1]
                discount_rate = discoufactlist[numoftime - 1]
                cashflow += flowvolumediff * self.NPVfluprilist[numcomp] * discount_rate
            NPVvalue += cashflow
        ObjFunCla.NPV = np.format_float_scientific(NPVvalue)
        return ObjFunCla.NPV


# Well cost class, currently an empty shell. Can be updated if well cost are to be included in NPV.
class WelCosCla(ObjFunCla):
    def __init__(self, simsummary, npvcomponent, wellsimdata, wellcostdata, initaldata):
        super().__init__(simsummary, npvcomponent, wellsimdata, wellcostdata, initaldata)
        pass
