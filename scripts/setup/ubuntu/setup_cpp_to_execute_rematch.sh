#!/bin/bash
# Update CMAKE to the latest version
sudo apt remove --purge --auto-remove cmake
sudo apt update && \
sudo apt install -y software-properties-common lsb-release && \
sudo apt clean all
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
sudo apt update
sudo apt install -y kitware-archive-keyring
sudo rm /etc/apt/trusted.gpg.d/kitware.gpg
sudo apt update
sudo apt install -y cmake
sudo apt-get install -y build-essential

# Install catch2
git clone https://github.com/catchorg/Catch2.git
cd Catch2/
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
cd ..
rm -rf Catch2

# Update compiler to support c++20
sudo apt install clang
