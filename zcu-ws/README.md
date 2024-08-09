# Xilinx ZCU10X

This guide provides steps to leverage the CROSSCON Hypervisor on the ZCU to
build virtual TEEs.
Start by setting the root folder path:

```sh
export ROOT=`pwd`
```


# Get firmware

Download latest pre-built Zynq UltraScale+ MPSoC Firmware and use *bootgen*
to build the firmware binary:

```sh
cd zcu-ws
```

```
git clone https://github.com/Xilinx/soc-prebuilt-firmware.git --depth 1 \
    --branch xilinx_v2023.1 firmware

mkdir bin

pushd firmware/zcu104-zynqmp && 
    bootgen -arch zynqmp -image bootgen.bif -w -o ./../../bin/BOOT.BIN
popd

```


## Prepare SDCard

In this example the sdcard is `/dev/mmcblk0` and the partitions are `/dev/mmcblk0p1`, `/dev/mmcblk0p2`, etc. 

### i) Make sure all partitions are unmounted

```sh
umount /dev/mmcblk0*
```

### ii) Delete all partitions

```sh
sudo fdisk /dev/mmcblk0
```

Then run the commands:

* Press `d` until there are no more partitions (if it asks you for the partition, press `return` for the default)
* Press `w` write changes and exit

### iii) Create partition

```sh
sudo fdisk /dev/mmcblk0
```

Then run the commands:

* `o` to create a new empty DOS partition table
* `n` to create a new partition. Select the following options:
    *  `p` to make it a primary partition
    *  the automatically assigned partition number by pressing `return`
    *  `16384` (this gap is needed for some of the selected boards)
    *  the max default size by pressing `return`
    *  if it asks you to remove the file system signature press `y`
* `a` to make the partition bootable
* `t` to set the partition type:
    * type `c` for W95 FAT32 (LBA)
* `w` to write changes and exit


### iv) Format partition

Format the created partition to a fat filesystem:

```sh
sudo mkfs.fat /dev/mmcblk0p1 -n boot
```

Remove and insert the sd card to automatically mount it. 

---

## Step 1: OP-TEE OS
```sh
cd optee_os

OPTEE_DIR="./"
PLATFORM="zynqmp"
PLATFORM_FLAVOR="zcu104"
CC="aarch64-none-elf-"

export CFLAGS=-Wno-cast-function-type
ARCH="arm"
TZDRAM_SIZE="0x00F00000"
SHMEM_SIZE="0x00200000"
CFG_GIC=n

export O="$OPTEE_DIR/optee-aarch64"
TZDRAM_START="0x60000000"
SHMEM_START="0x61000000"

rm -rf $O

make -C $OPTEE_DIR \
    O=$O \
    CROSS_COMPILE=$CC \
    PLATFORM=$PLATFORM \
    PLATFORM_FLAVOR=$PLATFORM_FLAVOR \
    ARCH=$ARCH \
    CFG_PKCS11_TA=n \
    CFG_SHMEM_START=$SHMEM_START \
    CFG_SHMEM_SIZE=$SHMEM_SIZE \
    CFG_CORE_DYN_SHM=n \
    CFG_NUM_THREADS=1 \
    CFG_CORE_RESERVED_SHM=y \
    CFG_CORE_ASYNC_NOTIF=n \
    CFG_TZDRAM_SIZE=$TZDRAM_SIZE \
    CFG_TZDRAM_START=$TZDRAM_START \
    CFG_GIC=$CFG_GIC \
    CFG_ARM_GICV2=y \
    CFG_CORE_IRQ_IS_NATIVE_INTR=n \
    CFG_ARM64_core=y \
    CFG_USER_TA_TARGETS=ta_arm64 \
    CFG_DT=n \
    CFG_CORE_ASLR=n \
    CFG_CORE_WORKAROUND_SPECTRE_BP=n \
    CFG_CORE_WORKAROUND_NSITR_CACHE_PRIME=n \
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_VULN_PTA=y \
    DEBUG=1 -j16


OPTEE_DIR="./"
export O="$OPTEE_DIR/optee2-aarch64"
TZDRAM_START="0x62000000"
SHMEM_START="0x63000000"


rm -rf $O

make -C $OPTEE_DIR \
    O=$O \
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
    CFG_GIC=$CFG_GIC \
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
    CFG_TEE_CORE_LOG_LEVEL=2 \
    CFG_VULN_PTA=y \
    DEBUG=1 -j16

cd $ROOT
```

## Step 2: Linux file system
We will first build an incomplete filesystem to benefit fom buildroot building the appropriate linux toolchains.
We've tested with Buildroot 2022.11.1 (https://buildroot.org/downloads/buildroot-2022.11.1.tar.gz)
``` sh
if [ ! -e buildroot ]; then
	wget https://buildroot.org/downloads/buildroot-2022.11.1.tar.gz
	tar -xf buildroot-2022.11.1.tar.gz
	mv buildroot-2022.11.1 buildroot
fi
```

