# CROSSCON TEE Isolation

Here you can find the initial version of the _CROSSCON TEE Isolation_: ... developed as part of the [CROSSCON project](https://crosscon.eu/). The repository contains on overview of the CROSSCON .... 

## Overview

**CROSSCON TEE Isolation** is developed as part of the [CROSSCON project](https://crosscon.eu/). It leverages the [CROSSCON Hypervisor](https://github.com/crosscon/CROSSCON-Hypervisor).

A more detailed description of CROSSCON SoC, BA51-H and Perimeter guard can be found in deliverable 4.1 of the [CROSSCON project](https://crosscon.eu/).


### Prerequisite
We assume that you are following the instructions on a PC with a working setup of Ubuntu or Debian OS.
Before you jump to Demos you need to install each Tool in the following list:

| Tool                    | Version |
| ----------------------- | ------- |
| arm-none-eabi-gcc       | 11.3.1  |
| aarch64-none-elf-gcc    | 11.2.1  |
| riscv64-unknown-elf-gcc | 10.2.0  |
| make                    | 4.2.1   |
| dtc                     | 1.5.0   |
| gcc                     | 9.3.0   |
| mkimage                 | 20.10   |
| cmake                   | 3.20.0  |
| ninja                   | 1.10.1  |


#### How to experiment CROSSCON Hypervisor Dynamic-VM feature?
To configure multiple Dynamic-VM features, this experiment will show you how to prepare a configuration file that follows a scenario containing a Linux OS alongside a Bare-metal application in a single CPU.




#### How to experiment CROSSCON Hypervisor Per-VM TEE feature?
This experiment involves features of previous demos, combining the dynamic-VM feature together with trusted OS execution in normal world. This experiment will use OP-TEE as trusted OS and will be carry out in the APU platform qemu.


## Licence

See LICENCE file.

## Acknowledgments

The work presented in this repository is part of the [CROSSCON project](https://crosscon.eu/) that received funding from the European Union’s Horizon Europe research and innovation programme under grant agreement No 101070537.

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/themes/crosscon/images/eu.svg" width=10% height=10%>
</p>

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/files/public/styles/large_1080_/public/content-images/media/2023/crosscon_logo.png?itok=LUH3ejzO" width=25% height=25%>
</p>


