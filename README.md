# HarborCLS

## Pre-requisites ##

### Installing latest version of CMake ###
```
sudo apt-get install snapd
sudo snap install cmake --classic
```

### Installing C++23 and gcc 13 ###
https://developernote.com/2023/08/installing-gcc13-on-ubuntu-22-04/

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-13 g++-13
ll /usr/bin/gcc-13
ll /usr/bin/g++-13
update-alternatives --display gcc
ll /etc/alternatives/g*
sudo update-alternatives --remove-all gcc
sudo update-alternatives --remove-all g++
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 10 --slave /usr/bin/g++ g++ /usr/bin/g++-13
g++ --version
gcc --version
```

### Installing SSL Support ###
```
sudo apt-get install libssl-dev
```

## Third party libraries used ##
### Json reader: Nlohmann's Json ###
https://github.com/nlohmann/jsonTo

### Inversion of Control Container: Ybainier's Hypodermic ###
https://github.com/ybainier/Hypodermic

### googletest ###
https://github.com/google/googletest