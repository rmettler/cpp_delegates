FROM ubuntu:20.04

RUN apt update -y && \
    apt install --no-install-recommends -y software-properties-common && \
    apt-add-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial main" && \
    apt-add-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial universe" && \
    apt-add-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial-updates main" && \
    apt-add-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial-updates universe" && \
    apt update && \
    apt install -y --no-install-recommends \
        wget \
        ninja-build clang-4.0 g++-5 \
        && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

RUN CMAKE_VERSION=3.14.7 && \
    wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    chmod a+x cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    ./cmake-$CMAKE_VERSION-Linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-$CMAKE_VERSION-Linux-x86_64.sh
