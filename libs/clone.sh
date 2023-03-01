#!/bin/bash

sudo apt -y install g++ cmake zlib1g-dev libarchive-dev libssl3 libsodium23 libopus0

rm -rf DPP
dpkg-deb -xv libdpp-10.0.23-linux-x64.deb DPP_
mv DPP_/usr DPP
rm -rf DPP_

rm -rf SQLiteCpp

mkdir SQLiteCpp

wget https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.2.1.tar.gz
tar -xzf 3.2.1.tar.gz -C SQLiteCpp --strip-components 1

rm *.tar.gz

echo -e "Clone finished with libs folder initialized as:\n\t $(ls)"