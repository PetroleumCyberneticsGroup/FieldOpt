SET(WELLINDEXCALCULATION_HEADERS
	WellDefinition.h
	intersected_cell.h
	wicalc_rixx.h
)

SET(WELLINDEXCALCULATION_SOURCES
	intersected_cell.cpp
	wicalc_rixx.cpp
)

SET(WELLINDEXCALCULATION_TESTS
	tests/test_intersected_cells.cpp
	tests/test_single_cell_wellindex.cpp
)
