#!/bin/bash

step_1() {
cd optee_os

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
    CROSS_COMPILE64=riscv64-linux-gnu- \
    CROSS_COMPILE32=riscv32-linux-gnu-\
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
    CROSS_COMPILE64=riscv64-linux-gnu- \
    CROSS_COMPILE32=riscv32-linux-gnu- \
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
}

step_2() {
mkdir -p buildroot/build-riscv64

cp support/br-riscv64.config buildroot/build-riscv64/.config

cd buildroot

make O=build-riscv64/ -j$(nproc) || echo "The above error is expected at this stage!"

cd ..
}

step_3() {
cd optee_client

git checkout master
make CROSS_COMPILE=riscv64-linux-gnu- WITH_TEEACL=0 O=out-riscv64
git checkout optee2
make CROSS_COMPILE=riscv64-linux-gnu- WITH_TEEACL=0 O=out2-riscv64 CFG_TEE_FS_PARENT_PATH=/data/tee2

cd ..
}

step_4() {
cd optee_test

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
}

step_5() {
cd bitcoin-wallet

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
}

step_6() {
cd malicous_ta

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
}

step_7() {
cd buildroot

make O=build-riscv64/ -j`nproc`

cd ..
}

step_8() {
mkdir -p linux/build-riscv64/
cp support/linux-riscv64.config linux/build-riscv64/.config

cd linux

make ARCH=riscv O=build-riscv64 CROSS_COMPILE=`realpath ../buildroot/build-riscv64/host/bin/riscv64-linux-` -j16 Image

cd ..

dtc -I dts -O dtb riscv64-ws/riscv64-dt1.dts > riscv64-ws/riscv64-dt1.dtb
dtc -I dts -O dtb riscv64-ws/riscv64-dt2.dts > riscv64-ws/riscv64-dt2.dtb

cd lloader

rm linux*-riscv64.bin || echo "Proceeding..."
rm linux*-riscv64.elf || echo "Proceeding..."
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
}

usage() {
    cat <<EOF
Usage: $0 [--steps=X-Y] [--help]

Options:
  --steps=X-Y   Run steps from X to Y (inclusive). If not provided, all steps are run.
  --help        Show this help message and exit.

Available steps: $(declare -F | awk '{print $3}' | grep '^step_[0-9]\+' | sort -t_ -k2n | tr '\n' ' ')
EOF
}

start=1
end=0   # 0 means "no upper limit"

set -e

# Parse arguments
for arg in "$@"; do
    case $arg in
        --help)
            usage
            exit 0
            ;;
        --steps=*)
            range="${arg#*=}"
            start="${range%-*}"
            end="${range#*-}"
            ;;
        *)
            echo "Unknown option: $arg"
            usage
            exit 1
            ;;
    esac
done

# Get all available step functions
all_steps=($(declare -F | awk '{print $3}' | grep '^step_[0-9]\+' | sort -t_ -k2n))

# If no --steps provided, run all
if [[ $end -eq 0 ]]; then
    for fn in "${all_steps[@]}"; do
        "$fn"
    done
else
    for fn in "${all_steps[@]}"; do
        num="${fn#step_}"
        if (( num >= start && num <= end )); then
            "$fn"
        fi
    done
fi
