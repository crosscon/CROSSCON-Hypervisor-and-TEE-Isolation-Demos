# Docker for CROSSCON Hypervisor

## Overview

The purpose of this repo is to provide the environment required for
compilation of the CROSSCON hypervisor.

## How to build the container

Run this command:

```bash
docker build -t crosscon_hv .
```

to build the docker image. The resulting image will have a `crosscon_hv` tag.
After the image has been built you can create and start a container directly by
running:

```bash
docker run -d --name crosscon_hv_container crosscon_hv tail -f /dev/null
```

Then, to enter the shell of the running container, use this command:

```bash
docker exec -it crosscon_hv_container /bin/bash
```

> Note: If at any point when rebuilding/rerunning the container you get error messages
> similar to this one:

```bash
docker: Error response from daemon: Conflict. The container name
"/crosscon_hv_container" is already in use by container
"d6ee75901fd0e090147d242c485651ebf5c4fc58e13d8363725a6cab830a9ba0". You have to
remove (or rename) that container to be able to reuse that name.
See 'docker run --help'.
```

Just use this command:

```bash
docker rm --force <hash_of_the_container_from_the_error_message>
```

## Building the rpi4-ws demo

This section contains instructions on how to build and flash the rpi4-ws demo.
After attaching to the container, it should look like this:

```bash
user in ~/CROSSCON-Hypervisor-and-TEE-Isolation-Demos/env λ docker exec -it crosscon_hv_container /bin/bash
root@d6ee75901fd0:/work#
```

Then you should `cd crosscon`, and follow the instructions from
[the README](../rpi4-ws/README.md).

This will allow you to build the binaries, since the container has all the
necessary dependencies, but here are some important tips to bear in mind:

### Cross-compilator names

Sometimes, the instructions are not 100% accurate when it comes to the
`CROSS_COMPILE` flag in the `make` commands. The only time i have come across
this, is when trying to compile the `optee_client`, the readme says to do this:

```bash
make CROSS_COMPILE=aarch64-none-linux-gnu- WITH_TEEACL=0 O=out-aarch64
```

but the correct command for debian (which our container is based off of) is this:

```bash
make CROSS_COMPILE=aarch64-linux-gnu- WITH_TEEACL=0 O=out-aarch64
```

as you can see the `CROSS_COMPILE` argument should be `aarch64-linux-gnu-`, NOT
`aarch64-none-linux-gnu-`. If you ever encounter build errors, this should be
the first thing to check.

> Note: while `aarch64-none-linux-gnu-` is supposed to be the bare-metal
> cross-compiler, having it on the container causes internal compiler errors and
its unusable. `aarch64-linux-gnu-` Seens to work just fine for our purposes.

### Copying the files to the SD card.

Obviously, the container allows you to build the binaries without worrying about
dependencies. But you also have to get them on the SD card in order to boot
the demo.

#### Firmware and bootloader files

In order to correctly do this, first you have to make sure get the firmware
files over from the container on your host. Instead of doing this:

> Note: Before running the following commands, ensure you have inserted the
> SD card into your host machine and that it is mounted at /media/$USER/boot
> (adjust the path as necessary if your system uses a different mount point).

```bash
cd $RPI4_WS
SDCARD=/media/$USER/boot

cp -vr firmware/boot/* $SDCARD
cp -v config.txt $SDCARD
cp -v bin/bl31.bin $SDCARD
cp -v bin/u-boot.bin $SDCARD
```

like the readme tells you to, you will have to run these commands on host
(here I assumed that we are copying the files directly over to the SD card,
but it doesnt really matter. If you want to, you can copy them to host,
then later to the SD card):

```bash
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/firmware/boot/ /run/media/$USER/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/config.txt /run/media/$USER/boot/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/bin/bl31.bin /run/media/$USER/boot/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/bin/u-boot.bin /run/media/$USER/boot/
```

#### Linux and Device Tree Image

After you’ve built the Linux kernel and used lloader to produce
`linux-rpi4.bin`, you’ll need to copy that file out of the container and onto
your SD card:

```bash
docker cp crosscon_hv_container:/work/crosscon/lloader/linux-rpi4.bin /media/$USER/boot
```

#### Copying the CROSSCON Hypervisor Binary

Building and copying of the hypervisor binary is done in the same script,
either `build-demo-vtee.sh` or `build-demo-dual-vtee.sh`. If you look at the
content of those scripts, you will see that they build the `crossconhyp.bin`
files, then copies `start*` firmware files and `crossconhyp.bin` to the SD card
mount point.

> Note: this readme only covers the `build-demo-vtee.sh` script, since once
> you understand the idea of what we are doing here, it becomes easy to think
> of the commands to achieve what goes on in the `dual` version of the script.

Since we are inside a container, we will have to build the hypervisor manually,
then copy it over to host. Once you get to the end of
[the README](../rpi4-ws/README.md), instead of just running the script:

```bash
./build-demo-vtee.sh
```

we will have to build those files manually:

```bash
cd /work/crosscon

make -C CROSSCON-Hypervisor/ \
    PLATFORM=rpi4 \
    CONFIG_BUILTIN=y \
    CONFIG_REPO=$CONFIG_REPO \
    CONFIG=rpi4-single-vTEE \
    OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
    CROSS_COMPILE=aarch64-none-elf- \
        clean

make -C CROSSCON-Hypervisor/ \
    PLATFORM=rpi4 \
    CONFIG_BUILTIN=y \
    CONFIG_REPO=$CONFIG_REPO \
    CONFIG=rpi4-single-vTEE \
    OPTIMIZATIONS=0 \
        SDEES="sdSGX sdTZ" \
    CROSS_COMPILE=aarch64-none-elf- \
        -j`nproc`
```

Then make sure that the hypervisor got built correctly, and that the firmware files
exist:

```bash
ls /work/crosscon/rpi4-ws/bin/
ls /work/crosscon/CROSSCON-Hypervisor/bin/rpi4/builtin-configs/rpi4-single-vTEE/
```

Then finally copy those files over to the host:

```bash
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/firmware/boot/start* $SDCARD_MOUNT/
sudo docker cp crosscon_hv_container:/work/crosscon/CROSSCON-Hypervisor/bin/rpi4/builtin-configs/rpi4-single-vTEE/crossconhyp.bin $SDCARD_MOUNT/
```


## QEMU build

The docker image contains all the neccessary dependencies to build the QEMU
images as well (RISCV included), so all that needs to be done is following
the instructions from [the readme](../README.md).
