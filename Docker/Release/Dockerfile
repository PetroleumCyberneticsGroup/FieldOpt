FROM einar90/fieldopt:base
MAINTAINER einar90 version: 1.0-1

# Build using:
#   sudo docker build --no-cache -t einar90/fieldopt:release ~/path/to/FieldOpt/Docker/Release
#   sudo docker push einar90/fieldopt:release
#
# This container is based on the fieldopt:base image,
# which contains the base Ubuntu 16.04 OS as well as
# repository dependencies and compiled dependencies
# from GitHub (e.g. OPM).
#
# This is still a work in progress. In order to work fully,
# the container still needs a simulator.
# Flow will be added as a default (other simulators may
# of course also be added manually by the user).


# Build FieldOpt
WORKDIR /app/FieldOpt
RUN git pull
RUN git checkout master
RUN mkdir /app/FieldOpt/FieldOpt/cmake-build-debug
WORKDIR /app/FieldOpt/FieldOpt/cmake-build-debug
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && make
WORKDIR /app/FieldOpt/FieldOpt/cmake-build-debug/bin

# Create output directory
RUN mkdir /app/fieldopt-output

# Define environment variables needed by example runscripts
ENV NAME FieldOpt
ENV FIELDOPT_BIN="/app/FieldOpt/FieldOpt/cmake-build-debug/bin/FieldOpt"
ENV FIELDOPT_BUILD="/app/FieldOpt/FieldOpt/cmake-build-debug/bin/"
ENV FIELDOPT_OUT="/app/fieldopt-output"
ENV PATH=$PATH:"/app/FieldOpt/FieldOpt/cmake-build-debug/bin/"

# Execute FieldOpt with the help argument
CMD ["FieldOpt"]

