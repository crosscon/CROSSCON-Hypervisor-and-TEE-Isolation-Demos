#!/bin/bash -e

CONFIG_REPO=`pwd`/configs

pushd ..

make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-aarch64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=qemu-virt-aarch64-dual-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf- \
        clean

make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-aarch64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=qemu-virt-aarch64-dual-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf- \
        -j`nproc`

cp -uv CROSSCON-Hypervisor/bin/qemu-aarch64-virt/builtin-configs/qemu-virt-aarch64-dual-vTEE/crossconhyp.bin ./aarch64-ws/bl33.bin

popd

../qemu/build/qemu-system-aarch64 \
    -nographic \
    -machine virt,secure=on, \
    -machine gic-version=2 \
    -machine virtualization=on \
    -cpu cortex-a53 \
    -smp 4 \
    -m 4G \
    -bios bl1.bin \
    -semihosting-config enable,target=native \
    -serial mon:stdio -serial pty \
    -s \
    $@

cd -
