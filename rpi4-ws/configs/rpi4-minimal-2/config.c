#include <config.h>

// Linux Image
VM_IMAGE(linux_image, "../lloader/linux-rpi4.bin");

// Linux VM configuration
struct vm_config linux = {
    .image = {
        .base_addr = 0x20200000,
        .load_addr = VM_IMAGE_OFFSET(linux_image),
        .size = VM_IMAGE_SIZE(linux_image),
    },
    .entry = 0x20200000,
    .colors = 0x1,

    .type = 0,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x20000000,
                .size = 0x38000000,
            }
        },
        .dev_num = 4,
        .devs =  (struct dev_region[]) {
            {
                .pa   = 0xfc000000,
                .va   = 0xfc000000,
                .size = 0x03000000,

            },
            {
                .pa   = 0x600000000,
                .va   = 0x600000000,
                .size = 0x200000000,

            },
            {
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {
                    0x5d + 32 // serial
                }
            },
            {
                /* Arch timer interrupt */
                .interrupt_num = 2,
                .interrupts = (irqid_t[]) {
                    27,
                    0x10 + 32, // PMU
                }
            }
        },
        .arch = {
            .gic = {
                .gicd_addr = 0xff841000,
                .gicc_addr = 0xff842000,
            }
        }
    }
};

// Linux Image
VM_IMAGE(linux_image2, "../lloader/linux2-rpi4.bin");

// Linux VM configuration
struct vm_config linux2 = {
    .image = {
        .base_addr = 0x20200000,
        .load_addr = VM_IMAGE_OFFSET(linux_image2),
        .size = VM_IMAGE_SIZE(linux_image2),
    },
    .entry = 0x20200000,
    .colors = 0x2,

    .type = 0,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x20000000,
                .size = 0x38000000,
            }
        },
        .dev_num = 4,
        .devs =  (struct dev_region[]) {
            {
                .pa   = 0xfc000000,
                .va   = 0xfc000000,
                .size = 0x03000000,

            },
            {
                .pa   = 0x600000000,
                .va   = 0x600000000,
                .size = 0x200000000,
            },
            {
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) { 0x79 + 32 } // serial
            },
            {
                /* Arch timer interrupt */
                .interrupt_num = 2,
                .interrupts = (irqid_t[]) {
                    27,
                    0x15 + 32, // PMU
                }
            }
        },
        .arch = {
            .gic = {
                .gicd_addr = 0xff841000,
                .gicc_addr = 0xff842000,
            }
        }
    }
};

struct config config = {
    CONFIG_HEADER
    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = {.size = 0x00200000,},
    },
    .vmlist_size = 2,
    .vmlist = {
        &linux,
        &linux2
    }
};
