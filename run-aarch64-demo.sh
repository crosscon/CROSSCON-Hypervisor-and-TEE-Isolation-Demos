#!/bin/bash -e
make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-aarch64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=`pwd`/CROSSCONHyp-configs \
	CONFIG=qemu-virt-aarch64-single-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf- \
        clean

make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-aarch64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=`pwd`/CROSSCONHyp-configs \
	CONFIG=qemu-virt-aarch64-single-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf-

cp -uv CROSSCON-Hypervisor/bin/qemu-aarch64-virt/builtin-configs/qemu-virt-aarch64-single-vTEE/crossconhyp.bin ./prebuilt/qemu-virt-aarch64/bl33.bin

cd ./prebuilt/qemu-virt-aarch64/

qemu-system-aarch64 \
    -nographic \
    -machine virt,secure=on, \
    -machine gic-version=2 \
    -machine virtualization=on \
    -cpu cortex-a53 \
    -smp 4 \
    -m 4G \
    -bios bl1.bin \
    -semihosting-config enable,target=native \
    -s \
    $@

cd -
