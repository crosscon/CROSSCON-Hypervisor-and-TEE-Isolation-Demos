#!/bin/bash -e

export CONFIG_REPO=`pwd`/configs

ENCLV=sgx-nbench
# ENCLV=sample_enclave
# ENCLV=YCSB-C-SGX
# ENCLV=sgx-mpk
# ENCLV=SGX-OpenSSL/SampleCode/Simple_TLS_Server
# ENCLV=SGX-OpenSSL/SampleCode/Simple_TLS_Client

cp -u ../prebuilt/qemu-virt-aarch64/bl1.bin ./
cp -u ../prebuilt/qemu-virt-aarch64/bl2.bin ./
cp -u ../prebuilt/qemu-virt-aarch64/bl31.bin ./

qemu-system-aarch64 \
    -nographic \
    -machine virt,secure=on, \
    -machine gic-version=2 \
    -machine virtualization=on \
    -cpu cortex-a53 \
    -smp 4 \
    -m 4G \
    -bios bl1.bin \
    -semihosting-config enable,target=native \
    -serial mon:stdio \
    -s \
    $@

