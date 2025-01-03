#include <config.h>
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
                .base = 0x70000000,
                .size = 0x00200000,
                .shmem_id = 0,
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
                .gicc_addr = 0x8010000,
                .gicd_addr = 0x8000000,
                .gicr_addr = 0x80A0000,
            }
        }
    }
};

VM_IMAGE(optee_image, "../optee_os/optee/core/tee-pager_v2.bin");

struct vm_config optee = {
    .image = {
        .base_addr = 0x10100000,
        .load_addr = VM_IMAGE_OFFSET(optee_image),
        .size = VM_IMAGE_SIZE(optee_image),
    },
    .entry = 0x10100000,
    .cpu_affinity = 0x2,


    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux },
    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0x10100000,
                .size = 0x00F00000, // 15 MB
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x70000000,
                .size = 0x00200000,
                .shmem_id = 0,
            }
        },
        .dev_num = 2,
        .devs = (struct dev_region[]) {
            {
                // PL011
                .va = 0x9040000,
                .pa = 0x9000000,
                .size = 0x10000,
            },
            {
                // Arch timer
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {27,40}
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


#include <config.h>
// Linux Image
VM_IMAGE(linux2_image, "../lloader/linux2-aarch64.bin");
// Linux VM configuration
struct vm_config linux2 = {
    .image = {
        .base_addr = 0x40200000,
        .load_addr = VM_IMAGE_OFFSET(linux2_image),
        .size = VM_IMAGE_SIZE(linux2_image),
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
                .base = 0x70000000,
                .size = 0x00200000,
                .shmem_id = 1,
            },

        },
        .dev_num = 2,
        .devs =  (struct dev_region[]) {
            {
                .pa = 0x90c0000,
                .va = 0x9000000,
                .size = 0x10000,
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {0x2a}
            },
	    {
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) { 0x1b }
	    },
        },
        .arch = {
            .gic = {
                .gicc_addr = 0x8010000,
                .gicd_addr = 0x8000000,
                .gicr_addr = 0x80A0000,
            }
        }
    }
};


VM_IMAGE(optee2_image, "../optee_os/optee/core/tee-pager_v2.bin");

struct vm_config optee2 = {
    .image = {
        .base_addr = 0x10100000,
        .load_addr = VM_IMAGE_OFFSET(optee2_image),
        .size = VM_IMAGE_SIZE(optee2_image),
    },
    .entry = 0x10100000,
    .cpu_affinity = 0x1,


    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux2 },
    .platform = {
        .cpu_num = 1,
        .region_num = 1,
        .regions = (struct mem_region[]) {
            {
                .base = 0x10100000,
                .size = 0x00F00000, // 15 MB
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x70000000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },
        .dev_num = 2,
        .devs = (struct dev_region[]) {
            {
                // PL011
                .va = 0x9040000,
                .pa = 0x90c0000,
                .size = 0x10000,
            },
            {
                // Arch timer
                .interrupt_num = 1,
                .interrupts = (irqid_t[]) {27}
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

struct config config = {

    CONFIG_HEADER
    .shmemlist_size = 2,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00200000, },
        [1] = { .size = 0x00200000, },
    },
    .vmlist_size = 2,
    .vmlist = {
        &optee,
        &optee2,
    }
};

