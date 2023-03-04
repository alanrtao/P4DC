#!/bin/bash

sudo apt -y install g++ cmake zlib1g-dev libarchive-dev libssl3 libsodium23 libopus0

rm -rf DPP
dpkg-deb -xv libdpp-10.0.23-linux-x64.deb DPP_
mv DPP_/usr DPP
rm -rf DPP_

rm -rf SQLiteCpp Re2

mkdir SQLiteCpp Re2

wget "https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.2.1.tar.gz" -O SQLiteCpp.tar.gz
tar -xzf "SQLiteCpp.tar.gz" -C SQLiteCpp --strip-components 1

wget https://github.com/google/re2/archive/refs/tags/2023-03-01.tar.gz -O Re2.tar.gz
tar -xzf "Re2.tar.gz" -C Re2 --strip-components 1

rm *.tar.gz

echo -e "Clone finished with libs folder initialized as:\n\t $(ls)"