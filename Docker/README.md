# Docker

This directory contains the `Dockerfile` used to build
a Docker image for FieldOpt that can be executed on
any operating system.

In order to build and/or run the image, you need to
install [Docker](https://www.docker.com/get-started).
On Ubuntu, this can be done by executing
```
sudo apt-get install docker.io
```

To build the image, from this directory (or any other
directory containing a copy of this Dockerfile),
execute
```
sudo docker build -t fieldopt .
```
This will create a local docker image named `fieldopt`.
Call `sudo docker image ls` to list all your local
images.

To run the image, call
```
sudo docker run -it fieldopt
```
If everything was successful, the output of
`./FieldOpt --help` should be printed.

## Image Details

The image is based on Ubuntu 16.04 with all necessary
dependencies installed.
During setup, FieldOpt and the ThirdParty dependencies
will all be cloned and compiled.

To change which version of FieldOpt to build, edit
the lines in the Dockerfile that have a `git checkout`
call.
