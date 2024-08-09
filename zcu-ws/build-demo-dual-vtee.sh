#!/bin/bash -e

CONFIG_REPO=`pwd`/configs

pushd ..

make -C CROSSCON-Hypervisor/ \
	PLATFORM=zcu104 \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=zcu-dual-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf- \
        clean

make -C CROSSCON-Hypervisor/ \
	PLATFORM=zcu104 \
	CONFIG_BUILTIN=y \
	CONFIG_REPO=$CONFIG_REPO \
	CONFIG=zcu-dual-vTEE \
	OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
	CROSS_COMPILE=aarch64-none-elf- \
        -j`nproc`

popd

cp -uv ../CROSSCON-Hypervisor/bin/zcu104/builtin-configs/zcu-dual-vTEE/crossconhyp.bin bin/


mkimage -n crossconhyp_uboot -A arm64 -O linux -C none -T kernel -a 0x200000 \
    -e 0x200000 -d bin/crossconhyp.bin bin/crossconhyp.img

cp -v bin/BOOT.BIN /media/$USER/boot
cp -v bin/crossconhyp.img /media/$USER/boot
sync
umount /media/$USER/boot




