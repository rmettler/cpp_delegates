FROM rust:1.79.0-alpine3.20 AS lcov2xml
RUN apk add musl-dev && \
    VERSION=1.0.5 && \
    wget -q https://github.com/mike-kfed/lcov2cobertura/archive/refs/tags/$VERSION.tar.gz && \
    tar -xf $VERSION.tar.gz && \
    rm $VERSION.tar.gz && \
    cd lcov2cobertura-$VERSION && \
    cargo build --release --bin lcov2xml && \
    mv target/release/lcov2xml / && \
    rm -r /lcov2cobertura-$VERSION/ && \
    apk del musl-dev

FROM alpine:3.20.2 AS latest
COPY --from=lcov2xml /lcov2xml /usr/bin
RUN apk add \
    xmlstarlet \
    cmake ninja-is-really-ninja \
    g++ \
    clang lld compiler-rt \
    llvm clang-extra-tools

FROM alpine:3.20.2 AS old_cmake
RUN apk add \
    xmlstarlet make gcompat && \
    CMAKE_VERSION=3.20.6 && \
    wget -q https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    chmod a+x cmake-$CMAKE_VERSION-Linux-x86_64.sh && \
    ./cmake-$CMAKE_VERSION-Linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-$CMAKE_VERSION-Linux-x86_64.sh

FROM old_cmake AS old_gcc
RUN apk add \
    g++=5.3.0-r0 --repository https://dl-cdn.alpinelinux.org/alpine/v3.4/main/

FROM old_cmake AS old_clang
RUN apk add \
    clang=4.0.0-r0 g++=6.3.0-r4 --repository https://dl-cdn.alpinelinux.org/alpine/v3.6/main/
