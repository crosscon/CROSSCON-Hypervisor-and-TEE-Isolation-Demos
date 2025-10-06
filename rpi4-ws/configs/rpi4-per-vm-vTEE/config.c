#include <config.h>

VM_IMAGE(host_linux_image, "../lloader/linux-rpi4.bin");
VM_IMAGE(nexmon_image, "../nexmon/nexmon.bin");
VM_IMAGE(optee_os_image, "../optee_os_cba/optee-cba-rpi4/core/tee-pager_v2.bin");
VM_IMAGE(optee2_os_image, "../optee_os/optee2-rpi4/core/tee-pager_v2.bin");

/* Notes
CPU CORE ASSIGNMENT: 1,1,2,2 (host/optee_os1/nexmon/optee_os2) -> bitmap 0x8, 0x4, 0x3, 0xf
Please set the paths to the .bin files appropriately.
Memory layout:
- Nexmon is from 0x20000000 -> 0x60000000 (dts: thinks 0x20000000 as well)
- Host is from 0x60000000 -> 0xa0000000 (dts: thinks 0x60000000 as well)
- OPTEE_OS 1 is from 0x10100000 -> 0x11100000
- OPTEE_OS 2 is from 0xb0100000 -> 0xb1100000
*/

/* The VM with PKCS#11 TA */
struct vm_config optee2_os = {
    .image = {
        .base_addr = 0xb0100000,
        .load_addr = VM_IMAGE_OFFSET(optee2_os_image),
        .size = VM_IMAGE_SIZE(optee2_os_image),
    },
    .entry = 0xb0100000,
    .cpu_affinity = 0xf,


    .type = 2,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0xb0000000,
                .size = 0x00F00000, // 15 MB
                .place_phys = true,
                .phys = 0xb0000000
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0xc8200000,
                .size = 0x00200000,
                .shmem_id = 2,
            }
        },
        .dev_num = 0,
        .devs = (struct dev_region[]) {
/*            {
                .pa = 0xfe215000,
                .va = 0xfe215000,
                .size = 0x10000,
            },*/
        },
        .arch = {
            .gic = {
                .gicc_addr = 0x8010000,
                .gicd_addr = 0x8000000,
                .gicr_addr = 0x80A0000,
            }
        }
    },
};


// Linux VM configuration
struct vm_config host_linux = {
    .image = {
        .base_addr = 0x60200000,
        .load_addr = VM_IMAGE_OFFSET(host_linux_image),
        .size = VM_IMAGE_SIZE(host_linux_image),
    },
    .entry = 0x60200000,
    .cpu_affinity = 0x8,

    .type = 0,

    .children_num = 1,
    .children = (struct vm_config*[]) { &optee2_os },

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x60000000,
                .size = 0x40000000,
                .place_phys = true,
                .phys = 0x60000000
            }
        },
        .ipc_num = 3,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x08000000,
                .size = 0x00200000,
                .shmem_id = 0,
            },
            {
                .base = 0x09000000,
                .size = 0x00800000,
                .shmem_id = 1,
            },
            {
                .base = 0xc8200000,
                .size = 0x00200000,
                .shmem_id = 2,
            }
        },
        .dev_num = 6,
        .devs = (struct dev_region[]) {
                {
                        .pa   = 0xfc000000,
                        .va   = 0xfc000000,
                        .size = 0x03000000
                },
                { // maybe needed for ethernet device communication due to scb
                  // device section
                        .pa   = 0x600000000,
                        .va   = 0x600000000,
                        .size = 0x40000000
                },
                { // ARCH timer interrupt
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {
                                27
                        }
                },
                { // this is not the timer device but still necessary.
                  // (hardware-level)
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {
                                32,
                        }
                },
                { // arm-pmu (hardware-level)
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {
                                53// or 48 (not based on which interrupt the
                                  // device in the dts has set. But still the
                                  // device's dts should have interrupts either
                                  // 0x10 or 0x15
                        }
                },
                { // soc (mailbox, ethernet, serial(uart))
                        .interrupt_num = 4,
                        .interrupts = (irqid_t[]) {
                                66,
                                189, 190,
                                125,
                        }
                },
        },
        .arch = { /* GLOBAL INTERRUPT CONTROLLER. Can be found under soc node
                     (with address translation keep in mind) */
            .gic = {
                .gicd_addr = 0xff841000,
                .gicc_addr = 0xff842000,
                .gicr_addr = 0xff844000,        /* <<< Based on some other
                                                   config somewhere this should
                                                   probably rather be gich_addr,
                                                   but leaving it like this also
                                                   works */
            }
        }
    }
};

struct vm_config nexmon_linux = {
    .image = {
        .base_addr = 0x20200000,
        .load_addr = VM_IMAGE_OFFSET(nexmon_image),
        .size = VM_IMAGE_SIZE(nexmon_image),
    },
    .entry = 0x20200000,
    .cpu_affinity = 0x3,

    .type = 0,

    .platform = {
        .cpu_num = 2,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x20000000,
                .size = 0x40000000,
                .place_phys = true,
                .phys = 0x20000000
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x09000000,
                .size = 0x00800000,
                .shmem_id = 1,
            },
        },
        .dev_num = 4,
        .devs = (struct dev_region[]) {
                {
                        .pa   = 0xfc000000,
                        .va   = 0xfc000000,
                        .size = 0x03000000
                },
                { // ARCH timer interrupt
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {
                                27
                        }
                },
                { // arm-pmu (hardware-level)
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {
                                48// or 53 (same argumentation as above)
                        }
                },
                { // soc (mailbox, wifi)
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) {
                                65,
                                158,
                        }
                },
        },
        .arch = {
            .gic = {
                .gicd_addr = 0xff841000,
                .gicc_addr = 0xff842000,
                .gicr_addr = 0xff844000,
            }
        }
    }
};

struct vm_config optee_os = {
    .image = {
        .base_addr = 0x10100000,
        .load_addr = VM_IMAGE_OFFSET(optee_os_image),
        .size = VM_IMAGE_SIZE(optee_os_image),
    },
    .entry = 0x10100000,
    .cpu_affinity = 0x4,

    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &host_linux },

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0x10100000,
                .size = 0x00F00000, // 15 MB
                .place_phys = true,
                .phys = 0x10100000
            }
        },
        .ipc_num = 2,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x08000000,
                .size = 0x00200000,
                .shmem_id = 0,
            },
            {
                .base = 0x09000000,
                .size = 0x00800000,
                .shmem_id = 1,
            }
        },
        .dev_num = 0,
        .devs = (struct dev_region[]) {
//            {
//                /* UART1 */
//                .pa = 0xfe215000,
//                .va = 0xfe215000,
//                .size = 0x1000,
//            },
//            {
//                /* Arch timer interrupt */
//                .interrupt_num = 1,
//               .interrupts = (irqid_t[]) {27}
//            }
        },
        .arch = {
            .gic = {
                .gicd_addr = 0xff841000,
                .gicc_addr = 0xff842000,
            }
        }
    },
};

struct config config = {

    CONFIG_HEADER
    .shmemlist_size = 3,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00200000, }, // OPTEE_OS 1 <-> Host
        [1] = { .size = 0x00800000, }, // OPTEE_OS 1 <-> NEXMON
        [2] = { .size = 0x00200000, } // OPTEE_OS 2 <-> Host
    },
    .vmlist_size = 2,
    .vmlist = {
        &optee_os,
        &nexmon_linux
    }
};


