#!/bin/bash

# Note, the steps more or less follow legacy readme manual for building demos.
STEP_0_NAME="Setup: set up FW components"
STEP_1_NAME="1: Build OP-TEE OS"
STEP_2_NAME="2: Build Linux file system (buildroot)"
STEP_3_NAME="3: Build OP-TEE Clients"
STEP_4_NAME="4: Build OP-TEE xtest"
STEP_5_NAME="5: Compile Bitcoin wallet UA an TA"
STEP_6_NAME="6: Compile Malicious UA and TA"
STEP_7_NAME="7: Compile Context-Based Authentication TA"
STEP_8_NAME="8: Compile Cache Coloring tests"
STEP_9_NAME="9: Finalize Linux file system"
STEP_10_NAME="10: Build linux"
STEP_11_NAME="11: Bind Linux image and device tree"

ROOT=$(git -C "$(dirname "$(realpath $0)")" rev-parse --show-toplevel)

RUN_ALL=false
LOCAL_CONFS=false
STEP_RANGE=""

BUILDROOT_CONF_PATH="support/br-aarch64.config"
BUILDROOT_LOCAL_CONF_PATH="buildroot/build-aarch64/.config"
LINUX_CONF_PATH="support/linux-aarch64.config"
LINUX_LOCAL_CONF_PATH="linux/build-aarch64/.config"
DTS_FILE="rpi4-ws/rpi4.dts"
LINUX_TARGET="linux-rpi4"
BUILD_CBA="false"

print_usage() {
  echo "Available steps:"
  for i in {0..11}; do
    step_name_var="STEP_${i}_NAME"
    echo "  $i - ${!step_name_var}"
  done

  echo
  echo "Usage:"
  echo "  $0 --all - execute all steps."
  echo "  $0 --local-confs - use local .config files instead of copying reference ones."
  echo "  $0 --steps=X-Y - execute steps from X to Y (inclusive)."
  echo "  [--buildroot_conf=PATH] [--linux_conf=PATH] [--dts=PATH] [--linux-target=NAME] - if not provided, defaults will be used."
  exit 1
}

