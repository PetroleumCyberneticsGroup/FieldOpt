SET(RUNNER_HEADERS
	bookkeeper.h
	loggable.hpp
	logger.h
	runners/abstract_runner.h
	runners/ensemble_helper.h
	runners/main_runner.h
	runners/mpi_runner.h
	runners/oneoff_runner.h
	runners/overseer.h
	runners/serial_runner.h
	runners/synchronous_mpi_runner.h
	runners/worker.h
	runtime_settings.h
)

SET(RUNNER_SOURCES
	bookkeeper.cpp
	logger.cpp
	runners/abstract_runner.cpp
	runners/ensemble_helper.cpp
	runners/main_runner.cpp
	runners/mpi_runner.cpp
	runners/oneoff_runner.cpp
	runners/overseer.cpp
	runners/serial_runner.cpp
	runners/synchronous_mpi_runner.cpp
	runners/worker.cpp
	runtime_settings.cpp
)

SET(RUNNER_TESTS
	tests/test_resource_runner.hpp
	tests/test_bookkeeper.cpp
	tests/test_runtime_settings.cpp
)

