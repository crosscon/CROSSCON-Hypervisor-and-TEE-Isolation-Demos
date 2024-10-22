# CROSSCON Hypervisor and TEE Isolation

Here you can find demos for the initial version of the _CROSSCON Hypervisor_ and _CROSSCON TEE Isolation_ demos.
These demos are related to developed as part of the [CROSSCON project](https://crosscon.eu/). 
The repository contains on overview of the CROSSCON per-VM TEE feature working for both Arm and RISC-V qemu platforms. This feature can be used to achieve TEE isolation, by running multiple TEE VMs instead of a single TEE instance in the normal world.

This repository is part of the deliverable D3.2: CROSSCON Open Security Stack - Initial Version.

## Overview

### CROSSCON per VM TEEs
Refer to [CROSSCON Hypervisor](https://github.com/crosscon/CROSSCON-Hypervisor) for details on CROSSCON Hypervisor per-VM TEE support.

For detailed description, please refer to Sections 3.2 on the deliverable [D3.1: CROSSCON Open Security Stack Documentation ‐ Draft.](https://crosscon.eu/library/deliverables).

### CROSSCON TEE Isolation
**CROSSCON TEE Isolation** is developed as part of the [CROSSCON project](https://crosscon.eu/). It leverages the [CROSSCON Hypervisor](https://github.com/crosscon/CROSSCON-Hypervisor) to support multiple TEE VMs per VM. We've updated the TEE CROSSCON Hypervisor internal module, to be able to identify which TEE VM the GPOS is requesting interaction with. Additionally, this required modifications at the GPOS level, to be able to provide TEE driver instances for each TEE separately. This means that when performing SMC calls, the OS will use different IDs depending on the targeted trusted OS.

For detailed description, please refer to Sections 3.1 on the deliverable [D3.1: CROSSCON Open Security Stack Documentation ‐ Draft.](https://crosscon.eu/library/deliverables).


### Prerequisite
We assume that you are following the instructions on a PC with a working setup of Ubuntu or Debian OS.
Before you jump to Demos you need to install each Tool in the following list:

| Tool                    | Version |
| ----------------------- | ------- |
| arm-none-eabi-gcc       | 11.3.1  |
| aarch64-none-elf-gcc    | 11.1.1  |
| riscv64-unknown-elf-gcc | 10.2.0  |
| make                    | 4.2.1   |
| dtc                     | 1.5.0   |
| gcc                     | 9.3.0   |
| mkimage                 | 20.10   |
| cmake                   | 3.20.0  |
| ninja                   | 1.10.1  |


#### How to experiment CROSSCON Hypervisor Per-VM TEE feature?
This experiment will use OP-TEE as trusted OS and will be carry out in qemu.
This guide provides steps for both aarch64 and RISC-V, please follow the
corresponding steps for the chosen architecture.

## Setup
Clone the repository:
``` sh
git clone git@github.com:crosscon/CROSSCON-Hypervisor-and-TEE-Isolation-Demos.git
```

If you're updating this repository, new components might have been added. Use
the following commands to update an existing repository.
``` sh
git pull
git submodule init
git submodule update
```

## Step 1: Build OP-TEE-OS

``` sh
cd optee_os
```
### Build for aarch64
``` sh
OPTEE_DIR="./"
export O="$OPTEE_DIR/optee"
CC="aarch64-none-elf-"
export CFLAGS=-Wno-cast-function-type
PLATFORM="vexpress"
PLATFORM_FLAVOR="qemu_armv8a"
ARCH="arm"
SHMEM_START="0x70000000"
SHMEM_SIZE="0x00200000"
TZDRAM_START="0x10100000"
TZDRAM_SIZE="0x00F00000"
CFG_GIC=n

rm -rf $O

make -C $OPTEE_DIR \
    O="$OPTEE_DIR/optee" \
    CROSS_COMPILE=$CC \
    PLATFORM=$PLATFORM \
    PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
    ARCH=$ARCH \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    CFG_CORE_DYN_SHM=n \
    CFG_CORE_RESERVED_SHM=y \
    CFG_CORE_ASYNC_NOTIF=n \
    CFG_TZDRAM_SIZE=$TZDRAM_SIZE \
    CFG_TZDRAM_START=$TZDRAM_START \
    CFG_GIC=y \
    CFG_ARM_GICV2=y \
    CFG_CORE_IRQ_IS_NATIVE_INTR=n \
    CFG_ARM64_core=y \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_DT=n \
    CFG_CORE_ASLR=n \
    CFG_TEE_CORE_LOG_LEVEL=0 \
    CFG_CORE_WORKAROUND_SPECTRE_BP=n \
    CFG_CORE_WORKAROUND_NSITR_CACHE_PRIME=n \
    CFG_TEE_CORE_LOG_LEVEL=1 \
    DEBUG=1 -j16


OPTEE_DIR="./"
export O="$OPTEE_DIR/optee2"
SHMEM_START="0x70200000"
TZDRAM_START="0x20100000"

rm -rf $O

make -C $OPTEE_DIR \
    O="$OPTEE_DIR/optee2" \
    CROSS_COMPILE=$CC \
    PLATFORM=$PLATFORM \
    PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
    ARCH=$ARCH \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    CFG_CORE_DYN_SHM=n \
    CFG_CORE_RESERVED_SHM=y \
    CFG_CORE_ASYNC_NOTIF=n \
    CFG_TZDRAM_SIZE=$TZDRAM_SIZE \
    CFG_TZDRAM_START=$TZDRAM_START \
    CFG_GIC=y \
    CFG_ARM_GICV2=y \
    CFG_CORE_IRQ_IS_NATIVE_INTR=n \
    CFG_ARM64_core=y \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_DT=n \
    CFG_CORE_ASLR=n \
    CFG_CORE_WORKAROUND_SPECTRE_BP=n \
    CFG_CORE_WORKAROUND_NSITR_CACHE_PRIME=n \
    CFLAGS="${CFLAGS} -DOPTEE2" \
    CFG_EARLY_TA=y \
    CFG_TEE_CORE_LOG_LEVEL=1 \
    DEBUG=1 -j16

cd ..
```

### Build for RISCV
``` sh
OPTEE_DIR="./"
export O="$OPTEE_DIR/optee-riscv"

SHMEM_START="0x98f00000"
SHMEM_SIZE="0x00200000"
TDDRAM_START="0xb0000000"
TDDRAM_SIZE="0x00f00000"

rm -rf $O

make \
    ARCH=riscv \
    PLATFORM=virt \
    CROSS_COMPILE64=riscv64-unknown-linux-gnu- \
    CROSS_COMPILE32=riscv32-unknown-linux-gnu- \
    CFG_TDDRAM_SIZE=$TDDRAM_SIZE \
    CFG_TDDRAM_START=$TDDRAM_START \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    DEBUG=1 \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_TEE_TA_LOG_LEVEL=2 \
    CFLAGS="-Og -DTARGET_RISCV" \
    -j16

export O="$OPTEE_DIR/optee2-riscv"
TDDRAM_START="0xb2000000"
SHMEM_START="0x99100000"

rm -rf $O

make \
    ARCH=riscv \
    PLATFORM=virt \
    CROSS_COMPILE64=riscv64-unknown-linux-gnu- \
    CROSS_COMPILE32=riscv32-unknown-linux-gnu- \
    CFG_TDDRAM_SIZE=$TDDRAM_SIZE \
    CFG_TDDRAM_START=$TDDRAM_START \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    DEBUG=1 \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_TEE_TA_LOG_LEVEL=2 \
    CFLAGS="-Og -DOPTEE2 -DTARGET_RISCV" \
    -j16

cd ..
```

## Step 2: Linux file system
We will first build an incomplete filesystem to benefit fom buildroot building
the appropriate linux toolchains.
We've tested with Buildroot 2022.11.1 (https://buildroot.org/downloads/buildroot-2022.11.1.tar.gz)
``` sh
wget https://buildroot.org/downloads/buildroot-2022.11.1.tar.gz
tar -xf buildroot-2022.11.1.tar.gz
mv buildroot-2022.11.1 buildroot
```

Create build directories for aarch64 and riscv64:
``` sh
mkdir buildroot/build-aarch64
mkdir buildroot/build-riscv64
```

Set our predefined `.config` files:
``` sh
cp support/br-aarch64.config buildroot/build-aarch64/.config
cp support/br-riscv64.config buildroot/build-riscv64/.config
```

This build step will fail as we haven't yet setup all the binaries necessary
for the full filesystem. However, we take advantage of the fact that buildroot
builds the linux toolchain to build these missing binaries.
Build:
``` sh
cd buildroot

make O=build-aarch64/ -j`nproc`
OR
make O=build-riscv64/ -j`nproc`

cd ..
```

## Step 3: Build OP-TEE Clients
Build the optee client application library and tee supplicant for both OP-TEEs.
``` sh
cd optee_client
```
### Build for AARCH64
``` sh
git checkout master
make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out-aarch64
git checkout optee2
make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out2-aarch64 CFG_TEE_FS_PARENT_PATH=/data/tee2

cd ..
```
### Build for RISCV
``` sh
git checkout master
make CROSS_COMPILE=riscv64-unknown-linux-gnu- WITH_TEEACL=0 O=out-riscv64
git checkout optee2
make CROSS_COMPILE=riscv64-unknown-linux-gnu- WITH_TEEACL=0 O=out2-riscv64 CFG_TEE_FS_PARENT_PATH=/data/tee2

cd ..
```

## Step 4: Build OP-TEE xtest
Build the OP-TEE xtest test suite.
``` sh
cd optee_test
```

### Build for AARCH64

``` sh
BUILDROOT=`pwd`/../buildroot/build-aarch64/
export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export ARCH=aarch64
export PLATFORM=plat-vexpress
export PLATFORM_FLAVOR=qemu_armv8a
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=y
export DESTDIR=./to_buildroot-aarch64
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=0
export CFLAGS=-O2
export O=`pwd`/out-aarch64
export CFG_PKCS11_TA=n

rm -rf $O
rm -rf to_buildroot-aarch64/
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`
make install


export O=`pwd`/out2-aarch64
export DESTDIR=./to_buildroot-aarch64-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
rm -rf `pwd`/out2-aarch64
find . -name "Makefile" -exec sed -i "s/\-lteec$/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
make install
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mv $DESTDIR/bin/xtest $DESTDIR/bin/xtest2
cd ..
```

### Build for RISCV
``` sh
BUILDROOT=`pwd`/../buildroot/build-riscv64/
export CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export ARCH=riscv
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=y
export DESTDIR=./to_buildroot-riscv
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=0
export O=`pwd`/out-riscv
export RISCV_TARGET=y 


rm -rf out-riscv/
## make sure we have things setup for first OP-TEE
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`
make install


## setup second OP-TEE
export O=`pwd`/out2-riscv64
export DESTDIR=./to_buildroot-riscv-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
rm -rf `pwd`/out2-riscv64
find . -name "Makefile" -exec sed -i "s/\-lteec$/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
make install
## undo changes
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mv $DESTDIR/bin/xtest $DESTDIR/bin/xtest2
cd ..
```

## Step 6: Compile Bitcoin Wallet Client and Trusted Application
```
cd bitcoin-wallet
```

### Arm
```
BUILDROOT=`pwd`/../buildroot/build-aarch64/

export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export ARCH=aarch64
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
export DESTDIR=./to_buildroot-aarch64
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=0
export O=`pwd`/out-aarch64


rm -rf out-aarch64/
## make sure we have things setup for first OP-TEE
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`

mkdir -p to_buildroot-aarch64/lib/optee_armtz
mkdir -p to_buildroot-aarch64/bin

cp out-aarch64/*.ta to_buildroot-aarch64/lib/optee_armtz
cp host/wallet to_buildroot-aarch64/bin/bitcoin_wallet_ca
chmod +x to_buildroot-aarch64/bin/bitcoin_wallet_ca



## setup second OP-TEE
export O=`pwd`/out2-aarch64
export DESTDIR=./to_buildroot-aarch64-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
rm -rf `pwd`/out2-aarch64
find . -name "Makefile" -exec sed -i "s/\-lteec/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
## undo changes
find . -name "Makefile" -exec sed -i "s/\-lteec2/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mkdir -p to_buildroot-aarch64-2/lib/optee2_armtz
mkdir -p to_buildroot-aarch64-2/bin

cp out-aarch64/*.ta to_buildroot-aarch64-2/lib/optee2_armtz
cp host/wallet to_buildroot-aarch64-2/bin/bitcoin_wallet_ca2
chmod +x to_buildroot-aarch64-2/bin/bitcoin_wallet_ca2


cd ..
```

### RISC-V
```

BUILDROOT=`pwd`/../buildroot/build-riscv64/

export CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export ARCH=riscv
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
export DESTDIR=./to_buildroot-riscv
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=0
export O=`pwd`/out-riscv
export RISCV_TARGET=y 


rm -rf out-riscv/
## make sure we have things setup for first OP-TEE
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`

mkdir -p to_buildroot-riscv64/lib/optee_armtz
mkdir -p to_buildroot-riscv64/bin

cp out-riscv/*.ta to_buildroot-riscv64/lib/optee_armtz
cp host/wallet to_buildroot-riscv64/bin/bitcoin_wallet_ca
chmod +x to_buildroot-riscv64/bin/bitcoin_wallet_ca


## setup second OP-TEE
export O=`pwd`/out2-riscv64
export DESTDIR=./to_buildroot-riscv-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
rm -rf `pwd`/out2-riscv64
find . -name "Makefile" -exec sed -i "s/\-lteec/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
## undo changes
find . -name "Makefile" -exec sed -i "s/\-lteec2/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mkdir -p to_buildroot-riscv64-2/lib/optee2_armtz
mkdir -p to_buildroot-riscv64-2/bin

cp out-riscv/*.ta to_buildroot-riscv64-2/lib/optee2_armtz
cp host/wallet to_buildroot-riscv64-2/bin/bitcoin_wallet_ca2
chmod +x to_buildroot-riscv64-2/bin/bitcoin_wallet_ca2

cd ..
```


## Step 7: Compile Malicious Client and Trusted Application
```
cd malicous_ta
```

### Arm
```
BUILDROOT=`pwd`/../buildroot/build-aarch64/

export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export ARCH=aarch64
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-aarch64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
export DESTDIR=./to_buildroot-aarch64
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=2
export O=`pwd`/out-aarch64
export aarch64_TARGET=y 


rm -rf out-aarch64/
## make sure we have things setup for first OP-TEE
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`

mkdir -p to_buildroot-aarch64/lib/optee_armtz
mkdir -p to_buildroot-aarch64/bin

cp out-aarch64/*.ta to_buildroot-aarch64/lib/optee_armtz
cp host/malicious_ca to_buildroot-aarch64/bin/malicious_ca
chmod +x to_buildroot-aarch64/bin/malicious_ca


## setup second OP-TEE
export O=`pwd`/out2-aarch64
export DESTDIR=./to_buildroot-aarch64-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2/export-ta_arm64
export TEEC_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-aarch64/export/usr/
rm -rf `pwd`/out2-aarch64
find . -name "Makefile" -exec sed -i "s/\-lteec/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
## undo changes
find . -name "Makefile" -exec sed -i "s/\-lteec2/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mkdir -p to_buildroot-aarch64-2/lib/optee2_armtz
mkdir -p to_buildroot-aarch64-2/bin

cp out2-aarch64/*.ta to_buildroot-aarch64-2/lib/optee2_armtz
cp host/malicious_ca to_buildroot-aarch64-2/bin/malicious_ca2
chmod +x to_buildroot-aarch64-2/bin/malicious_ca2


cd..
```

### RISC-V
```
BUILDROOT=`pwd`/../buildroot/build-riscv64/

export CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/riscv64-linux-
export ARCH=riscv
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out-riscv64/export/usr/
export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
export DESTDIR=./to_buildroot-riscv64
export DEBUG=0
export CFG_TEE_TA_LOG_LEVEL=2
export O=`pwd`/out-riscv64
export RISCV_TARGET=y 


rm -rf out-riscv64/
## make sure we have things setup for first OP-TEE
find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
make clean
make -j`nproc`

mkdir -p to_buildroot-riscv64/lib/optee_armtz
mkdir -p to_buildroot-riscv64/bin

cp out-riscv64/*.ta to_buildroot-riscv64/lib/optee_armtz
cp host/malicious_ca to_buildroot-riscv64/bin/malicious_ca
chmod +x to_buildroot-riscv64/bin/malicious_ca


## setup for second OP-TEE
export O=`pwd`/out2-riscv64
export DESTDIR=./to_buildroot-riscv-2
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2-riscv/export-ta_rv64
export TEEC_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
export OPTEE_CLIENT_EXPORT=`pwd`/../optee_client/out2-riscv64/export/usr/
rm -rf `pwd`/out2-riscv64
find . -name "Makefile" -exec sed -i "s/\-lteec/\-lteec2/g" {} +
find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
make clean
make -j`nproc`
## undo changes
find . -name "Makefile" -exec sed -i "s/\-lteec2/\-lteec/g" {} +
find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

mkdir -p to_buildroot-riscv64-2/lib/optee2_armtz
mkdir -p to_buildroot-riscv64-2/bin

cp out2-riscv64/*.ta to_buildroot-riscv64-2/lib/optee2_armtz
cp host/malicious_ca to_buildroot-riscv64-2/bin/malicious_ca2
chmod +x to_buildroot-riscv64-2/bin/malicious_ca2

cd ..
```


## Step 8: Finalize Linux file system
We have everything setup now, so build the final file system for Linux.
```sh
cd buildroot

make O=build-aarch64/ -j`nproc`
OR
make O=build-riscv64/ -j`nproc`

cd ..
```

## Step 9: Build Linux

Set our predefined `.config` files:
``` sh
mkdir linux/build-aarch64/
cp support/linux-aarch64.config linux/build-aarch64/.config

mkdir linux/build-riscv64/
cp support/linux-riscv64.config linux/build-riscv64/.config

cd linux
```

Build:
``` sh
make ARCH=arm64 O=build-aarch64 CROSS_COMPILE=`realpath ../buildroot/build-aarch64/host/bin/aarch64-linux-` -j16 Image
OR
make ARCH=riscv O=build-riscv64 CROSS_COMPILE=`realpath ../buildroot/build-riscv64/host/bin/riscv64-linux-` -j16 Image

cd ..
```

### Bind Linux Image and device tree
Although we reuse the Linux kernel image we need to modify the device tree for a second linux guest:

```sh
dtc -I dts -O dtb aarch64-ws/aarch64-dt1.dts > aarch64-ws/aarch64-dt1.dtb
dtc -I dts -O dtb aarch64-ws/aarch64-dt2.dts > aarch64-ws/aarch64-dt2.dtb
OR
dtc -I dts -O dtb riscv64-ws/riscv64-dt1.dts > riscv64-ws/riscv64-dt1.dtb
dtc -I dts -O dtb riscv64-ws/riscv64-dt2.dts > riscv64-ws/riscv64-dt2.dtb
```

#### Arm
```sh
cd lloader

rm linux*-aarch64.bin
rm linux*-aarch64.elf
make  \
    IMAGE=../linux/build-aarch64/arch/arm64/boot/Image \
    DTB=../aarch64-ws/aarch64-dt1.dtb \
    TARGET=linux-aarch64.bin \
    CROSS_COMPILE=aarch64-none-elf- \
    ARCH=aarch64

make \
    IMAGE=../linux/build-aarch64/arch/arm64/boot/Image \
    DTB=../aarch64-ws/aarch64-dt2.dtb \
    TARGET=linux2-aarch64.bin \
    CROSS_COMPILE=aarch64-none-elf- \
    ARCH=aarch64

cd ..
```

#### RISC-V
```sh
cd lloader

rm linux*-riscv64.bin
rm linux*-riscv64.elf
make \
    IMAGE=../linux/build-riscv64/arch/riscv/boot/Image \
    DTB=../riscv64-ws/riscv64-dt1.dtb \
    TARGET=linux-riscv64.bin \
    CROSS_COMPILE=riscv64-unknown-elf- \
    ARCH=riscv

make \
    IMAGE=../linux/build-riscv64/arch/riscv/boot/Image \
    DTB=../riscv64-ws/riscv64-dt2.dtb \
    TARGET=linux2-riscv64.bin \
    CROSS_COMPILE=riscv64-unknown-elf- \
    ARCH=riscv
	
cd ..
```

## Step 10: QEMU Setup
We need to add a second physical UART so we need to build our own qemu (we've tested v7.2.0):
``` sh
git clone https://github.com/qemu/qemu
cd qemu
git checkout v7.2.0
./configure --target-list="riscv64-softmmu aarch64-softmmu"
git apply ../support/0001-hw-riscv-virt-Add-second-uart.patch
git apply ../support/0001-hw-arm-virt-Add-second-uart.patch
make -j`nproc`

cd ..
```

## Step 11: Run the Demos

Change to the aarch64-ws or riscv64-ws:
```
cd aarch64-ws
OR
cd riscv64-ws
```

### Simple Demo
This demo instantiates a Linux VM and an OP-TEE VM
``` sh
./run-demo-vtee.sh
```

After Linux finishes booting run xtest as follows:
``` sh
xtest -t regression
```

### Demo 1
This demo instantiates two Linux VMs each with an OP-TEE VM, run:
``` sh
./run-demo-per-vm-vtee.sh
```

Our Qemu features two UARTs. Qemu on start up will inform us of the location of
the second UART (e.g., /dev/pts/tty3):
```
char device redirected to /dev/pts/3 (label serial1)

OpenSBI v1.1
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
 | |__| | |_) |  __/ | | |____) | |_) || |_
  \____/| .__/ \___|_| |_|_____/|____/_____|
        | |
        |_|

Platform Name             : riscv-virtio,qemu
Platform Features         : medeleg
Platform HART Count       : 2
Platform IPI Device       : aclint-mswi
Platform Timer Device     : aclint-mtimer @ 10000000Hz

```

To connect to the second uart you may use screen on another terminal instance
. For example:
```
screen /dev/pts/3
```

Again on each Linux, you may run xtest as follows:
``` sh
xtest -t regression
```

### Demo 2
This demo instantiates a Linux VM and two OP-TEE VMs.
``` sh
./run-demo-dual-vtee.sh
```

After Linux finishes booting you may execute xtest in both OP-TEE VMs.
``` sh
xtest -t regression
xtest2 -t regression
```

### Demo 3
This demo showcases a Linux VM and two OP-TEE VMs, where the OP-TEEs are
vulnerable, and might try to compromise the Linux VM or the other OP-TEE
instance.

#### Build Vulnerable OP-TEE

``` sh
cd ../optee_os
git fetch --all
git checkout vulnerable
```
##### Build for aarch64
``` sh
OPTEE_DIR="./"
export O="$OPTEE_DIR/optee"
CC="aarch64-none-elf-"
export CFLAGS=-Wno-cast-function-type
PLATFORM="vexpress"
PLATFORM_FLAVOR="qemu_armv8a"
ARCH="arm"
SHMEM_START="0x70000000"
SHMEM_SIZE="0x00200000"
TZDRAM_START="0x10100000"
TZDRAM_SIZE="0x00F00000"
CFG_GIC=n

rm -rf $O

make -C $OPTEE_DIR \
    O="$OPTEE_DIR/optee" \
    CROSS_COMPILE=$CC \
    PLATFORM=$PLATFORM \
    PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
    ARCH=$ARCH \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    CFG_CORE_DYN_SHM=n \
    CFG_CORE_RESERVED_SHM=y \
    CFG_CORE_ASYNC_NOTIF=n \
    CFG_TZDRAM_SIZE=$TZDRAM_SIZE \
    CFG_TZDRAM_START=$TZDRAM_START \
    CFG_GIC=y \
    CFG_ARM_GICV2=y \
    CFG_CORE_IRQ_IS_NATIVE_INTR=n \
    CFG_ARM64_core=y \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_DT=n \
    CFG_CORE_ASLR=n \
    CFG_TEE_CORE_LOG_LEVEL=0 \
    CFG_CORE_WORKAROUND_SPECTRE_BP=n \
    CFG_CORE_WORKAROUND_NSITR_CACHE_PRIME=n \
    CFG_VULN_PTA=y \
    CFG_TEE_CORE_LOG_LEVEL=1 \
    DEBUG=1 -j16


OPTEE_DIR="./"
export O="$OPTEE_DIR/optee2"
SHMEM_START="0x70200000"
TZDRAM_START="0x20100000"

rm -rf $O

make -C $OPTEE_DIR \
    O="$OPTEE_DIR/optee2" \
    CROSS_COMPILE=$CC \
    PLATFORM=$PLATFORM \
    PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
    ARCH=$ARCH \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    CFG_CORE_DYN_SHM=n \
    CFG_CORE_RESERVED_SHM=y \
    CFG_CORE_ASYNC_NOTIF=n \
    CFG_TZDRAM_SIZE=$TZDRAM_SIZE \
    CFG_TZDRAM_START=$TZDRAM_START \
    CFG_GIC=y \
    CFG_ARM_GICV2=y \
    CFG_CORE_IRQ_IS_NATIVE_INTR=n \
    CFG_ARM64_core=y \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_DT=n \
    CFG_CORE_ASLR=n \
    CFG_CORE_WORKAROUND_SPECTRE_BP=n \
    CFG_CORE_WORKAROUND_NSITR_CACHE_PRIME=n \
    CFLAGS="${CFLAGS} -DOPTEE2" \
    CFG_EARLY_TA=y \
    CFG_VULN_PTA=y \
    CFG_TEE_CORE_LOG_LEVEL=1 \
    DEBUG=1 -j16

cd -
```

##### Build for RISCV
``` sh
OPTEE_DIR="./"
export O="$OPTEE_DIR/optee-riscv"

SHMEM_START="0x98f00000"
SHMEM_SIZE="0x00200000"
TDDRAM_START="0xb0000000"
TDDRAM_SIZE="0x00f00000"

rm -rf $O

make \
    ARCH=riscv \
    PLATFORM=virt \
    CROSS_COMPILE64=riscv64-unknown-linux-gnu- \
    CROSS_COMPILE32=riscv32-unknown-linux-gnu- \
    CFG_TDDRAM_SIZE=$TDDRAM_SIZE \
    CFG_TDDRAM_START=$TDDRAM_START \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    DEBUG=1 \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_TEE_TA_LOG_LEVEL=2 \
    CFG_VULN_PTA=y \
    CFLAGS="-Og -DTARGET_RISCV" \
    -j16

export O="$OPTEE_DIR/optee2-riscv"
TDDRAM_START="0xb2000000"
SHMEM_START="0x99100000"

rm -rf $O

make \
    ARCH=riscv \
    PLATFORM=virt \
    CROSS_COMPILE64=riscv64-unknown-linux-gnu- \
    CROSS_COMPILE32=riscv32-unknown-linux-gnu- \
    CFG_TDDRAM_SIZE=$TDDRAM_SIZE \
    CFG_TDDRAM_START=$TDDRAM_START \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    DEBUG=1 \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_TEE_TA_LOG_LEVEL=2 \
    CFG_VULN_PTA=y \
    CFLAGS="-Og -DOPTEE2 -DTARGET_RISCV" \
    -j16

cd -
```

#### Execute the demo
We reutilize the setup from Demo 2, so run:

``` sh
./run-demo-dual-vtee.sh
```

Login with root, and run the following commands to validate that the OP-TEE
instances can try to attack linux, and the other OP-TEE instance but fail to do
so.

Validate that both OP-TEE instances execute, by creating a new bitcoin wallet master key.
``` sh
bitcoin_wallet_ca 2 1234
bitcoin_wallet_ca2 2 1234
```

Use the malicious TA to compromise OP-TEE and attack Linux or the other TEE.
``` sh
malicious_ca 1

OR

malicious_ca 3
```

Validate that the first OP-TEE instance is not accessible after trying to
access memory outside it's allowed range:
``` sh
bitcoin_wallet_ca 2 1234
```

Validate that the second OP-TEE instance remains operable:
``` sh
bitcoin_wallet_ca2 2 1234
```

### Demo 4
This demo showcases a Linux VM instantiating an SGX-like enclave on aarch64.

``` sh
./run-demo-enclave.sh
```

After logging in as root execute the following command to execute sgx-nbench
(https://github.com/utds3lab/sgx-nbench).
``` sh
enclave_app
```

### Demo 5
This demo showcases support for multiple programming models simultaneously,
with each TEE being configured with specific access to memory and IO.
Similarly to Demo 1, we leverage the second Qemu UART, which is attributed to a
second Linux and corresponding OP-TEE instances.

``` sh
./run-demo-models.sh
```

After logging in as root execute the following command to execute sgx-nbench
(https://github.com/utds3lab/sgx-nbench).
``` sh
enclave_app
```

You can also access TEE functionality, on either the first or second Linux
instance. For example:

``` sh
bitcoin_wallet_ca 2 1234
```

In the first instance Linux has a second OP-TEE VM. You can access it by running, for example:

``` sh
bitcoin_wallet_ca2 2 1234
```

### Demo 6
This demo instantiates a Linux VM a FreeRTOS VM, and Baremetal environment VM,
and establishes IPC communication between the Linux and FreeRTOS VMs.
Currently this demo only works for qemu-arch64-virt.

``` sh
./run-demo-multi.sh
```

Our Qemu features two UARTs. Qemu on start up will inform us of the location of
the second UART (e.g., /dev/pts/tty3):
While the CROSSCON Hypervisor outputs to the first UART, the Baremetal and
FreeRTOS output to the second UART.

After Linux finishes booting, you can communicate with FreeRTOS as follows:
``` sh
echo "hello" > /dev/crossconhypipc0
```

The second UART should output the following, among the periodic baremetal and
FreeRTOS output:
```
[FreeRTOS] message from linux: hello
```


## License

See LICENSE file.

## Acknowledgments

The work presented in this repository is part of the [CROSSCON project](https://crosscon.eu/) that received funding from the European Union’s Horizon Europe research and innovation programme under grant agreement No 101070537.

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/themes/crosscon/images/eu.svg" width=10% height=10%>
</p>

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/files/public/styles/large_1080_/public/content-images/media/2023/crosscon_logo.png?itok=LUH3ejzO" width=25% height=25%>
</p>


