#
# Top-level Makefile for certikos64
#

ifndef V
V		:= @
else
V		:=
endif

ARCH		:= i386

#
# Which lab experiment you are currently working on (1 - 7), or 8 for the
# course project (built on top of the completed experiments).
# All seven labs live in this single project; each lab builds on the code you
# wrote in the previous ones. Either change the default below as you move on
# to the next lab, or override it on the command line, e.g.
#     make LAB=2 TEST=1
#
LAB		?= 1

ifeq ($(filter $(LAB),1 2 3 4 5 6 7 8),)
$(error LAB must be one of 1 to 7, or 8 for the project (got "$(LAB)"))
endif

COMP_NAME	:= "ccomp"

ENABLE_CCOMP	:= 0
COMP_NAME	:= "cc"

# Cross-compiler toolchain
#
# This Makefile will automatically use a cross-compiler toolchain installed
# as 'pios-*' or 'i386-elf-*', if one exists. If the host tools ('gcc',
# 'objdump', and so forth) compile for a 32-bit x86 ELF target, that will
# be detected as well. If you have the right compiler toolchain installed
# using a different name, set GCCPREFIX explicitly in conf/env.mk

# try to infer the correct GCCPREFIX
ifndef GCCPREFIX
GCCPREFIX	:= $(shell sh misc/gccprefix.sh)
endif

# Directories
TOP		:= .
SRCDIR		:= $(TOP)
OBJDIR		:= $(TOP)/obj

# LAB and TEST are compile-time switches, so rebuild everything from scratch
# whenever either of them changes since the last build.
BUILD_CONFIG	:= LAB=$(LAB) TEST=$(TEST)
ifeq ($(filter-out all,$(MAKECMDGOALS)),)
ifneq ($(shell cat $(OBJDIR)/.config 2>/dev/null),$(BUILD_CONFIG))
$(shell rm -rf $(OBJDIR); mkdir -p $(OBJDIR); echo "$(BUILD_CONFIG)" > $(OBJDIR)/.config)
endif
endif
UTILSDIR	:= $(TOP)/misc
TESTDIR		:= $(TOP)/test
OBJDIRS		:=

# Compiler and Linker
CC		:= $(GCCPREFIX)gcc
LD		:= $(GCCPREFIX)ld
CFLAGS		:= -std=gnu11 -Wall -MD -Wno-strict-aliasing -Wno-unused-function -pipe -fno-builtin -nostdinc -fno-stack-protector
# GCC 12+ treats fixed hardware addresses (e.g. the VGA buffer) as zero-size
# arrays and warns about every access; tell it pages start at address 0.
CFLAGS		+= $(shell $(CC) --param=min-pagesize=0 -E -x c /dev/null >/dev/null 2>&1 && echo --param=min-pagesize=0)
LDFLAGS		:= -nostdlib -z noexecstack
# Newer linkers (binutils >= 2.39) warn about the RWX boot segments; that is
# expected for a boot loader, so silence it where the option exists.
LDFLAGS		+= $(shell $(LD) --help 2>/dev/null | grep -q no-warn-rwx-segments && echo --no-warn-rwx-segments)

ifeq (ENABLE_CCOMP, 1)
CCOMP		:= ccomp
CCOMP_CFLAGS	:= -finline-asm -fpacked-structs -fno-sse

CLIGHTGEN	:= clightgen
CLIGHTGEN_FLAGS	:= -fall

ifeq ($(V),)
CCOMP_CFLAGS	+= -v
endif

else
# Uncomment following two lines when you suspect differences between gcc and
# compcert cause problems.

CCOMP		:= $(GCCPREFIX)gcc
CCOMP_CFLAGS	:= -std=gnu11 -MD -Wno-strict-aliasing -Wno-unused-function -pipe -fno-builtin -nostdinc -fno-stack-protector -O2 -g -m32 -D__COMPCERT__
endif

# other tools
PERL		:= perl
OBJDUMP		:= $(GCCPREFIX)objdump
OBJCOPY		:= $(GCCPREFIX)objcopy
DD		:= $(GCCPREFIX)dd
NM		:= $(GCCPREFIX)nm
CSCOPE		:= cscope
# gdb: the first one installed of the toolchain gdb, i386-elf-gdb (macOS
# Homebrew), gdb-multiarch or the plain host gdb.
GDB		?= $(firstword $(foreach g,$(GCCPREFIX)gdb i386-elf-gdb i686-elf-gdb gdb-multiarch gdb,$(if $(shell command -v $(g) 2>/dev/null),$(g))) gdb)

