FROM ubuntu:16.04
MAINTAINER einar90 version: 0.1


RUN apt-get update && apt-get install -y git build-essential cmake qt5-default libboost-all-dev libhdf5-dev libopenmpi-dev libeigen3-dev 


WORKDIR /app
RUN git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git

WORKDIR /app/FieldOpt
RUN git checkout feature/ResInsight_WIC_Integration
RUN git submodule update --init --recursive

WORKDIR /app/FieldOpt/FieldOpt/ThirdParty
RUN cmake . && make && make install

WORKDIR /app/FieldOpt/FieldOpt/ThirdParty/opm-common
RUN git checkout release/2018.04
RUN cmake . && make && make install

WORKDIR /app/FieldOpt/FieldOpt
RUN mkdir cmake-build-debug

WORKDIR /app/FieldOpt/FieldOpt/cmake-build-debug
RUN cmake ..  && make

WORKDIR /app/FieldOpt/FieldOpt/cmake-build-debug/bin
RUN ./FieldOpt --help

# Set the working directory to /app
#WORKDIR /app

# Copy the current directory contents into the container at /app
#ADD . /app

# Install any needed packages specified in requirements.txt
#RUN pip install --trusted-host pypi.python.org -r requirements.txt

# Make port 80 available to the world outside this container
#EXPOSE 80

# Define environment variable
#ENV NAME World

# Run app.py when the container launches
#CMD ["python", "app.py"]

