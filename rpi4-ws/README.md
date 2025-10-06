# RPi4 CROSSCON Hypervisor Demo

This guide provides steps to leverage the CROSSCON Hypervisor on the RPi4 to
build virtual TEEs.

## Clone the repository

Clone the repository by executing following command.

```bash
git clone --recurse-submodules git@github.com:3mdeb/CROSSCON-Hypervisor-and-TEE-Isolation-Demos.git crosscon-demos && \
cd crosscon-demos
```

## Build & run the container

Build docker container.

```bash
docker build -t crosscon_hv -f rpi4-ws/Dockerfile .
```

The resulting image will have a `crosscon_hv` tag.
After the image has been built, execute `rpi4-ws/run.sh` script to run
the container.

```bash
rpi4-ws/run.sh
```

## Building the rpi4-ws demo

Inside the container, use below script to build the demo `rpi4-ws` package.

```bash
rpi4-ws/build.sh --all
```

This command will perform all steps. Run the command without any parameters to
see other options.

```bash
rpi4-ws/build.sh
```

## Creating and flashing the image

The following command can be used to build the hypervisor and create an image
with all required files included.

```bash
sudo rpi4-ws/create_hyp_img.sh
```

The command will output the image to `/work/crosscon/crosscon-demo-img.img`.
Note: The command must be run with `sudo`.

The built image can be then flashed to SD card.

```bash
sudo dd if=./crosscon-demo-img.img of=<drive> bs=4M conv=fsync
```

## Running the image

Use UART to USB adapter to connect RPI to your machine. Use pins
[6,8 and 10](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio)
to connect the UART converter to RPI. Make sure the logic level is set to 3.3V.
Launch `minicom` to access the command line.

```bash
minicom -D /dev/ttyUSB0 -b 115200
```

Supply power to RPI and hit any key when asked to stop u-boot from attempting
auto-boot.

```bash
[...]
scanning bus xhci_pci for devices... 2 USB Device(s) found
       scanning usb for storage devices... 0 Storage Device(s) found
Hit any key to stop autoboot:  0
U-Boot>
```

_Note: If you missed the timeframe, you can spam CTRL+C many times to achieve
same result._

Boot the image by manually loading it into the memory and "jumping" to it.

```bash
fatload mmc 0 0x200000 crossconhyp.bin; go 0x200000
```

### Options

One the image finishes booting you can run the following command to request ip
address via dhcp (ethernet only).

```bash
udhcpc -i eth0
```

You can also run regression test.

```bash
xtest -t regression
```

## Building other demo configurations

The default the hypervisor is built from `rpi4-single-vTEE` configuration,
resulting in a configuration with a Linux VM and OP-TEE VM.

### Demo 2
This demo instantiates a Linux VM and two OP-TEE VMs.

```bash
sudo rpi4-ws/create_hyp_img.sh --config=rpi4-dual-vTEE
```

Flash contents as described in
["Creating and flashing the Image" section](#creating-and-flashing-the-image).

After Linux finishes booting you may execute xtest in both OP-TEE VMs.

```bash
xtest -t regression
xtest2 -t regression
```

### Security test

**Note: For this demo, you'll need RPI 4 8GB version!**

* Build security test demo:

  ```bash
  rpi4-ws/build.sh --steps=10-10 --linux-target=linux-rpi4 --dts=rpi4-ws/rpi4-minimal.dts && \
  rpi4-ws/build.sh --steps=10-10 --linux-target=linux2-rpi4 --dts=rpi4-ws/rpi4-minimal2.dts && \
  sudo rpi4-ws/create_hyp_img.sh --config=rpi4-minimal-2
  ```

* Flash contents as described in
["Creating and flashing the Image" section](#creating-and-flashing-the-image)

* Connect second UART to RPI4. Board configuration:
    - [GPIO 4](https://pinout.xyz/pinout/pin7_gpio4/) - UART TX
    - [GPIO 5](https://pinout.xyz/pinout/pin29_gpio5/) - UART RX

* Boot normally, after a while you should see output on both `ttyUSB0` and
  `ttyUSB1`

* Follow `Run` section in [security test README](../security_test/README.md#run)

### Memory separation test

* Build baremetal app:

  ```sh
  rpi4-ws/build.sh --steps=10-10 --linux-target=linux2-rpi4 --dts=rpi4-ws/rpi4-minimal2.dts && \
  sudo rpi4-ws/create_hyp_img.sh --config=rpi4-baremetal
  ```
* Flash contents as described in
["Creating and flashing the Image" section](#creating-and-flashing-the-image)

* After a while you should boot into baremetal app. You can pass memory address
  in hexadecimal (with or without `0x` prefix) and app will try to read this
  address e.g.:

    ```txt
    Access (hex): 0x20000000
    Trying to access: 0x20000000
    Value: 0xff
    ```

    When trying to access ranges outside of memory assigned to baremetal app you
    should see data abort errors and VM will freeze

    ```txt
    CROSSCONHYP DATA ABORT: 0x1fffffff
    ```

    Second VM is just there to check if we can't access its memory, but you can
    connect second UART ([GPIO 4](https://pinout.xyz/pinout/pin7_gpio4) & [GPIO
    5](https://pinout.xyz/pinout/pin29_gpio5)) if you need to communicate with
    it.
