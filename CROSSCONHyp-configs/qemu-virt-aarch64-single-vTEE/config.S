	.arch armv8-a
	.file	"config.c"
	.text
	.global	config
	.section	.config,"aw"
	.align	3
	.type	config, %object
	.size	config, 96
config:
	.word	-302117424
	.word	671088640
	.word	671088640
	.word	671088640
	.word	671088640
	.word	285212672
	.word	33554432
	.zero	12
	.xword	_config_end
	.xword	_images_end
	.zero	8
	.xword	2
	.xword	__compound_literal.10
	.xword	1
	.xword	optee_os
	// Start of user assembly
	.pushsection .vm_image_linux_image, "a"
	.global _linux_image_vm_beg
	_linux_image_vm_beg:
	.incbin "../prebuilt/qemu-virt-aarch64/linux-aarch64.bin"
	_linux_image_vm_end:
	.global _linux_image_vm_size
	.set _linux_image_vm_size,  (_linux_image_vm_end - _linux_image_vm_beg)
	.popsection
	// End of user assembly
	.data
	.align	3
	.type	__compound_literal.0, %object
	.size	__compound_literal.0, 40
__compound_literal.0:
	.xword	1073741824
	.xword	805240832
	.zero	24
	.align	3
	.type	__compound_literal.1, %object
	.size	__compound_literal.1, 80
__compound_literal.1:
	.xword	1879048192
	.xword	2097152
	.xword	1
	.zero	16
	.xword	1878982656
	.xword	65536
	.xword	0
	.zero	16
	.align	3
	.type	__compound_literal.2, %object
	.size	__compound_literal.2, 4
__compound_literal.2:
	.word	33
	.align	3
	.type	__compound_literal.3, %object
	.size	__compound_literal.3, 4
__compound_literal.3:
	.word	27
	.align	3
	.type	__compound_literal.4, %object
	.size	__compound_literal.4, 96
__compound_literal.4:
	.xword	150994944
	.xword	150994944
	.xword	65536
	.xword	1
	.xword	__compound_literal.2
	.zero	8
	.zero	24
	.xword	1
	.xword	__compound_literal.3
	.zero	8
	.global	linux
	.align	3
	.type	linux, %object
	.size	linux, 768
linux:
	.xword	1075838976
	.xword	_linux_image_vm_beg
	.xword	_linux_image_vm_size
	.zero	8
	.xword	1075838976
	.zero	16
	.xword	0
	.zero	16
	.xword	2
	.xword	1
	.xword	__compound_literal.0
	.xword	2
	.xword	__compound_literal.1
	.xword	2
	.xword	__compound_literal.4
	.zero	504
	.xword	134283264
	.zero	16
	.xword	134217728
	.xword	134873088
	.zero	16
	.zero	72
	// Start of user assembly
	.pushsection .vm_image_optee_os_image, "a"
	.global _optee_os_image_vm_beg
	_optee_os_image_vm_beg:
	.incbin "../prebuilt/qemu-virt-aarch64/tee-pager_v2.bin"
	_optee_os_image_vm_end:
	.global _optee_os_image_vm_size
	.set _optee_os_image_vm_size,  (_optee_os_image_vm_end - _optee_os_image_vm_beg)
	.popsection
	// End of user assembly
	.align	3
	.type	__compound_literal.5, %object
	.size	__compound_literal.5, 8
__compound_literal.5:
	.xword	linux
	.align	3
	.type	__compound_literal.6, %object
	.size	__compound_literal.6, 40
__compound_literal.6:
	.xword	269484032
	.xword	15728640
	.zero	24
	.align	3
	.type	__compound_literal.7, %object
	.size	__compound_literal.7, 40
__compound_literal.7:
	.xword	1879048192
	.xword	2097152
	.xword	1
	.zero	16
	.align	3
	.type	__compound_literal.8, %object
	.size	__compound_literal.8, 8
__compound_literal.8:
	.word	27
	.word	40
	.align	3
	.type	__compound_literal.9, %object
	.size	__compound_literal.9, 96
__compound_literal.9:
	.xword	150994944
	.xword	151257088
	.xword	65536
	.zero	24
	.zero	24
	.xword	1
	.xword	__compound_literal.8
	.zero	8
	.global	optee_os
	.align	3
	.type	optee_os, %object
	.size	optee_os, 768
optee_os:
	.xword	269484032
	.xword	_optee_os_image_vm_beg
	.xword	_optee_os_image_vm_size
	.zero	8
	.xword	269484032
	.xword	15
	.zero	8
	.xword	1
	.xword	1
	.xword	__compound_literal.5
	.xword	2
	.xword	1
	.xword	__compound_literal.6
	.xword	1
	.xword	__compound_literal.7
	.xword	1
	.xword	__compound_literal.9
	.zero	504
	.xword	134283264
	.zero	16
	.xword	134217728
	.xword	134873088
	.zero	16
	.zero	72
	.align	3
	.type	__compound_literal.10, %object
	.size	__compound_literal.10, 80
__compound_literal.10:
	.xword	65536
	.zero	32
	.xword	2097152
	.zero	32
	.ident	"GCC: (Arm GNU Toolchain 11.3.Rel1) 11.3.1 20220712"
