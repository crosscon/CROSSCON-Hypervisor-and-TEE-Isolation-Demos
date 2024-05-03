# CROSSCON Hypervisor and TEE Isolation

Here you can find demos for the initial version of the _CROSSCON Hypervisor_ and _CROSSCON TEE Isolation_ demos.
These demos are related to developed as part of the [CROSSCON project](https://crosscon.eu/). 
The repository contains on overview of the CROSSCON per-VM TEE feature working for both Arm and RISC-V qemu platforms. This feature can be used to achieve TEE isolation, by running multiple TEE VMs instead of a single TEE instance in the normal world.

This repository is part of the deliverable D3.2: CROSSCON Open Security Stack - Initial Version.

## Overview

### CROSSCON per VM TEEs
Refer to [CROSSCON Hypervisor](https://github.com/crosscon/CROSSCON-Hypervisor) for details on CROSSCON Hypervisor per-VM TEE support.

For detailed description, please refer to Sections 3.2 on the deliverable [D3.1: CROSSCON Open Security Stack Documentation ‐ Draft.](https://crosscon.eu/library/deliverables).

### CROSSCON TEE Isolation
**CROSSCON TEE Isolation** is developed as part of the [CROSSCON project](https://crosscon.eu/). It leverages the [CROSSCON Hypervisor](https://github.com/crosscon/CROSSCON-Hypervisor) to support multiple TEE VMs per VM. We've updated the TEE CROSSCON Hypervisor internal module, to be able to identify which TEE VM the GPOS is requesting interaction with. Additionally, this required modifications at the GPOS level, to be able to provide TEE driver instances for each TEE separately. This means that when performing SMC calls, the OS will use different IDs depending on the targeted trusted OS.

For detailed description, please refer to Sections 3.1 on the deliverable [D3.1: CROSSCON Open Security Stack Documentation ‐ Draft.](https://crosscon.eu/library/deliverables).


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


#### How to experiment CROSSCON Hypervisor Per-VM TEE feature?
This experiment will use OP-TEE as trusted OS and will be carry out in qemu.

To install qemu we recommend the following:

Clone the repo:

```
git clone --recursive git@github.com:qemu/qemu.git
cd qemu
git checkout v8.0.3
```

For Arm:
```
./configure --target-list=riscv64-softmmu && make -j16 && make install
```

For RISC-V:
```
./configure --target-list=aarch64-softmmu && make -j16 && make install
```

Install the aforementioned dependencies.
And execute one of the demo scripts:

For Arm:
```
./run-aarch64-demo.sh
```

For RISC-V:
```
./run-riscv64-demo.sh
```

This should boot you into a linux terminal after a few seconds.

Enter `root` as the user name.

After logging in as root issue the following command, to execute the tee supplicant as a background process for this session.

```
tee-supplicant &
```

Now run xtest to validate OP-TEE execution, and wait for execution to complete.

```
xtest -t regression
```

## License

See LICENSE file.

## Acknowledgments

The work presented in this repository is part of the [CROSSCON project](https://crosscon.eu/) that received funding from the European Union’s Horizon Europe research and innovation programme under grant agreement No 101070537.

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/themes/crosscon/images/eu.svg" width=10% height=10%>
</p>

<p align="center">
    <img src="https://crosscon.eu/sites/crosscon/files/public/styles/large_1080_/public/content-images/media/2023/crosscon_logo.png?itok=LUH3ejzO" width=25% height=25%>
</p>


