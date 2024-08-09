#include <config.h>

VM_IMAGE(optee2_os_image, "../optee_os/optee2-aarch64/core/tee-pager_v2.bin");

struct vm_config optee2_os = {
    .image = {
        .base_addr = 0x62000000,
        .load_addr = VM_IMAGE_OFFSET(optee2_os_image),
        .size = VM_IMAGE_SIZE(optee2_os_image),
    },
    .entry = 0x62000000,
    .cpu_affinity = 0xf,


    .type = 2,

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0x62000000,
                .size = 0x00F00000, // 15 MB
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x63000000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },
        .dev_num = 1,
        .devs = (struct dev_region[]) {
            {
                .pa = 0xFF000000,
                .va = 0xFF000000,
                .size = 0x1000,
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

    .children_num = 1,
    .children = (struct vm_config*[]) { &optee2_os },

    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions =  (struct mem_region[]) {
            {
                .base = 0x00000000,
                .size = 0x20000000
            }
        },
        .ipc_num = 2,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x61000000,
                .size = 0x00200000,
                .shmem_id = 0,
            },
            {
                .base = 0x63000000,
                .size = 0x00200000,
                .shmem_id = 1,
            },

        },
        .dev_num = 2,
        .devs =  (struct dev_region[]) {
            {
                /* UART1 */
                .pa = 0xFF010000,
                .va = 0xFF010000,
                .size = 0x1000,
                .interrupt_num = 1,
                .interrupts =
                    (irqid_t[]) {54}
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
                .pa = 0xFF000000,
                .va = 0xFF000000,
                .size = 0x1000,
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
    .shmemlist_size = 2,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00200000, },
        [1] = { .size = 0x00200000, },
    },
    .vmlist_size = 1,
    .vmlist = {
        &optee_os
    }
};

