#include <config.h>

// Linux Image
VM_IMAGE(linux_image, "../lloader/linux-zcu.bin");

// Linux VM configuration
struct vm_config linux = {
    .image = {
        .base_addr = 0x00200000,
        .load_addr = VM_IMAGE_OFFSET(linux_image),
        .size = VM_IMAGE_SIZE(linux_image),
    },
    .entry = 0x00200000,

    .type = 0,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x00000000,
                .size = 0x20000000,
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x61000000,
                .size = 0x00200000,
                .shmem_id = 0,
            },
        },
        .dev_num = 4,
        .devs =  (struct dev_region[]) {
            {
                .pa   = 0x80000000,
                .va   = 0x80000000,
                .size = 0x79010000,
            },
            /* Hole for GIC */
            {
                .pa   = 0xF9030000,
                .va   = 0xF9030000,
                .size = 0x06FCFFFF,
            },
            {
                .interrupt_num = 148,
                .interrupts =
                    (irqid_t[]) {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                        51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
                        65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
                        79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
                        93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
                        105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
                        116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
                        127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
                        138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
                        149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                        160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
                        171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
                        182, 183, 184, 185, 186, 187}
            },
            {
                /* Arch timer interrupt */
                .interrupt_num = 1,
                .interrupts =
                    (irqid_t[]) {27}
            },
        },
        .arch = {
            .gic = {
                .gicc_addr = 0xF9020000,
                .gicd_addr = 0xF9010000
            },
        }
    }
};

VM_IMAGE(optee_os_image, "../optee_os/optee-aarch64/core/tee-pager_v2.bin");
struct vm_config optee_os = {
    .image = {
        .base_addr = 0x60000000,
        .load_addr = VM_IMAGE_OFFSET(optee_os_image),
        .size = VM_IMAGE_SIZE(optee_os_image),
    },
    .entry = 0x60000000,
    .cpu_affinity = 0xf,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux },

    .type = 1,


    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0x60000000,
                .size = 0x00F00000, // 15 MB
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x61000000,
                .size = 0x00200000,
                .shmem_id = 0,
            }
        },
        .dev_num = 1,
        .devs = (struct dev_region[]) {
            {
                .va = 0xFF000000,
                .pa = 0xFF000000,
                .size = 0x10000,
            },
        },
        .arch = {
            .gic = {
                .gicc_addr = 0xF9020000,
                .gicd_addr = 0xF9010000
            },
        }
    },
};


struct config config = {

    CONFIG_HEADER
    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00200000, },
    },
    .vmlist_size = 1,
    .vmlist = {
        &optee_os
    }
};

