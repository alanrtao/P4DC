#!/bin/sh

# git clone https://github.com/SRombauts/SQLiteCpp.git
# git clone https://github.com/brainboxdotcc/DPP.git

rm -rf DPP
dpkg-deb -xv libdpp-10.0.22-linux-x64.deb DPP_
mv DPP_/usr DPP
rm -rf DPP_
sudo apt install g++ gdb cmake zlib1g-dev libarchive-dev libssl-dev
