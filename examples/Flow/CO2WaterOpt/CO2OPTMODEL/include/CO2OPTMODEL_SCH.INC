
WELSPECS
-- 1)   2)   3)   4)   5)     6)
   INJ1    G1    1   1    1715   GAS /
   PROD1   G2    6   6    1715   OIL /
/

-- 1) Well name
-- 2) Name of group to which well belongs
-- 3) I-location of well head or heel (x)
-- 4) J-location of well head or heel (y)
-- 5) Reference depth for bottom hole pressure
-- 6) Preferred phase for the well

-- ____________________________________________________
COMPDAT
-- 1)  2)  3) 4) 5)  6)    7)  8)  9)      10-13) 14)
   INJ1   1   1  1  1   OPEN  1*   1*  0.1905  4*     1*  /
   PROD1  6   6  1  1   OPEN  1*   1*  0.1905  4*     1*  /
/
-- 1) Well name
-- 2) I-location of connecting grid blocks
-- 3) J-location of connecting grid blocks
-- 4) K-location of upper connecting grid blocks
-- 5) K-location of lower connecting grid blocks
-- 6) Open/shut flag connection
-- 7) Sat table number for connection rel-perms
-- 8) Transmissibility factor for the connection
-- 9) Wellbore diameter at the connection 
--    (needed for well PI calculation);
--    def.: 1 ft = 0.3048 m  UNITS: m (METRIC)
--    ignored of not required. Required if 8) 
--    is zero(0.0). ft. def.: Undefined.
--    MLO: 0.1905m
-- 14) Eq. wellbore radius, UNITS: m (METRIC)



-- WELL GAS INJECTION PROPERTIES
--
-- WELL STREAM SOURCE MAKEUP SEP
-- NAME OPTION DEPTH GAS STAGE
--WINJGAS
  --INJ1 STREAM C02STREAM /





-- ____________________________________________________
 WCONPROD
-- 1)  2)    3)   4)  5)  6)  7)  8)  9)
   PROD1  OPEN  BHP  1*  1*  1*  1*  1*  160  /
/

-- 1) Well name
-- 2) Open/shut flag
-- 3) Control mode: ORAT cntr by oil rate, WRAT cntr by water rate,
--    LRAT cntr by liquid rate, RESV cntr by res. fluid volume rate target,
--    BHP cntr by BHP target, THP cntr by THP target.
-- 4) Oil rate target or upper limit.
-- 5) Water rate target or upper limit.
-- 6) Gas rate target or upper limit.
-- 7) Liquid rate target or upper limit.
-- 8) Reservoir fluid volume rate target or upper limit.
-- 9) BHP target or lower limit.
-- Note= since Bo~1, the surf-oil prod = res.oil prod

-- ____________________________________________________
WCONINJE
-- 1)     2)     3)    4)    5)        6)  7) 
-- INJ1   WATER  OPEN  BHP   1.00E+12  1*  5200 /
   INJ1   GAS  	 OPEN  RATE  1300      1*  220  /
-- INJ1   WATER  OPEN  RATE  1*        1*  250  /
/

-- 1) Well name
-- 2) Injector type: WATER or GAS
-- 3) Open/shut flag
-- 4) Control mode: RATE cntr by surface flow rate
--    (Item 5), RESV cntr by reservoir volume target 
--    rate (Item 6), BHP Controlled by BHP target 
--    (Item 7), THP Controlled by THP target (Item 8)
-- 5) Surface flow rate target or upper limit. For 
--    a multi-phase injector, this is the surface 
--    flow rate of the preferred phase of the well.
--    Units: [SM3/DAY]. Def. No target or limit.
-- 6) Reservoir fluid volume rate target or upper 
--    limit. Units: [RM3/DAY]. Def. No target or 
--    limit.
-- 7) BHP target or upper limit. It is recommended 
--    that this quantity should be set to a value 
--    not exceeding the highest pressure in the 
--    PVT tables, to ensure the tables are not 
--    extrapolated in the well. It is best not 
--    to default this quantity, as its value is 
--    used in calculating the well potential.
--    Def. 6803 atma, or 1.0E5 psia, or 6891 barsa.
-- 8) THP target or upper limit. Def. No target or 
--    limit.
-- 9) Injection well VFP table number.

