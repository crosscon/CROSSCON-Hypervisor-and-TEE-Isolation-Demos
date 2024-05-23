#!/bin/bash -e

CONFIG_REPO=`pwd`/configs

pushd ..
make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-riscv64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=qemu-virt-riscv64-dual-vTEE \
	OPTIMIZATIONS=g \
        SDEES="sdSGX sdTZ" \
        CROSS_COMPILE=riscv64-unknown-elf- \
        clean

make -C CROSSCON-Hypervisor/ \
	PLATFORM=qemu-riscv64-virt \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=qemu-virt-riscv64-dual-vTEE \
	OPTIMIZATIONS=g \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=riscv64-unknown-elf- \
        -j`nproc`

rm -rf opensbi/build/
make -C opensbi \
	PLATFORM=generic \
	FW_PAYLOAD=y \
	FW_PAYLOAD_FDT_ADDR=0x80100000 \
	FW_PAYLOAD_PATH=`pwd`/CROSSCON-Hypervisor/bin/qemu-riscv64-virt/builtin-configs/qemu-virt-riscv64-dual-vTEE/crossconhyp.bin \
	CROSS_COMPILE=riscv64-unknown-elf- \
	-j7

cp -v opensbi/build/platform/generic/firmware/fw_payload.elf ./riscv64-ws

popd


../qemu/build/riscv64-softmmu/qemu-system-riscv64 \
    -nographic \
    -cpu rv64,priv_spec=v1.12.0,sstc=false \
    -M virt \
    -m 4G \
    -smp 2 \
    -bios fw_payload.elf \
    -serial mon:stdio -serial pty \
    -s \
    $@

cd -

