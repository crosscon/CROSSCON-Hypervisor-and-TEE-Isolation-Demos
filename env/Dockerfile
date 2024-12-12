# Use Debian base image
FROM debian:12.8

# Set environment variables
ENV MAKE_VERSION=4.2.1
ENV WORKDIR_PATH=/work

# Set working directory
WORKDIR $WORKDIR_PATH

# Update package lists, install necessary dependencies
RUN apt-get update && \
    apt-get install -y \
    git \
    wget \
    build-essential \
    libfdt1 \
    libyaml-0-2 \
    gdebi-core \
    libuv1 \
    procps \
    librhash0 \
    libarchive13 \
    libc6 \
    libcurl4 \
    libexpat1 \
    libgcc-s1 \
    binutils \
    flex \
    bison \
    pkg-config \
    openssl \
    libssl-dev \
    cpio \
    unzip \
    rsync \
    bc \
    device-tree-compiler \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    python3-pyelftools  \
    gcc-riscv64-linux-gnu \
    g++-riscv64-linux-gnu \
    binutils-riscv64-linux-gnu

# Install Make 4.2.1
RUN wget http://ftp.pl.debian.org/debian/pool/main/m/make-dfsg/make_4.2.1-1.2_amd64.deb \
    && dpkg -i make_4.2.1-1.2_amd64.deb \
    && rm -f make_4.2.1-1.2_amd64.deb

# Install dtc 1.6.1
COPY patches/dtc $WORKDIR_PATH/patches/dtc/

RUN git clone https://salsa.debian.org/crosstoolchain-team/device-tree-compiler.git \
    && cd device-tree-compiler \
    && git checkout debian/1.5.0-2 \
    && git apply $WORKDIR_PATH/patches/dtc/dtc-patch.patch \
    && make install \
    && rm -rf $WORKDIR_PATH/device-tree-compiler

# Install libssl1.1 (requirement for mkimage)
RUN wget http://ftp.pl.debian.org/debian/pool/main/o/openssl/libssl1.1_1.1.1w-0+deb11u1_amd64.deb \
    && dpkg -i libssl1.1_1.1.1w-0+deb11u1_amd64.deb \
    && rm -f libssl1.1_1.1.1w-0+deb11u1_amd64.deb

# Install mkimage 20.10
RUN git clone https://github.com/u-boot/u-boot.git \
    && cd u-boot \
    && git checkout v2020.10 \
    && make tools-only_defconfig \
    && make tools \
    && cp tools/mkimage /usr/local/bin \
    && chmod +x /usr/local/bin/mkimage

# Install cmake-data 3.25
RUN wget http://ftp.pl.debian.org/debian/pool/main/c/cmake/cmake-data_3.25.1-1~bpo11+1_all.deb \
    && dpkg -i cmake-data_3.25.1-1~bpo11+1_all.deb \
    && rm -f cmake-data_3.25.1-1~bpo11+1_all.deb

# Install libjsoncpp24
RUN wget http://ftp.pl.debian.org/debian/pool/main/libj/libjsoncpp/libjsoncpp24_1.9.4-4_amd64.deb \
    && dpkg -i libjsoncpp24_1.9.4-4_amd64.deb \
    && rm -f libjsoncpp24_1.9.4-4_amd64.deb

# Install Cmake 3.20.0
COPY patches/cmake $WORKDIR_PATH/patches/cmake/

RUN wget https://cmake.org/files/v3.20/cmake-3.20.0.tar.gz \
    && tar -xvf cmake-3.20.0.tar.gz \
    && cd cmake-3.20.0 \
    && git apply $WORKDIR_PATH/patches/cmake/001-search-path.diff \
    && git apply $WORKDIR_PATH/patches/cmake/003-libuv-application-services.diff \
    && git apply $WORKDIR_PATH/patches/cmake/custom-application-services.patch

RUN cd cmake-3.20.0 \
    && ./bootstrap \
    && make install \
    && cd .. && rm -rf cmake-3.20.0

# Install ninja 1.10.1
RUN wget http://ftp.pl.debian.org/debian/pool/main/n/ninja-build/ninja-build_1.10.1-1_amd64.deb \
    && dpkg -i ninja-build_1.10.1-1_amd64.deb \
    && rm -f ninja-build_1.10.1-1_amd64.deb

# Install BASH
RUN apt install bash -y
SHELL ["/bin/bash", "-c"]

# Update PATH
COPY files/env.sh $WORKDIR_PATH/

# Setup CROSSCON repository
RUN git clone https://github.com/crosscon/CROSSCON-Hypervisor-and-TEE-Isolation-Demos.git \
    && mv CROSSCON-Hypervisor-and-TEE-Isolation-Demos $WORKDIR_PATH/crosscon

COPY files/.gitmodules $WORKDIR_PATH/crosscon/.gitmodules

RUN cd $WORKDIR_PATH/crosscon && \
    git submodule init && \
    git submodule update --depth 1

# Fix missing "cryptography" python module
RUN apt install python3-cryptography -y

# copy script for rpi4-ws demo
COPY files/build.sh $WORKDIR_PATH/

# Install Linaro AArch64 bare-metal toolchain (aarch64-none-elf)
RUN wget https://releases.linaro.org/archive/14.11/components/toolchain/binaries/aarch64-none-elf/gcc-linaro-4.9-2014.11-x86_64_aarch64-elf.tar.xz \
    && tar -xf gcc-linaro-4.9-2014.11-x86_64_aarch64-elf.tar.xz -C /opt \
    && rm -f gcc-linaro-4.9-2014.11-x86_64_aarch64-elf.tar.xz

# Install Arm GNU toolchain
RUN wget -O aarch64-none-elf.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf.tar.xz?rev=981d8f7e91864070a466d852589598e2&hash=8D5397D4E41C99A96989ED813E8E95F0" \
    && unxz aarch64-none-elf.tar.xz \
    && tar -xvf aarch64-none-elf.tar \
    && rm -f aarch64-none-elf.tar

# Install ARM Developer Toolchain (AArch64-none-elf, Linux version)
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz \
&& tar -xf arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz -C /opt \
&& rm -f arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz

# Update PATH for arm developer toolchain
RUN echo 'export PATH="/opt/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf/bin:$PATH"' >> ~/.bashrc
