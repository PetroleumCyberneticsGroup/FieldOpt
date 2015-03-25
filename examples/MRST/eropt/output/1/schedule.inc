WELSPECS
--     WELL   GROUP LOCATION  BHP   PI
--     NAME   NAME    I  J   DEPTH DEFN
       'i1'   'grp1'     5  8    1*  'WAT'  /
       'i2'   'grp1'     20  1    1*  'WAT'  /
       'p1'   'grp1'     1  1    1*  'WAT'  /
       'p2'   'grp1'     1  10    1*  'WAT'  /
       'p3'   'grp1'     20  10    1*  'WAT'  /
       'p4'   'grp1'     14  3    1*  'WAT'  /
       'p5'   'grp1'     8  2    1*  'OIL'  /

 / 

COMPDAT
--     WELL     -LOCATION- OPEN/ SAT CONN  WELL
--     NAME     I  J K1 K2 SHUT  TAB FACT  DIAM
       'i1'     5  8  1  5  'OPEN' 2*       0.3/
       'i2'     20  1  1  5  'OPEN' 2*       0.3/
       'p1'     1  1  1  5  'OPEN' 2*       0.3/
       'p2'     1  10  1  5  'OPEN' 2*       0.3/
       'p3'     20  10  1  5  'OPEN' 2*       0.3/
       'p4'     14  3  1  5  'OPEN' 2*       0.3/
       'p5'     8  2  1  5  'OPEN' 2*       0.3/

 / 

-- CONTROL INTERVAL 1
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            299.623/
        'p2'    'OPEN'  'BHP'   5*                            313.616/
        'p3'    'OPEN'  'BHP'   5*                            294.938/
        'p4'    'OPEN'  'BHP'   5*                            291.015/
        'p5'    'OPEN'  'ORAT'  0  5*    /
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   177.525/
        'i2'   'WATER' 'OPEN' 'RATE'   130/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 34 
/

-- CONTROL INTERVAL 2
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            313.594/
        'p2'    'OPEN'  'BHP'   5*                            328.141/
        'p3'    'OPEN'  'BHP'   5*                            310.44/
        'p4'    'OPEN'  'BHP'   5*                            314.829/
        'p5'    'OPEN'  'ORAT'  0  5*    /
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   142.661/
        'i2'   'WATER' 'OPEN' 'RATE'   130/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 34 
/

-- CONTROL INTERVAL 3
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            281.519/
        'p2'    'OPEN'  'BHP'   5*                            314.562/
        'p3'    'OPEN'  'BHP'   5*                            277.766/
        'p4'    'OPEN'  'BHP'   5*                            280.442/
        'p5'    'OPEN'  'ORAT'  0  5*    /
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   160.683/
        'i2'   'WATER' 'OPEN' 'RATE'   130/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 34 
/

-- CONTROL INTERVAL 4
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            274.477/
        'p2'    'OPEN'  'BHP'   5*                            317.595/
        'p3'    'OPEN'  'BHP'   5*                            267.372/
        'p4'    'OPEN'  'BHP'   5*                            271.119/
        'p5'    'OPEN'  'ORAT'  0  5*    /
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   183.471/
        'i2'   'WATER' 'OPEN' 'RATE'   130/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 49 
/

-- CONTROL INTERVAL 5
WCONPROD
--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP
--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE
        'p1'    'OPEN'  'BHP'   5*                            277.563/
        'p2'    'OPEN'  'BHP'   5*                            327.316/
        'p3'    'OPEN'  'BHP'   5*                            275.162/
        'p4'    'OPEN'  'BHP'   5*                            283.161/
        'p5'    'OPEN'  'ORAT'  0  5*    /
/ 

WCONINJE
--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP
--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET
        'i1'   'WATER' 'OPEN' 'RATE'   150.322/
        'i2'   'WATER' 'OPEN' 'RATE'   130/
/

TSTEP
0.5 1 2 4 8 16 32 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 31.5 
/

