NORNE_C5_3P_EQUIL_RSRV.DATA

    Grid/petroprops: Norne with all except: FAULTS, FAULTMULT, FLUXNUM, MULTREGT, MINPV, minor grid adjustment (ACTNUM away disconnected block) that is probably unnecessary.
    Also commented out MULTZ_JUN_05_MOD.INC for local barriers.
    Fluids: Norne fluid, but 1 region
    Wells: 2 injection wells, 3 producers (NOT original SCHEDULE).
    Time span of Simulation: 2000 days run.

    The faults should be easily added and kind of tested before.


NORNE_C5_3P_EQUIL.DATA

    Similar to NORNE_C5_3P_EQUIL_RSRV.DATA except using dead gas and dead oil with some hand-made PVDO and PVDG.
