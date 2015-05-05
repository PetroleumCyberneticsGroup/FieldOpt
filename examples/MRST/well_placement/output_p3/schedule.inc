WELSPECS
--     WELL   GROUP LOCATION  BHP   PI
--     NAME   NAME    I  J   DEPTH DEFN
       'i1'   'grp1'     2  5    1*  'WAT'  /
       'p1'   'grp1'     5  3    1*  'WAT'  /

 / 

COMPDAT
--     WELL     -LOCATION- OPEN/ SAT CONN  WELL
--     NAME     I  J K1 K2 SHUT  TAB FACT  DIAM
       'i1'     2  5  1  5  'OPEN' 2*       0.3/
       'p1'     5  3  1  6  'OPEN' 2*       0.3/

 / 

-- CONTROL INTERVAL 1
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            200/
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   200/
/

TSTEP
0.5 1 2 4 8 16 32 36.5 
/

-- CONTROL INTERVAL 2
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            200/
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   200/
/

TSTEP
0.5 1 2 4 8 16 32 36.5 
/

