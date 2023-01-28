LIB := lib
KERNEL_LIB := $(LIB)/kernel
USER_LIB := $(LIB)/user

LAB := l1

KERNEL_OBJS := \
  $(KERNEL_LIB)/entry.o \
  $(KERNEL_LIB)/start.o \
  $(KERNEL_LIB)/console.o \
  $(KERNEL_LIB)/printf.o \
  $(KERNEL_LIB)/uart.o \
  $(KERNEL_LIB)/kalloc.o \
  $(KERNEL_LIB)/spinlock.o \
  $(KERNEL_LIB)/string.o \
  $(KERNEL_LIB)/main.o \
  $(KERNEL_LIB)/vm.o \
  $(KERNEL_LIB)/proc.o \
  $(KERNEL_LIB)/swtch.o \
  $(KERNEL_LIB)/trampoline.o \
  $(KERNEL_LIB)/trap.o \
  $(KERNEL_LIB)/syscall.o \
  $(KERNEL_LIB)/sysproc.o \
  $(KERNEL_LIB)/bio.o \
  $(KERNEL_LIB)/fs.o \
  $(KERNEL_LIB)/log.o \
  $(KERNEL_LIB)/sleeplock.o \
  $(KERNEL_LIB)/file.o \
  $(KERNEL_LIB)/pipe.o \
  $(KERNEL_LIB)/exec.o \
  $(KERNEL_LIB)/sysfile.o \
  $(KERNEL_LIB)/kernelvec.o \
  $(KERNEL_LIB)/plic.o \
  $(KERNEL_LIB)/virtio_disk.o \
  obj/ps/sys_getprocs.o

USER_OBJS := \
	$(USER_LIB)/ulib.o \
	$(USER_LIB)/usys.o \
	$(USER_LIB)/printf.o \
	$(USER_LIB)/umalloc.o

USER_PROGRAMS := \
	$(USER_LIB)/_cat \
	$(USER_LIB)/_echo \
	$(USER_LIB)/_forktest \
	$(USER_LIB)/_grep \
	$(USER_LIB)/_init \
	$(USER_LIB)/_kill \
	$(USER_LIB)/_ln \
	$(USER_LIB)/_ls \
	$(USER_LIB)/_mkdir \
	$(USER_LIB)/_rm \
	$(USER_LIB)/_sh \
	$(USER_LIB)/_stressfs \
	$(USER_LIB)/_usertests \
	$(USER_LIB)/_grind \
	$(USER_LIB)/_wc \
	$(USER_LIB)/_zombie \
	$(USER_LIB)/_hello \
	$(USER_LIB)/_ps

.DEFAULT_GOAL := $(KERNEL_LIB)/kernel

.PHONY: clean qemu test print-gdbport prepare-handin internal-prepare-handout

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell \
	if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find a riscv64 version of GCC/binutils." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

QEMU := qemu-system-riscv64
CC := $(TOOLPREFIX)gcc
AS := $(TOOLPREFIX)gas
LD := $(TOOLPREFIX)ld
OBJCOPY := $(TOOLPREFIX)objcopy
OBJDUMP := $(TOOLPREFIX)objdump

