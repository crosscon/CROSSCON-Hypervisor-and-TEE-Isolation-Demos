#include <config.h>

VM_IMAGE(linux_img, "../lloader/linux-riscv64.bin");
struct vm_config linux = {
    .image = {
        .base_addr = 0x81600000,
        .load_addr = VM_IMAGE_OFFSET(linux_img),
        .size = VM_IMAGE_SIZE(linux_img)
    },

    .entry = 0x81600000,
    .cpu_affinity = 0x1,

    .type = 0,

    .platform = {
        .cpu_num = 1,

        .region_num = 2,
        .regions =  (struct mem_region[]) {
	    {
		.base = 0x81200000,
		.size = 0x07d00000
	    },
	    /* { */
		/* /1* HOLE FOR IPC *1/ */
		/* .base = 0x88f00000, */
                /* .size = 0x00200000, */
	    /* }, */
	    {
		.base = 0x89100000,
		.size = 0x16f00000
	    },
        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x88f00000,
                .size = 0x00200000,
                .shmem_id = 0,
            }
        },

	.dev_num = 1,
	.devs =  (struct dev_region[]) {
	    {
		.pa = 0x10000000,
		.va = 0x10000000,
		.size = 0x1000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {10}
	    },
	},

        .arch = {
            .plic_base = 0xc000000,
        }
    },
};



VM_IMAGE(optee_img, "../optee_os/optee-riscv/core/tee.bin");
struct vm_config optee = {
    .image = {
        .base_addr = 0x80200000,
        .load_addr = VM_IMAGE_OFFSET(optee_img),
        .size = VM_IMAGE_SIZE(optee_img)
    },

    .entry = 0x80200000,
    .cpu_affinity = 0x1,

    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux },

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {

	    {
		.base = 0x80200000,
		.size = 0x00f00000
	    },

        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x88f00000,
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


VM_IMAGE(linux2_img, "../lloader/linux2-riscv64.bin");
struct vm_config linux2 = {
    .image = {
        .base_addr = 0x81600000,
        .load_addr = VM_IMAGE_OFFSET(linux2_img),
        .size = VM_IMAGE_SIZE(linux2_img)
    },

    .entry = 0x81600000,

    .cpu_affinity = 0x2,

    .type = 0,

    .platform = {
        .cpu_num = 1,

        .region_num = 2,
        .regions =  (struct mem_region[]) {
	    {
		.base = 0x81200000,
		.size = 0x07d00000
	    },
	    /* { */
		/* /1* HOLE FOR IPC *1/ */
		/* .base = 0x88f00000, */
                /* .size = 0x00200000, */
	    /* }, */
	    {
		.base = 0x89100000,
		.size = 0x16f00000
	    },
        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x88f00000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },

	.dev_num = 1,
	.devs =  (struct dev_region[]) {
	    {
		.pa   = 0x10001000,
		.va   = 0x10000000,
		.size = 0x00001000,
		.interrupt_num = 1,
		.interrupts = (irqid_t[]) {12}
	    },
        },

        .arch = {
            .plic_base = 0xc000000,
        }
    },
};



VM_IMAGE(optee2_img, "../optee_os/optee-riscv/core/tee.bin");
struct vm_config optee2 = {
    .image = {
        .base_addr = 0x80200000,
        .load_addr = VM_IMAGE_OFFSET(optee2_img),
        .size = VM_IMAGE_SIZE(optee2_img)
    },

    .entry = 0x80200000,

    .cpu_affinity = 0x2,

    .type = 1,

    .children_num = 1,
    .children = (struct vm_config*[]) { &linux2 },

    .platform = {
        .cpu_num = 1,

        .region_num = 1,
        .regions =  (struct mem_region[]) {

	    {
		.base = 0x80200000,
		.size = 0x00f00000
	    },

        },

        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x88f00000,
                .size = 0x00200000,
                .shmem_id = 1,
            }
        },

        .dev_num = 1,
        .devs =  (struct dev_region[]) {
            {
                /* UART */
                .pa = 0x10001000,
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

    .vmlist_size = 2,
    .vmlist = {
        &optee,
        &optee2,
    }
};


