# Docker for CROSSCON Hypervisor

## Overview

The purpose of this repo is to provide the environment required for
compilation of the CROSSCON hypervisor.

## How to build the container

Run this command from top of the repository:

```bash
docker build -t crosscon_hv -f env/Dockerfile .
```

to build the docker image. The resulting image will have a `crosscon_hv` tag.
After the image has been built, a container can be created and started directly
by running this command:

```bash
docker run -d --name crosscon_hv_container crosscon_hv tail -f /dev/null
```

> This step also is dependant on how you want to Flash the SD card later. All
approaches are specified [below](#Copying-the-files-to-the-SD-card.).

Then, to enter the shell of the running container, this command can be used:

```bash
docker exec -it crosscon_hv_container /bin/bash
```

> Note: If at any point when rebuilding/rerunning the container, error messages
> similar to this one pop up:
>
> ```bash
> docker: Error response from daemon: Conflict. The container name
> "/crosscon_hv_container" is already in use by container
> "d6ee75901fd0e090147d242c485651ebf5c4fc58e13d8363725a6cab830a9ba0". You have to
> remove (or rename) that container to be able to reuse that name.
> See 'docker run --help'.
> ```
>
> This command should be used:
>
> ```bash
> docker rm --force <hash_of_the_container_from_the_error_message>
> ```
>
> Alternatively:
>
> ```bash
> docker rm --force crosscon_hv_container
> ```

## Building the rpi4-ws demo

This section contains instructions on how to build and flash the rpi4-ws demo.
After attaching to the container, it should look like this:

```bash
user in ~/CROSSCON-Hypervisor-and-TEE-Isolation-Demos/env λ docker exec -it crosscon_hv_container /bin/bash
root@d6ee75901fd0:/work#
```

Then `cd crosscon` should be ran, and the instructions from
[the README](../rpi4-ws/README.md) followed.

This will allow the binaries to be built, since the container has all the
necessary dependencies.

## Copying the files to the SD card.

Obviously, the container allows the binaries to be built without worrying about
dependencies. But in the end they have to end up on the SD card in order to
boot the demo.

### Running the container as privileged

You can pass all your devices to the container and use the
[Prepare SDCard](https://github.com/3mdeb/CROSSCON-Hypervisor-and-TEE-Isolation-Demos/tree/master/rpi4-ws#prepare-sdcard)
instructions directly from inside of it using the `--privileged` flag or more restrictively pass only the
single SD card.

```bash
docker run -d --name crosscon_hv_container_copy --privileged crosscon_hv_copy tail -f /dev/null
```

```bash
docker run -d --name crosscon_hv_container_copy --device=/dev/sdX:/dev/sdX crosscon_hv_copy tail -f /dev/null
```

### Unprivileged container

#### Firmware and bootloader files

> Note: Before running the following commands, ensure that the SD card is
> inserted into the host machine and that it is mounted.
> (adjust the commands in this README to fit your SD card mount point).

These commands can be ran to copy over the firmware to the SD card (this
syntax assumes they are being copied directly from the container to the SD
card):

```bash
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/firmware/boot/ $SDCARD_MOUNT/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/config.txt $SDCARD_MOUNT/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/bin/bl31.bin $SDCARD_MOUNT/
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/bin/u-boot.bin $SDCARD_MOUNT/
```

#### Linux and Device Tree Image

After the Linux kernel has been build and lloader has been used to produce
`linux-rpi4.bin`, that file will need to be copied out of the container and onto
the SD card:

```bash
docker cp crosscon_hv_container:/work/crosscon/lloader/linux-rpi4.bin $SDCARD_MOUNT/
```

#### Copying the CROSSCON Hypervisor Binary

Building and copying of the hypervisor binary is done in the same script,
either `build-demo-vtee.sh` or `build-demo-dual-vtee.sh`. By looking at the
content of those scripts, it can be determined that they build the
`crossconhyp.bin` files, then copies `start*` firmware files and
`crossconhyp.bin` to the SD card mount point.

> Note: this readme only covers the `build-demo-vtee.sh` script, since
> the idea of what is being here is understood, it becomes easy to think
> of the commands to achieve what goes on in the `dual` version of the script.

The hypervisor will have to be built manually, then copied over to the host.
Once the end of the [the README](../rpi4-ws/README.md) demo has been reached,
instead of just running the script:

```bash
./build-demo-vtee.sh
```

those files will have to be built manually:

```bash
CONFIG_REPO=`pwd`/configs

pushd ..

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

Then confirm that the hypervisor got built correctly, and that the firmware
files exist:

```bash
ls /work/crosscon/rpi4-ws/bin/
ls /work/crosscon/CROSSCON-Hypervisor/bin/rpi4/builtin-configs/rpi4-single-vTEE/
```

Then finally those files can be copied over to host:

```bash
sudo docker cp crosscon_hv_container:/work/crosscon/rpi4-ws/firmware/boot/start* $SDCARD_MOUNT/
sudo docker cp crosscon_hv_container:/work/crosscon/CROSSCON-Hypervisor/bin/rpi4/builtin-configs/rpi4-single-vTEE/crossconhyp.bin $SDCARD_MOUNT/
```


## QEMU build

The docker image contains all the neccessary dependencies to build the QEMU
images as well (RISCV included), so all that needs to be done is following
the instructions from [the readme](../README.md).