CFLAGS := -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2 -MD -mcmodel=medany \
	-ffreestanding -fno-common -nostdlib -mno-relax -I. \
	$(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LDFLAGS := -z max-page-size=4096

-include $(KERNEL_LIB)/*.d $(USER_LIB)/*.d

_%: %.o $(USER_OBJS)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

HELLO_OBJS := \
	obj/hello/main.o \
	obj/hello/greetings.o

$(USER_LIB)/_hello: $(HELLO_OBJS) $(USER_OBJS)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_LIB)/hello.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_LIB)/hello.sym

PS_OBJS := \
	obj/ps/main.o \
	obj/ps/process_list.o \
	obj/utils/realloc.o

$(USER_LIB)/_ps: $(PS_OBJS) $(USER_OBJS)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_LIB)/ps.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_LIB)/ps.sym

obj/%.o: src/%.c $(wildcard src/*.h)
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(USER_LIB)/usys.S : $(USER_LIB)/usys.pl
	perl $(USER_LIB)/usys.pl > $(USER_LIB)/usys.S

$(USER_LIB)/usys.o : $(USER_LIB)/usys.S
	$(CC) $(CFLAGS) -c -o $(USER_LIB)/usys.o $(USER_LIB)/usys.S

$(USER_LIB)/_forktest: $(USER_LIB)/forktest.o $(USER_OBJS)
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $(USER_LIB)/_forktest $(USER_LIB)/forktest.o $(USER_LIB)/ulib.o $(USER_LIB)/usys.o
	$(OBJDUMP) -S $(USER_LIB)/_forktest > $(USER_LIB)/forktest.asm

$(LIB)/mkfs/mkfs: $(LIB)/mkfs/mkfs.c $(KERNEL_LIB)/fs.h $(KERNEL_LIB)/param.h
	gcc -Werror -Wall -I. -o $(LIB)/mkfs/mkfs $(LIB)/mkfs/mkfs.c

$(LIB)/fs.img: $(LIB)/mkfs/mkfs $(LIB)/README $(USER_PROGRAMS)
	$(LIB)/mkfs/mkfs $(LIB)/fs.img $(LIB)/README $(USER_PROGRAMS)

$(KERNEL_LIB)/kernel: $(KERNEL_OBJS) $(KERNEL_LIB)/kernel.ld $(USER_LIB)/initcode
	$(LD) $(LDFLAGS) -T $(KERNEL_LIB)/kernel.ld -o $(KERNEL_LIB)/kernel $(KERNEL_OBJS) 
	$(OBJDUMP) -S $(KERNEL_LIB)/kernel > $(KERNEL_LIB)/kernel.asm
	$(OBJDUMP) -t $(KERNEL_LIB)/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(KERNEL_LIB)/kernel.sym

$(USER_LIB)/initcode: $(USER_LIB)/initcode.S
	$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c $(USER_LIB)/initcode.S -o $(USER_LIB)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(USER_LIB)/initcode.out $(USER_LIB)/initcode.o
	$(OBJCOPY) -S -O binary $(USER_LIB)/initcode.out $(USER_LIB)/initcode
	$(OBJDUMP) -S $(USER_LIB)/initcode.o > $(USER_LIB)/initcode.asm

tags: $(KERNEL_OBJS) _init
	etags *.S *.c

clean: 
	rm -f \
		*.ans \
		*.out* \
		$(LIB)/*.tex \
		$(LIB)/*.dvi \
		$(LIB)/*.idx \
		$(LIB)/*.aux \
		$(LIB)/*.log \
		$(LIB)/*.ind \
		$(LIB)/*.ilg \
		$(LIB)/*/*.o \
		$(LIB)/*/*.d \
		$(LIB)/*/*.asm \
		$(LIB)/*/*.sym \
		$(LIB)/*.tar.gz \
		$(LIB)/fs.img \
		$(LIB)/.gdbinit \
		$(LIB)/mkfs/mkfs \
		$(KERNEL_LIB)/kernel \
		$(USER_LIB)/initcode \
		$(USER_LIB)/initcode.out \
        $(USER_LIB)/usys.S \
		$(USER_PROGRAMS)

	rm -rf \
		obj \
		bin \
		$(LIB)/__pycache__

# try to generate a unique GDB port
GDBPORT := $(shell expr `id -u` % 5000 + 25000)

# QEMU's gdb stub command line changed in 0.11
QEMUGDB := $(shell \
	if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

ifndef CPUS
CPUS := 3
endif

QEMUOPTS := -machine virt -bios none -kernel $(KERNEL_LIB)/kernel -m 128M -smp $(CPUS) -nographic \
	-global virtio-mmio.force-legacy=false \
	-drive file=lib/fs.img,if=none,format=raw,id=x0 \
	-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: $(KERNEL_LIB)/kernel $(LIB)/fs.img
	$(QEMU) $(QEMUOPTS)

$(LIB)/.gdbinit: $(LIB)/.gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

qemu-gdb: $(KERNEL_LIB)/kernel $(LIB)/.gdbinit $(LIB)/fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

##
##  For testing lab grading script
##

GRADEFLAGS += -v --color always

print-gdbport:
	@echo $(GDBPORT)

test:
	@echo $(MAKE) clean
	@$(MAKE) clean || \
          (echo "'make clean' failed.  HINT: Do you have another running instance of xv6?" && exit 1)
	./$(LIB)/test-lab-$(LAB) $(GRADEFLAGS) 2>&1 | tee ./test.ans

prepare-handin: test
	tar -zcvf lab-$(LAB)-handin.tar.gz .

internal-prepare-handout:
	tar -zcvf lab-$(LAB)-handout.tar.gz .