Create build directory:
``` sh
mkdir buildroot/build-aarch64
```

Set our predefined `.config` files:
``` sh
cp support/br-aarch64.config buildroot/build-aarch64/.config
```

This build step will fail as we haven't yet setup all the binaries necessary for the full filesystem. However, we take advantage of the fact that buildroot builds the linux toolchain to build these missing binaries. Build:
``` sh
cd buildroot

make O=build-aarch64/ -j`nproc`

cd $ROOT
```

## Step 3: Build OP-TEE Clients
Build the optee client application library and tee supplicant for both OP-TEEs.
``` sh
cd optee_client

git checkout master
make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out-aarch64
git checkout optee2
make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out2-aarch64

cd $ROOT
```

## Step 4: Build OP-TEE xtest
Build the OP-TEE xtest test suite.
``` sh
cd optee_test

BUILDROOT=`pwd`/../buildroot/build-aarch64/
export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export ARCH=aarch64
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee-aarch64/export-ta_arm64
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
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2-aarch64/export-ta_arm64
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
cd $ROOT
```

## Step 5: Compile Bitcoin Wallet Client and Trusted Application
```sh
cd bitcoin-wallet

BUILDROOT=`pwd`/../buildroot/build-aarch64/

export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export HOST_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export TA_CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export ARCH=aarch64
export PLATFORM=plat-virt
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee-aarch64/export-ta_arm64
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
export TA_DEV_KIT_DIR=`pwd`/../optee_os/optee2-aarch64/export-ta_arm64
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


cd $ROOT
```

## Step 6: Finalize Linux file system
We have everything setup now, so build the final file system for Linux.
```sh
cd buildroot

make O=build-aarch64/ -j`nproc`

cd $ROOT
```

## Step 7: Build Linux

Set our predefined `.config` files:
``` sh
mkdir linux/build-aarch64/
cp support/linux-aarch64.config linux/build-aarch64/.config

cd linux

make ARCH=arm64 O=build-aarch64 CROSS_COMPILE=`realpath ../buildroot/build-aarch64/host/bin/aarch64-linux-` -j16 Image dtbs

cd $ROOT
```

### Bind Linux Image and device tree

```sh
dtc -I dts -O dtb zcu-ws/zcu.dts > zcu-ws/zcu.dtb
```

```sh
cd lloader

rm linux-zcu.bin
rm linux-zcu.elf
make  \
    IMAGE=../linux/build-aarch64/arch/arm64/boot/Image \
    DTB=../zcu-ws/zcu.dtb \
    TARGET=linux-zcu.bin \
    CROSS_COMPILE=aarch64-none-elf- \
    ARCH=aarch64

cd $ROOT
```

### Simple Demo'
This demo instantiates a Linux VM and an OP-TEE VM.
Insert the sdcard in your pc, and wait for it to mount the boot partition. The script expects it to be mount on `/media/$USER/boot`
``` sh
./build-demo-vtee.sh
```

---

# Setup SD card

Copy the firmware and crossconhyp's final image to the sdcard:

```
cp bin/BOOT.BIN /media/$USER/boot
cp bin/crossconhyp.img /media/$USER/boot
umount /media/$USER/boot
```

# Setup board

First make sure you have the board configured to boot from the SD card. If you 
are not sure how, check the MPSoC Device Configuration section in the board's 
User Guide:

- [ZCU102](https://www.xilinx.com/support/documentation/boards_and_kits/zcu102/ug1182-zcu102-eval-bd.pdf) 
- [ZCU104](https://www.xilinx.com/support/documentation/boards_and_kits/zcu104/ug1267-zcu104-eval-bd.pdf)

Insert the sd card in the board's sd slot and connect to the board via the 
available micro-USB JTAG/UART port.

Connect to both UARTs available on the zcu (baud 115200). In our setup:

* /dev/ttyUSB1 and /dev/ttyUSB2 for ZCU104
* /dev/ttyUSB0 and /dev/ttyUSB1 for ZCU102

Open a new terminal and connect to it. For example:

```
miniterm.py /dev/ttyUSB1 115200
```

```
miniterm.py /dev/ttyUSB2 115200
```

Turn on/reset your board.

# Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until 
you get the u-boot prompt. Then load the crossconhyp image, and jump to it:

```
fatload mmc 0 0x200000 crossconhyp.img; bootm start 0x200000; bootm loados; bootm go
```

You should see the firmware, crossconhyp and its guests printing on the UARTs.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ45 socket.


You should see crossconhyp and its guest printing on the UART.

After Linux finishes booting run xtest as follows:
``` sh
xtest -t regression
```

### Demo 2
This demo instantiates a Linux VM and two OP-TEE VMs.
``` sh
./build-demo-dual-vtee.sh
```

After Linux finishes booting you may execute xtest in both OP-TEE VMs.
``` sh
xtest -t regression
xtest2 -t regression
```



