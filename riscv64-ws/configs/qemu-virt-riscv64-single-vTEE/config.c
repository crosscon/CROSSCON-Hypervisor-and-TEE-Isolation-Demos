#include <config.h>

VM_IMAGE(linux, "../lloader/linux-riscv64.bin");
struct vm_config linux = {
    .image = {
        .base_addr = 0x91600000,
        .load_addr = VM_IMAGE_OFFSET(linux),
        .size = VM_IMAGE_SIZE(linux)
    },

    .entry = 0x91600000,

    .type = 0,

    .platform = {
        .cpu_num = 1,

        .region_num = 2,
        .regions =  (struct mem_region[]) {
	    {
		.base = 0x91200000,
		.size = 0x07d00000
	    },
	    /* { */
		/* /1* HOLE FOR IPC *1/ */
		/* .base = 0x88f00000, */
                /* .size = 0x00200000, */
	    /* }, */
	    {
		.base = 0x99100000,
		.size = 0x16f00000
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
		.pa = 0x10000000,
		.va = 0x10000000,
		.size = 0x10000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {10}
	    },
	},

        .arch = {
            .plic_base = 0xc000000,
        }
    },
};



VM_IMAGE(optee, "../optee_os/optee-riscv/core/tee.bin");
struct vm_config optee = {
    .image = {
        .base_addr = 0xb0000000,
        .load_addr = VM_IMAGE_OFFSET(optee),
        .size = VM_IMAGE_SIZE(optee)
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

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        {
            .size = 0x00200000,
        }
    },

    .vmlist_size = 1,
    .vmlist = {
        &optee
    }
};


