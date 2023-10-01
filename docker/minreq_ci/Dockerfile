FROM ubuntu:22.04

RUN apt update -y && \
    apt install --no-install-recommends -y software-properties-common gnupg && \
    gpg -k && gpg --no-default-keyring --keyring /tmp/ubuntu-xenial.gpg --keyserver keyserver.ubuntu.com --recv-keys 40976EAF437D05B5 && \
    gpg --keyring /tmp/ubuntu-xenial.gpg --export --output /etc/apt/trusted.gpg.d/ubuntu-xenial-imported.gpg && \
    rm /tmp/ubuntu-xenial.gpg && \
    add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial main" && \
    add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial universe" && \
    add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial-updates main" && \
    add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu/ xenial-updates universe" && \
    apt update && \
    apt install -y --no-install-recommends \
        wget \
        ninja-build clang-4.0 g++-5 \
        python3 python3-pip \
        && \
    pip3 install -U pytest && \
    apt autoremove -y software-properties-common gnupg && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

RUN CMAKE_VERSION=3.14.7 && \
    wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    chmod a+x cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    ./cmake-$CMAKE_VERSION-Linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-$CMAKE_VERSION-Linux-x86_64.sh
