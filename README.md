# CROSSCON TEE Isolation

Here you can find the initial version of the _CROSSCON TEE Isolation_: ... developed as part of the [CROSSCON project](https://crosscon.eu/). The repository contains on overview of the CROSSCON .... 

## Overview

**CROSSCON TEE Isolation** is developed as part of the [CROSSCON project](https://crosscon.eu/). It leverages the CROSSCON Hypervisor ...

A more detailed description of CROSSCON SoC, BA51-H and Perimeter guard can be found in deliverable 4.1 of the [CROSSCON project](https://crosscon.eu/).

## How to upload the bitstream
	                        
The CROSSCON SoC bitstream can be uploaded to the Arty-A7 100T board aither through Vivado Lab GUI or via terminal using Tool Command Language (TCL). Here we cover the letter and describe how to program the Arty-A7 over terminal (without the GUI). For how to program Arity-A7 over GUI see [the reference manual](https://digilent.com/reference/programmable-logic/arty-a7/reference-manual).

### Prerequisite
        
We assume that you are following the instructions on a PC with a working setup of Ubuntu or Debian OS.

In order to upload the CROSSCON SoC bitstream to the FPGA, you need to install Vivado Lab that is available [here](https://www.xilinx.com/support/download.html).

After installing Vivado Lab, make sure that `vivado_lab` command is available in your terminal.

Here is a list of demos that guides the CROSSCON Hypervisor use:
- How to prepare 2 VMs executing on top of CROSSCON Hypervisor?
- How to experiment CROSSCON Hypervisor Dynamic-VM feature?
- How to experiment CROSSCON Hypervisor Per-VM TEE feature?

#### How to prepare 2 VMs executing on top of CROSSCON Hypervisor?
Before deep dive into novel CROSSCON Hypervisor features, we should first familiarize with CROSSCON VM configuration.

For a step-by-step guide on how to run different demo configurations of the CROSSCONHyp hypervisor featuring multiple guest operating systems and targeting several platforms please refer to: [**Bao Hypervisor Demo Guide**](https://github.com/crossconhyp-project/crossconhyp-demos)

#### How to experiment CROSSCON Hypervisor Dynamic-VM feature?
To configure multiple Dynamic-VM features, this experiment will show you how to prepare a configuration file that follows a scenario containing a Linux OS alongside a Bare-metal application in a single CPU.
First, to configure two VMs we should follow the configuration file:

[VM Configuration file Example]

Setup process:
- Step 1:
- Step 2:
- Step 3:

#### How to experiment CROSSCON Hypervisor Per-VM TEE feature?
This experiment involves features of previous demos, combining the dynamic-VM feature together with trusted OS execution in normal world. This experiment will use OP-TEE as trusted OS and will be carry out in the APU platform XXX.

To support OP-TEE in CROSSCON, we need to two VMs at boot time, booting the VM with the trusted OS (secure world) before the VM executes the normal world OS (normal world). To establish the resources belonging to each VM, CROSSCON Hypervisor configuration file should follow the example:

[VM Configuration Example]

Setup process:
- Step 1:
- Step 2:
- Step 3:


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


