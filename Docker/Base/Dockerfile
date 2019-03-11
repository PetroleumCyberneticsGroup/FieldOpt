FROM ubuntu:16.04
MAINTAINER einar90 version: base

# Build using:
#   sudo docker build --no-cache -t einar90/fieldopt:base ~/path/to/FieldOpt/Docker/Base
#   sudo docker push einar90/fieldopt:base
#
# This container contains the base for FieldOpt, i.e. all
# depndencies, pre-compiled.
# The FieldOpt source-code is in this container, but not built.
# A different image should be created based on this to create
# functional FieldOpt images.
#
# This is still a work in progress. In order to work fully,
# the container still needs a simulator.


# Install dependencies from ubuntu repositories
RUN apt-get update && apt-get install -y git build-essential cmake qt5-default libboost-all-dev libhdf5-dev libopenmpi-dev libeigen3-dev 

# Clone the FieldOpt repo into the root /app directory
WORKDIR /app
RUN git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git

# cd into FieldOpt and get the submodules
WORKDIR /app/FieldOpt
RUN git checkout develop
RUN git submodule update --init --recursive

# Build the third party dependencies
WORKDIR /app/FieldOpt/FieldOpt/ThirdParty
RUN cmake -DCMAKE_BUILD_TYPE=Release . && make && make install

