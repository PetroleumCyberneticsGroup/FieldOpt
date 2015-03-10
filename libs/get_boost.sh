#!/bin/sh
echo "Downloading boost ..."
wget -c 'http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.bz2/download'

echo "Uncompressing boost ..."
tar xf download

echo "Deleting downloaded tar ..."
rm download

echo "Configuring boost installer ..."
cd boost_1_57_0
./bootstrap.sh --with-libraries=mpi,serialization --prefix=./boost

echo "Compiling and installing ..."
./b2
