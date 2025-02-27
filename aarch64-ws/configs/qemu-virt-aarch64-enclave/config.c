#include <dynconfig.h>


VM_IMAGE(enclave, "/home/david/Documents/PARA/1-Projects/anytee/sgx_anytee_enclave/sgx_anytee_enclave.bin")

struct dynconfig dynconfig __attribute__((section(".dynconfig"))) = {

    DYN_CONFIG_HEADER


    .vm_cfg = {

        .image = {
            .base_addr = 0x40000000,
            .load_addr = VM_IMAGE_OFFSET(enclave),
            .size = VM_IMAGE_SIZE(enclave)
        },

        .entry = 0x40000000,
        .cpu_affinity = 0x1,

        .children_num = 0,

        .type = 3,

        .platform = {
            .cpu_num = 1,

            .region_num = 2,
            .regions =  (struct vm_mem_region[]) {
                {
                    .base = 0x40000000,
                    .size = 0x07fe0000
                },
                {
                    .base = 0x80000000,
                    .size = 0x00400000
                }
            },

            .dev_num = 1,
            .devs =  (struct vm_dev_region[]) {
                {
                    .pa = 0x9000000,
                    .va = 0xFF010000,
                    .size = 0x10000
                },

            },

            .ipc_num = 1,
            .ipcs = (struct ipc[]) {
                {
                    .base = 0x47fe0000,
                    .size = 0x00010000,
                    .shmem_id = 1
                        //.interrupt_num = 1,
                        //.interrupts = (uint64_t[]) {112}
                }
            },
        },
    },
};

