#include <config.h>

VM_IMAGE(optee2_os_image, "../optee_os/optee2-riscv/core/tee.bin");
struct vm_config optee2_os = {
    .image = {
        .base_addr = 0xb2000000,
        .load_addr = VM_IMAGE_OFFSET(optee2_os_image),
        .size = VM_IMAGE_SIZE(optee2_os_image)
    },

    .entry = 0xb2000000,

    .type = 2,

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {

	    {
		.base = 0xb2000000,
		.size = 0x00f00000
	    },

        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x99100000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },

        .dev_num = 1,
        .devs =  (struct dev_region[]) {
            {
                /* UART */
                .pa = 0x10000000,
                .va = 0x10000000,
                .size = 0x1000,
            },
        },

        .arch = {
            .plic_base = 0xc000000,
        }
    },
};


VM_IMAGE(linux_image, "../lloader/linux-riscv64.bin");
struct vm_config linux = {
    .image = {
        .base_addr = 0x91600000,
        .load_addr = VM_IMAGE_OFFSET(linux_image),
        .size = VM_IMAGE_SIZE(linux_image)
    },

    .entry = 0x91600000,

    .type = 0,

    .children_num = 1,
    .children = (struct vm_config*[]) { &optee2_os },

    .platform = {
        .cpu_num = 1,

        .region_num = 2,
        .regions =  (struct mem_region[]) {
	    {
		.base = 0x91200000,
		.size = 0x07d00000,
                .place_phys = true,
                .phys = 0x91200000
	    },
	    /* { */
		/* /1* HOLE FOR IPC *1/ */
		/* .base = 0x88f00000, */
                /* .size = 0x00200000, */
	    /* }, */
	    {
		.base = 0x99300000,
		.size = 0x16d00000,
                .place_phys = true,
                .phys = 0x99300000
	    },
        },

        .ipc_num = 2,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x98f00000,
                .size = 0x00200000,
                .shmem_id = 0,
            },
            {
                .base = 0x99100000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },

	.dev_num = 5,
	.devs =  (struct dev_region[]) {
	    {
		.pa = 0x10000000,
		.va = 0x10000000,
		.size = 0x10000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {10}
	    },
	    {
		.pa = 0x18000000,
		.va = 0x18000000,
		.size = 0x00001000,
		.interrupt_num = 4,
		.interrupts = (irqid_t[]) {4,5,6,7}
	    },
	    {
		.pa = 0x20000000,
		.va = 0x20000000,
		.size = 0x00001000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {2}
	    },
	    {
		.pa = 0x30000000,
		.va = 0x30000000,
		.size = 0x00008000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {3}
	    },
	    {
		.pa = 0x40000000,
		.va = 0x40000000,
		.size = 0x00010000,
		.interrupt_num = 0,
		.interrupts = (irqid_t[]) {}
	    },
	},

        .arch = {
            .plic_base = 0xc000000,
        }
    },
};



VM_IMAGE(optee_os_image, "../optee_os/optee-riscv/core/tee.bin");
struct vm_config optee_os = {
    .image = {
        .base_addr = 0xb0000000,
        .load_addr = VM_IMAGE_OFFSET(optee_os_image),
        .size = VM_IMAGE_SIZE(optee_os_image)
    },

    .entry = 0xb0000000,

    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux },

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {

	    {
		.base = 0xb0000000,
		.size = 0x00f00000
	    },

        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x98f00000,
                .size = 0x00200000,
                .shmem_id = 0,
            }
        },

        .dev_num = 1,
        .devs =  (struct dev_region[]) {
            {
                /* UART */
                .pa = 0x10000000,
                .va = 0x10000000,
                .size = 0x1000,
            },
        },

        .arch = {
            .plic_base = 0xc000000,
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


