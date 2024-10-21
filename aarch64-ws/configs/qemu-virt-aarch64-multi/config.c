#include <config.h>

VM_IMAGE(baremetal_image, "../support/baremetal-aarch64.bin");

struct vm_config baremetal = {
    .image = {
        .base_addr = 0x50000000,
        .load_addr = VM_IMAGE_OFFSET(baremetal_image),
        .size = VM_IMAGE_SIZE(baremetal_image)
    },

    .entry = 0x50000000,

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x50000000,
                .size = 0x04000000
            }
        },

        .dev_num = 2,
        .devs =  (struct dev_region[]) {
            {
                /* PL011 */
                .pa = 0x90c0000,
                .va = 0x9000001,
                .size = 0x10000,
            },
            {
                /* Arch timer interrupt */
                .interrupt_num = 1,
                .interrupts =
                    (irqid_t[]) {27}
            }
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

VM_IMAGE(freertos_image, "../support/freertos-aarch64.bin");

struct vm_config freertos = {
    .image = {
        .base_addr = 0x0,
        .load_addr = VM_IMAGE_OFFSET(freertos_image),
        .size = VM_IMAGE_SIZE(freertos_image)
    },

    .entry = 0x0,

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x00000000,
                .size = 0x08000000
            }
        },

        .ipc_num = 1,
        .ipcs = (struct ipc []) {
            {
                .base = 0x70000000,
                .size = 0x00010000,
                .shmem_id = 3,
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {52}
            }
        },

        .dev_num = 2,
        .devs =  (struct dev_region[]) {
            {
                /* PL011 */
                .pa = 0x090c0000,
                .va = 0xff000000,
                .size = 0x10000,
            },
            {
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {27}
            }
        },

        .arch = {
            .gic = {
                .gicd_addr = 0xF9000000,
                .gicc_addr = 0xF9010000,
                .gicr_addr = 0xF90A0000,
            }
        }
    },
};


// Linux Image
VM_IMAGE(linux_image, "../lloader/linux-aarch64.bin");
// Linux VM configuration
struct vm_config linux = {
    .image = {
        .base_addr = 0x40200000,
        .load_addr = VM_IMAGE_OFFSET(linux_image),
        .size = VM_IMAGE_SIZE(linux_image),
    },
    .entry = 0x40200000,

    .type = 0,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x40000000,
                .size = 0x30000000,
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0xf0000000,
                .size = 0x00010000,
                .shmem_id = 3,
                .interrupts = (irqid_t[]) {52}
            },
        },
        .dev_num = 2,
        .devs =  (struct dev_region[]) {
            {
                .pa = 0x9000000,
                .va = 0x9000000,
                .size = 0x10000,
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {33}
            },
	    {
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) { 27 }
	    },
        },
        .arch = {
            .gic = {
                .gicd_addr = 0x8000000,
                .gicc_addr = 0x8010000,
                .gicr_addr = 0x80A0000,
            }
        }
    }
};



struct config config = {

    CONFIG_HEADER
    .shmemlist_size = 4,
    .shmemlist = (struct shmem[]) {
        [3] = { .size = 0x00010000, },
    },
    .vmlist_size = 3,
    .vmlist = {
        &linux,
        &baremetal,
        &freertos,
    }
};