print_step_header() {
  local step="$1"
  local prefix="Step: "
  local content="$prefix$step"
  local length=${#content}
  local border=$(printf '%*s' $((length + 4)) '' | tr ' ' '#')

  echo "$border"
  echo "# $content #"
  echo "$border"
}

# Copy PKCS#11 TA ".TA" files
extra_step_1() {
    cd "$ROOT"

    TA_FILE_PATH=optee_os/optee-rpi4/export-ta_arm64/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta
    TA_FILE_PATH_2=optee_os/optee2-rpi4/export-ta_arm64/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta
    BUILDROOT_PATH=buildroot/build-aarch64/target/lib/optee_armtz/
    BUILDROOT_PATH_2=buildroot/build-aarch64/target/lib/optee2_armtz/

    mkdir -p "${BUILDROOT_PATH}" "${BUILDROOT_PATH_2}"
    cp $TA_FILE_PATH $BUILDROOT_PATH
    cp $TA_FILE_PATH_2 $BUILDROOT_PATH_2

    cd "$ROOT"
}

# Build memory separation app
extra_step_2() {
    cd "$ROOT/memory-separation"

    make clean
    make CROSS_COMPILE=aarch64-none-elf- CONFIG_TEXT_BASE=0x20200000

    cd "$ROOT"
}

step_0() {
    cd "$ROOT"

    cd rpi4-ws
    export RPI4_WS=$(pwd)

    mkdir -p bin

    git clone https://github.com/raspberrypi/firmware.git --depth 1 --branch 1.20230405 \
        || echo "# Skipping cloning RPI firmware."

    git clone https://github.com/u-boot/u-boot.git --depth 1 --branch v2022.10 \
        || echo "# Skipping cloning u-boot."

    cd u-boot
    make rpi_4_defconfig
    make -j`nproc`  CROSS_COMPILE=aarch64-none-elf-
    cp -v u-boot.bin ../bin/
    cd $RPI4_WS

    git clone https://github.com/bao-project/arm-trusted-firmware.git --branch bao/demo --depth 1 \
        || echo "# Skipping cloning bao hypervisor."

    cd arm-trusted-firmware
    make PLAT=rpi4 -j`nproc`  CROSS_COMPILE=aarch64-none-elf-
    cp -v build/rpi4/release/bl31.bin ../bin/
    cd $RPI4_WS
}

step_1() {
    cd "$ROOT"

    cd optee_os

    OPTEE_DIR="./"
    export O="$OPTEE_DIR/optee-rpi4"
    CC="aarch64-none-elf-"
    export CFLAGS=-Wno-cast-function-type
    PLATFORM="rpi4"
    ARCH="arm"
    SHMEM_START="0x08000000"
    SHMEM_SIZE="0x00200000"
    TZDRAM_START="0x10100000"
    TZDRAM_SIZE="0x00F00000"
    CFG_GIC=n

    rm -rf $O

    make -C $OPTEE_DIR \
        O=$O \
        CROSS_COMPILE=$CC \
        PLATFORM=$PLATFORM \
        PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
        ARCH=$ARCH \
        CFG_PKCS11_TA=y \
        CFG_SHMEM_START=$SHMEM_START \
        CFG_SHMEM_SIZE=$SHMEM_SIZE \
        CFG_CORE_DYN_SHM=n \
        CFG_NUM_THREADS=1 \
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
        CFG_TEE_CORE_LOG_LEVEL=1 \
        DEBUG=1 -j16


    OPTEE_DIR="./"
    export O="$OPTEE_DIR/optee2-rpi4"
    SHMEM_START="0xc8200000"
    TZDRAM_START="0xb0100000"

    rm -rf $O

    make -C $OPTEE_DIR \
        O=$O \
        CROSS_COMPILE=$CC \
        PLATFORM=$PLATFORM \
        PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
        ARCH=$ARCH \
        CFG_PKCS11_TA=y \
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


    cd $ROOT

    [ "$BUILD_CBA" == "false" ] && return

    cd optee_os_cba

    OPTEE_DIR="./"
    export O="$OPTEE_DIR/optee-cba-rpi4"
    CC="aarch64-none-elf-"
    export CFLAGS=-Wno-cast-function-type
    PLATFORM="rpi4"
    ARCH="arm"
    SHMEM_START="0x08000000"
    SHMEM_SIZE="0x00200000"
    TZDRAM_START="0x10100000"
    TZDRAM_SIZE="0x00F00000"
    CFG_GIC=n

    rm -rf $O

    make -C $OPTEE_DIR \
        O=$O \
        CROSS_COMPILE=$CC \
        PLATFORM=$PLATFORM \
        PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
        ARCH=$ARCH \
        CFG_PKCS11_TA=y \
        CFG_SHMEM_START=$SHMEM_START \
        CFG_SHMEM_SIZE=$SHMEM_SIZE \
        CFG_CORE_DYN_SHM=n \
        CFG_NUM_THREADS=1 \
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
        CFG_TEE_CORE_LOG_LEVEL=1 \
        DEBUG=1 -j16

    cd $ROOT
}

step_2() {
    cd "$ROOT"

    mkdir -p buildroot/build-aarch64

    if [ "$LOCAL_CONFS" = "false" ]; then
        cp $BUILDROOT_CONF_PATH $BUILDROOT_LOCAL_CONF_PATH
    fi

    cd buildroot

    # Build an incomplete filesystem to benefit fom buildroot building the
    # appropriate linux toolchain.
    make O=build-aarch64/ -j`nproc` ||  echo "Building buildroot failed as expected!"

    cd $ROOT
}

step_3() {
    cd "$ROOT"

    cd optee_client

    git checkout master
    make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out-aarch64
    git checkout optee2
    make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out2-aarch64

    cd $ROOT
}

step_4() {
    cd "$ROOT"

    cd optee_test

    BUILDROOT=$(pwd)/../buildroot/build-aarch64/
    export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export ARCH=aarch64
    export PLATFORM=plat-vexpress
    export PLATFORM_FLAVOR=qemu_armv8a
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=y
    export DESTDIR=./to_buildroot-aarch64
    export DEBUG=0
    export CFG_TEE_TA_LOG_LEVEL=0
    export CFLAGS=-O2
    export O=$(pwd)/out-aarch64
    export CFG_PKCS11_TA=n

    rm -rf $O
    rm -rf to_buildroot-aarch64/
    find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/\-lckteec2$/\-lckteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
    make clean
    make -j`nproc`
    make install

    export O=$(pwd)/out2-aarch64
    export DESTDIR=./to_buildroot-aarch64-2
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee2-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    rm -rf $(pwd)/out2-aarch64
    find . -name "Makefile" -exec sed -i "s/\-lteec$/\-lteec2/g" {} +
    find . -name "Makefile" -exec sed -i "s/\-lckteec$/\-lckteec2/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee_armtz/optee2_armtz/g" {} +
    make clean
    make -j`nproc`
    make install
    find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +

    mv $DESTDIR/bin/xtest $DESTDIR/bin/xtest2

    [ "$BUILD_CBA" == "false" ] && return

    export O=$(pwd)/out-cba-aarch64
    export DESTDIR=./to_buildroot-aarch64-cba
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os_cba/optee-cba-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    rm -rf $(pwd)/out-cba-aarch64
    find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/\-lckteec2$/\-lckteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
    make clean
    make -j`nproc`
    make install

    cd $ROOT
}

step_5() {
    cd "$ROOT"

    cd bitcoin-wallet

    BUILDROOT=$(pwd)/../buildroot/build-aarch64/

    export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export ARCH=aarch64
    export PLATFORM=plat-virt
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
    export DESTDIR=./to_buildroot-aarch64
    export DEBUG=0
    export CFG_TEE_TA_LOG_LEVEL=0
    export O=$(pwd)/out-aarch64

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
    export O=$(pwd)/out2-aarch64
    export DESTDIR=./to_buildroot-aarch64-2
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee2-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    rm -rf $(pwd)/out2-aarch64
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

    cd $ROOT
}

step_6() {
    cd "$ROOT"

    cd malicous_ta
    BUILDROOT=$(pwd)/../buildroot/build-aarch64/
    export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export ARCH=aarch64
    export PLATFORM=plat-virt
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=n
    export DESTDIR=./to_buildroot-aarch64
    export DEBUG=0
    export CFG_TEE_TA_LOG_LEVEL=2
    export O=$(pwd)/out-aarch64
    export aarch64_TARGET=y
    rm -rf out-aarch64/
    ## make sure we have things setup for first OP-TEE
    find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
    make clean
    make -j`nproc`
    if [ $? -ne 0 ]; then
        echo "Failed to compile malicious TA for first optee!"
        exit 1
    fi
    mkdir -p to_buildroot-aarch64/lib/optee_armtz
    mkdir -p to_buildroot-aarch64/bin
    cp out-aarch64/*.ta to_buildroot-aarch64/lib/optee_armtz
    cp host/malicious_ca to_buildroot-aarch64/bin/malicious_ca
    chmod +x to_buildroot-aarch64/bin/malicious_ca
    ## setup second OP-TEE
    export O=$(pwd)/out2-aarch64
    export DESTDIR=./to_buildroot-aarch64-2
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os/optee2-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out2-aarch64/export/usr/
    rm -rf $(pwd)/out2-aarch64
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
    cd $ROOT
}

# Build CBA UA and TA
# (is similar to building any other TA)
# DIFFERENCE: The `make install` step isn't available,
# so all files must be copied to their respective locations manually!
step_7() {
    [ "$BUILD_CBA" == "false" ] && return

    cd "$ROOT"

    cd 'cba_ta'

    BUILDROOT=$(pwd)/../buildroot/build-aarch64/
    export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export ARCH=aarch64
    export PLATFORM=plat-vexpress
    export PLATFORM_FLAVOR=qemu_armv8a
    export TA_DEV_KIT_DIR=$(pwd)/../optee_os_cba/optee-cba-rpi4/export-ta_arm64
    export TEEC_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export OPTEE_CLIENT_EXPORT=$(pwd)/../optee_client/out-aarch64/export/usr/
    export CFG_TA_OPTEE_CORE_API_COMPAT_1_1=y
    export DESTDIR=./to_buildroot-aarch64-cba
    export DEBUG=0
    export CFG_TEE_TA_LOG_LEVEL=0
    export CFLAGS=-O2
    export O=$(pwd)/out-cba-aarch64
    export CFG_PKCS11_TA=n

    rm -rf $O
    rm -rf to_buildroot-aarch64-cba/
    find . -name "Makefile" -exec sed -i "s/\-lteec2$/\-lteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/\-lckteec2$/\-lckteec/g" {} +
    find . -name "Makefile" -exec sed -i "s/optee2_armtz/optee_armtz/g" {} +
    make clean
    make -j$(nproc)

    mkdir -p to_buildroot-aarch64-cba/lib/optee_armtz
    mkdir -p to_buildroot-aarch64-cba/bin

    cp out-cba-aarch64/*.ta to_buildroot-aarch64-cba/lib/optee_armtz
    cp host/context_based_authentication_demo to_buildroot-aarch64-cba/bin

    cd "$ROOT"
}

step_8() {
    cd "$ROOT/security_test"

    OLD_CFLAGS=$CFLAGS
    BUILDROOT=$ROOT/buildroot/build-aarch64
    export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
    export DESTDIR=./to_buildroot-aarch64
    export O=$PWD/out-aarch64
    export TIME_SOURCE=perf
    export ARCH=armv8
    export DEVICE_CONFIGURATION=rpi4
    export LLC_SIZE=0x100000
    unset CFLAGS

    cp "files/${DEVICE_CONFIGURATION}.h" armageddon/libflush/libflush/eviction/strategies/
    make clean || true
    make -j"$(nproc)"

    export CFLAGS=$OLD_CFLAGS
}

step_9() {
    cd "$ROOT"

    # Call extra step to copy ".TA" files
    extra_step_1

    cd buildroot

    make O=build-aarch64/ -j`nproc`

    cd $ROOT
}

step_10() {
    cd "$ROOT"

    mkdir -p linux/build-aarch64/

    if [ "$LOCAL_CONFS" = "false" ]; then
        cp $LINUX_CONF_PATH $LINUX_LOCAL_CONF_PATH
    fi

    cd linux

    make ARCH=arm64 O=build-aarch64 CROSS_COMPILE=`realpath ../buildroot/build-aarch64/host/bin/aarch64-linux-` -j16 Image dtbs

    cd $ROOT
}

step_11() {
    cd "$ROOT"

    dtc -I dts -O dtb $DTS_FILE > rpi4-ws/rpi4.dtb
    cd lloader

    rm -f "$LINUX_TARGET.bin"
    rm -f "$LINUX_TARGET.elf"
    make  \
        IMAGE=../linux/build-aarch64/arch/arm64/boot/Image \
        DTB=../rpi4-ws/rpi4.dtb \
        TARGET=$LINUX_TARGET.bin \
        CROSS_COMPILE=aarch64-none-elf- \
        ARCH=aarch64

    cd $ROOT

    # Build memory separation app
    extra_step_2

    cd $ROOT
}

set -e

# Handle sysargs
for arg in "$@"; do
  case "$arg" in
    --all)
      RUN_ALL=true
      ;;
    --local-confs)
      LOCAL_CONFS=true
      ;;
    --steps=*)
      STEP_RANGE="${arg#*=}"
      ;;
    --buildroot_conf=*)
      BUILDROOT_CONF_PATH="${arg#*=}"
      ;;
    --linux_conf=*)
      LINUX_CONF_PATH="${arg#*=}"
      ;;
    --dts=*)
      DTS_FILE="${arg#*=}"
      ;;
    --linux-target=*)
      LINUX_TARGET="${arg#*=}"
      ;;
    *)
      echo "Unknown argument: $arg"
      print_usage
      ;;
  esac
done

# Parse params
if [ "$RUN_ALL" = true ]; then
  STEP_START=0
  STEP_END=11
elif [[ "$STEP_RANGE" =~ ^([0-9]+)-([0-9]+)$ ]]; then
  STEP_START=${BASH_REMATCH[1]}
  STEP_END=${BASH_REMATCH[2]}
else
  print_usage
fi

# Check for CBA:
config_path=""
if [ "${LOCAL_CONF:-}" = "true" ]; then
    config_path="$BUILDROOT_LOCAL_CONF_PATH"
else
    config_path="$BUILDROOT_CONF_PATH"
fi

if grep -q "optee_os_cba" "$config_path" || grep -q "cba_ta" "$config_path"; then
    BUILD_CBA="true"
else
    BUILD_CBA="false"
fi

# Run steps
for ((i=STEP_START; i<=STEP_END; i++)); do
  step_name_var="STEP_${i}_NAME"
  print_step_header "${!step_name_var}"

  "step_$i"
done

echo "Done!"
