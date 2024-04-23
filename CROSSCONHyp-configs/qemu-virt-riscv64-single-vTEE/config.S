	.file	"config.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	config
	.section	.config,"aw"
	.align	3
	.type	config, @object
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
	.dword	_config_end
	.dword	_images_end
	.zero	8
	.dword	1
	.dword	__compound_literal.12
	.dword	1
	.dword	optee
 #APP
	.pushsection .vm_image_linux, "a"
	.global _linux_vm_beg
	_linux_vm_beg:
	.incbin "../prebuilt/qemu-virt-riscv64/linux-riscv64-qemu.bin"
	_linux_vm_end:
	.global _linux_vm_size
	.set _linux_vm_size,  (_linux_vm_end - _linux_vm_beg)
	.popsection
 #NO_APP
	.data
	.align	3
	.type	__compound_literal.0, @object
	.size	__compound_literal.0, 80
__compound_literal.0:
	.dword	2166358016
	.dword	131072000
	.zero	24
	.dword	2299527168
	.dword	384827392
	.zero	24
	.align	3
	.type	__compound_literal.1, @object
	.size	__compound_literal.1, 40
__compound_literal.1:
	.dword	2297430016
	.dword	2097152
	.dword	0
	.zero	16
	.section	.sdata,"aw"
	.align	3
	.type	__compound_literal.2, @object
	.size	__compound_literal.2, 4
__compound_literal.2:
	.word	10
	.data
	.align	3
	.type	__compound_literal.3, @object
	.size	__compound_literal.3, 16
__compound_literal.3:
	.word	4
	.word	5
	.word	6
	.word	7
	.section	.sdata
	.align	3
	.type	__compound_literal.4, @object
	.size	__compound_literal.4, 4
__compound_literal.4:
	.word	2
	.align	3
	.type	__compound_literal.5, @object
	.size	__compound_literal.5, 4
__compound_literal.5:
	.word	3
	.local	__compound_literal.6
	.comm	__compound_literal.6,0,8
	.data
	.align	3
	.type	__compound_literal.7, @object
	.size	__compound_literal.7, 240
__compound_literal.7:
	.dword	268435456
	.dword	268435456
	.dword	65536
	.dword	1
	.dword	__compound_literal.2
	.zero	8
	.dword	402653184
	.dword	402653184
	.dword	4096
	.dword	4
	.dword	__compound_literal.3
	.zero	8
	.dword	536870912
	.dword	536870912
	.dword	4096
	.dword	1
	.dword	__compound_literal.4
	.zero	8
	.dword	805306368
	.dword	805306368
	.dword	32768
	.dword	1
	.dword	__compound_literal.5
	.zero	8
	.dword	1073741824
	.dword	1073741824
	.dword	65536
	.dword	0
	.dword	__compound_literal.6
	.zero	8
	.globl	linux
	.align	3
	.type	linux, @object
	.size	linux, 648
linux:
	.dword	2170552320
	.dword	_linux_vm_beg
	.dword	_linux_vm_size
	.zero	8
	.dword	2170552320
	.zero	16
	.dword	0
	.zero	16
	.dword	1
	.dword	2
	.dword	__compound_literal.0
	.dword	1
	.dword	__compound_literal.1
	.dword	5
	.dword	__compound_literal.7
	.zero	504
	.dword	201326592
 #APP
	.pushsection .vm_image_optee, "a"
	.global _optee_vm_beg
	_optee_vm_beg:
	.incbin "../prebuilt/qemu-virt-riscv64/tee.bin"
	_optee_vm_end:
	.global _optee_vm_size
	.set _optee_vm_size,  (_optee_vm_end - _optee_vm_beg)
	.popsection
 #NO_APP
	.section	.sdata
	.align	3
	.type	__compound_literal.8, @object
	.size	__compound_literal.8, 8
__compound_literal.8:
	.dword	linux
	.data
	.align	3
	.type	__compound_literal.9, @object
	.size	__compound_literal.9, 40
__compound_literal.9:
	.dword	2149580800
	.dword	15728640
	.zero	24
	.align	3
	.type	__compound_literal.10, @object
	.size	__compound_literal.10, 40
__compound_literal.10:
	.dword	2297430016
	.dword	2097152
	.dword	0
	.zero	16
	.align	3
	.type	__compound_literal.11, @object
	.size	__compound_literal.11, 48
__compound_literal.11:
	.dword	268435456
	.dword	268435456
	.dword	4096
	.zero	24
	.globl	optee
	.align	3
	.type	optee, @object
	.size	optee, 648
optee:
	.dword	2149580800
	.dword	_optee_vm_beg
	.dword	_optee_vm_size
	.zero	8
	.dword	2149580800
	.zero	16
	.dword	1
	.dword	1
	.dword	__compound_literal.8
	.dword	1
	.dword	1
	.dword	__compound_literal.9
	.dword	1
	.dword	__compound_literal.10
	.dword	1
	.dword	__compound_literal.11
	.zero	504
	.dword	201326592
	.align	3
	.type	__compound_literal.12, @object
	.size	__compound_literal.12, 40
__compound_literal.12:
	.dword	2097152
	.zero	32
	.ident	"GCC: (g2ee5e430018) 12.2.0"
