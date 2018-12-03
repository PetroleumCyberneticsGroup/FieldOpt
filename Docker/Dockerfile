FROM ubuntu:16.04
MAINTAINER einar90 version: 0.1

# This is still a work in progress. In order to work fully,
# the container still needs a simulator.
# Flow will be added as a default (other simulators may
# of course also be added manually by the user).


# Install dependencies from ubuntu repositories
RUN apt-get update && apt-get install -y git build-essential cmake qt5-default libboost-all-dev libhdf5-dev libopenmpi-dev libeigen3-dev 

# Install the Flow reservoir simulator
RUN apt-get install -y software-properties-common
RUN apt-add-repository -y ppa:opm/ppa && apt-get update
RUN apt-get install -y mpi-default-bin
RUN apt-get install -y libopm-simulators-bin


# Clone the FieldOpt repo into the root /app directory
WORKDIR /app
RUN git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git

# cd into FieldOpt and get the submodules
WORKDIR /app/FieldOpt
RUN git checkout develop
RUN git submodule update --init --recursive

# Build the third party dependencies
WORKDIR /app/FieldOpt/FieldOpt/ThirdParty
RUN cmake . && make && make install

# Build the opm-common dependency
WORKDIR /app/FieldOpt/FieldOpt/ThirdParty/opm-common
RUN git checkout release/2018.04
RUN cmake . && make && make install

# Build FieldOpt
RUN mkdir /app/FieldOpt/FieldOpt/cmake-build-debug
WORKDIR /app/FieldOpt/FieldOpt/cmake-build-debug
RUN cmake ..  && make
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