# others
GCC_LIB32	:= $(shell $(CC) $(CFLAGS) -m32 -print-libgcc-file-name)
ifeq ($(ARCH), amd64)
GCC_LIB64	:= $(shell $(CC) $(CFLAGS) -m64 -print-libgcc-file-name)
endif

# If this is the first time building CertiKOS64, please follow the instructions
# in HOW_TO_MAKE_DISK_IMAGE to create a disk image file manually and put it in
# directory $(OBJDIR)/ (default: obj/)
CERTIKOS_IMG	:= certikos.img

# try to generate a unique GDB port
GDBPORT		:= $(shell expr `id -u` % 5000 + 25000)

# qemu
QEMU		:= qemu-system-x86_64
QEMUOPTS	:= -smp 4 -drive id=disk0,file=$(CERTIKOS_IMG),format=raw,if=ide -drive id=disk1,file=certikos_disk.img,format=raw,if=ide -serial mon:stdio -gdb tcp::$(GDBPORT) -m 2048 -k en-us -no-reboot
QEMUOPTS_TCG	:= -icount shift=auto
# Use hardware virtualization (KVM) when it is available; it is much faster.
ifneq ($(wildcard /dev/kvm),)
QEMUOPTS_TCG	:= -enable-kvm -cpu host
endif
QEMUOPTS_KVM	:= -cpu host -enable-kvm
QEMUOPTS_BIOS	:= -L $(UTILSDIR)/qemu/

# Targets

.PHONY: all boot kern user deps qemu qemu-nox qemu-gdb mkfs

all: boot kern user link
	@./make_image.py
	@echo "Built for LAB=$(LAB)$(if $(TEST), (test mode),)."
ifdef TEST
	$(V)cp newfs/certikos_disk_new.img certikos_disk.img
	@echo "***"
	@echo "*** Use Ctrl-a x to exit qemu"
	@echo "***"
	$(V)$(QEMU) -nographic $(QEMUOPTS) $(QEMUOPTS_TCG)
endif
	@echo "All targets are done."

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@
	@./scripts/set_auto_load.sh

# QEMU doesn't truncate the pcap file. Work around this.
pre-qemu: .gdbinit
	@rm -f qemu.pcap
	$(V)cp newfs/certikos_disk_new.img certikos_disk.img

qemu: $(CERTIKOS_IMG) pre-qemu
	$(V)$(QEMU) $(QEMUOPTS) $(QEMUOPTS_TCG)

qemu-nox: $(CERTIKOS_IMG) pre-qemu
	@echo "***"
	@echo "*** Use Ctrl-a x to exit qemu"
	@echo "***"
	$(V)$(QEMU) -nographic $(QEMUOPTS) $(QEMUOPTS_TCG)

qemu-gdb: $(CERTIKOS_IMG) pre-qemu
	@echo "***"
	@echo "*** Now run 'make gdb' in another terminal." 1>&2
	@echo "***"
	$(V)$(QEMU) $(QEMUOPTS) $(QEMUOPTS_TCG) -S

qemu-nox-gdb: $(CERTIKOS_IMG) pre-qemu
	@echo "***"
	@echo "*** Now run 'make gdb' in another terminal." 1>&2
	@echo "***"
	$(V)$(QEMU) -nographic $(QEMUOPTS) $(QEMUOPTS_TCG) -S

qemu-kvm: $(CERTIKOS_IMG)
	$(V)$(QEMU) $(QEMUOPTS) $(QEMUOPTS_KVM)

qemu-kvm-gdb: $(CERTIKOS_IMG)
	$(V)$(QEMU) $(QEMUOPTS) $(QEMUOPTS_KVM) -S

qemu-bios: $(CERTIKOS_IMG)
	$(V)$(QEMU) $(QEMUOPTS) $(QEMUOPTS_TCG) $(QEMUOPTS_BIOS)

package:
	$(V)tar czf ../certikos.tar.gz --exclude=obj --exclude=cscope.* .

cscope:
	$(V)rm -rf cscope.*
	$(V)find . -name "*.[chsS]" > cscope.files
	$(V)cscope -bkq -i cscope.files

gdb: pre-qemu
	$(GDB)

# Sub-makefiles
include boot/Makefile.inc
include kern/Makefile.inc
include user/Makefile.inc

deps: $(OBJDIR)/.deps

$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(dir)/*.d))
	$(V)mkdir -p $(@D)
	$(V)$(PERL) $(UTILSDIR)/mergedep.pl $@ $^

-include $(OBJDIR)/.deps

clean:
	$(V)rm -rf $(OBJDIR) .gdbinit certikos.img
	$(V)find . -name "*.[v]" -delete

mkfs:
	@echo "Copying the new file system disk image..."
	$(V)cp newfs/certikos_disk_new.img certikos_disk.img
	@echo "Done."
