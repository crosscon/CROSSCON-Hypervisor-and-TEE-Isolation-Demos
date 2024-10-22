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
                .size = 0x2fff0000,
            }
        },
        .ipc_num = 1,
        .ipcs = (struct ipc[]) {
            {
                .base = 0x6fff0000,
                .size = 0x00010000,
                .shmem_id = 1,
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
struct config config = {

    CONFIG_HEADER
    .shmemlist_size = 2,
    .shmemlist = (struct shmem[]) {
        [1] = { .size = 0x00010000, },
    },
    .vmlist_size = 1,
    .vmlist = {
        &linux
    }
};

