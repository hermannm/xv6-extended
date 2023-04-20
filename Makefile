SRC := src

LIB := lib
KERNEL_LIB := $(LIB)/kernel
USER_LIB := $(LIB)/user

OBJ := obj
KERNEL_OBJ := $(OBJ)/kernel
USER_OBJ := $(OBJ)/user

BIN := bin
KERNEL_BIN := $(BIN)/kernel
USER_BIN := $(BIN)/user

KERNEL_OBJS := \
  $(KERNEL_OBJ)/entry.o \
  $(KERNEL_OBJ)/start.o \
  $(KERNEL_OBJ)/console.o \
  $(KERNEL_OBJ)/printf.o \
  $(KERNEL_OBJ)/uart.o \
  $(KERNEL_OBJ)/kalloc.o \
  $(KERNEL_OBJ)/spinlock.o \
  $(KERNEL_OBJ)/string.o \
  $(KERNEL_OBJ)/main.o \
  $(KERNEL_OBJ)/vm.o \
  $(KERNEL_OBJ)/proc.o \
  $(KERNEL_OBJ)/swtch.o \
  $(KERNEL_OBJ)/trampoline.o \
  $(KERNEL_OBJ)/trap.o \
  $(KERNEL_OBJ)/syscall.o \
  $(KERNEL_OBJ)/sysproc.o \
  $(KERNEL_OBJ)/bio.o \
  $(KERNEL_OBJ)/fs.o \
  $(KERNEL_OBJ)/log.o \
  $(KERNEL_OBJ)/sleeplock.o \
  $(KERNEL_OBJ)/file.o \
  $(KERNEL_OBJ)/pipe.o \
  $(KERNEL_OBJ)/exec.o \
  $(KERNEL_OBJ)/sysfile.o \
  $(KERNEL_OBJ)/kernelvec.o \
  $(KERNEL_OBJ)/plic.o \
  $(KERNEL_OBJ)/virtio_disk.o \
  $(OBJ)/processes/sys_getprocs.o \
  $(OBJ)/scheduling/mlfq_scheduler.o \
  $(OBJ)/memory/sys_vatopa.o \
  $(OBJ)/memory/page_reference_count.o \
  $(OBJ)/memory/copy_on_write_handler.o

USER_OBJS := \
	$(USER_OBJ)/ulib.o \
	$(USER_OBJ)/usys.o \
	$(USER_OBJ)/printf.o \
	$(USER_OBJ)/umalloc.o \
	$(USER_OBJ)/lock.o \
	$(USER_OBJ)/tswtch.o \
	$(USER_OBJ)/uthread.o

USER_PROGRAMS := \
	$(USER_BIN)/_cat \
	$(USER_BIN)/_congen \
	$(USER_BIN)/_echo \
	$(USER_BIN)/_forktest \
	$(USER_BIN)/_grep \
	$(USER_BIN)/_init \
	$(USER_BIN)/_kill \
	$(USER_BIN)/_ln \
	$(USER_BIN)/_ls \
	$(USER_BIN)/_mkdir \
	$(USER_BIN)/_rm \
	$(USER_BIN)/_schedls \
	$(USER_BIN)/_schedset \
	$(USER_BIN)/_sh \
	$(USER_BIN)/_stressfs \
	$(USER_BIN)/_task1test\
	$(USER_BIN)/_time \
	$(USER_BIN)/_ttest\
	$(USER_BIN)/_usertests \
	$(USER_BIN)/_grind \
	$(USER_BIN)/_wc \
	$(USER_BIN)/_zombie \
	$(USER_BIN)/_cowtest \
	$(USER_BIN)/_hello \
	$(USER_BIN)/_ps \
	$(USER_BIN)/_proctree \
	$(USER_BIN)/_vatopa

SCRIPTS := \
	$(USER_LIB)/load.sh

.DEFAULT_GOAL := $(KERNEL_BIN)/kernel

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

$(USER_BIN)/_%: $(USER_OBJ)/%.o $(USER_OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_BIN)/$*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_BIN)/$*.sym

HELLO_OBJS := \
	$(OBJ)/hello/main.o

$(USER_BIN)/_hello: $(HELLO_OBJS) $(USER_OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_BIN)/hello.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_BIN)/hello.sym

PS_OBJS := \
	$(OBJ)/processes/ps.o \
	$(OBJ)/processes/process_list.o \
	$(OBJ)/utils/realloc.o

$(USER_BIN)/_ps: $(PS_OBJS) $(USER_OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_BIN)/ps.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_BIN)/ps.sym

PROCTREE_OBJS := \
	$(OBJ)/processes/proctree.o \
	$(OBJ)/processes/process_list.o \
	$(OBJ)/utils/realloc.o \
	$(OBJ)/utils/int_list.o \
	$(OBJ)/utils/strings.o

$(USER_BIN)/_proctree: $(PROCTREE_OBJS) $(USER_OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_BIN)/proctree.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_BIN)/proctree.sym

VATOPA_OBJS := \
	$(OBJ)/memory/vatopa.o \
	$(OBJ)/utils/strings.o

$(USER_BIN)/_vatopa: $(VATOPA_OBJS) $(USER_OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(USER_LIB)/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $(USER_BIN)/vatopa.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(USER_BIN)/vatopa.sym

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(USER_OBJ)/%.o: $(USER_LIB)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(USER_OBJ)/%.o: $(USER_LIB)/%.S
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(KERNEL_OBJ)/%.o: $(KERNEL_LIB)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(KERNEL_OBJ)/%.o: $(KERNEL_LIB)/%.S
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(USER_LIB)/usys.S: $(USER_LIB)/usys.pl
	perl $(USER_LIB)/usys.pl > $(USER_LIB)/usys.S

$(USER_OBJ)/usys.o: $(USER_LIB)/usys.S
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(USER_BIN)/_forktest: $(USER_OBJ)/forktest.o $(USER_OBJS)
# forktest has less library code linked in - needs to be small in order to be able to max out the
# proc table.
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $< $(USER_OBJ)/ulib.o $(USER_OBJ)/usys.o
	$(OBJDUMP) -S $@ > $(USER_BIN)/forktest.asm

$(BIN)/mkfs/mkfs: $(LIB)/mkfs/mkfs.c $(KERNEL_LIB)/fs.h $(KERNEL_LIB)/param.h
	@mkdir -p $(@D)
	gcc -Werror -Wall -I. -o $@ $<

$(BIN)/fs.img: $(BIN)/mkfs/mkfs $(LIB)/README $(USER_PROGRAMS) $(SCRIPTS)
	@mkdir -p $(@D)
	$(BIN)/mkfs/mkfs $(BIN)/fs.img $(LIB)/README $(USER_PROGRAMS) $(SCRIPTS)

$(KERNEL_BIN)/kernel: $(KERNEL_OBJS) $(KERNEL_LIB)/kernel.ld $(USER_BIN)/initcode
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -T $(KERNEL_LIB)/kernel.ld -o $@ $(KERNEL_OBJS) 
	$(OBJDUMP) -S $(KERNEL_BIN)/kernel > $(KERNEL_BIN)/kernel.asm
	$(OBJDUMP) -t $(KERNEL_BIN)/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(KERNEL_BIN)/kernel.sym

$(USER_BIN)/initcode: $(USER_LIB)/initcode.S
	@mkdir -p $(@D)
	@mkdir -p $(USER_OBJ)
	$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c $< -o $(USER_OBJ)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(USER_BIN)/initcode.out $(USER_OBJ)/initcode.o
	$(OBJCOPY) -S -O binary $(USER_BIN)/initcode.out $@
	$(OBJDUMP) -S $(USER_OBJ)/initcode.o > $(USER_BIN)/initcode.asm

tags: $(KERNEL_OBJS) _init
	etags *.S *.c

clean: 
	rm -f \
		*.ans \
		*.out* \
		*.tar.gz \
		$(LIB)/*.tex \
		$(LIB)/*.dvi \
		$(LIB)/*.idx \
		$(LIB)/*.aux \
		$(LIB)/*.log \
		$(LIB)/*.ind \
		$(LIB)/*.ilg \
		$(LIB)/.gdbinit \
		$(USER_LIB)/usys.S \

	rm -rf \
		$(OBJ) \
		$(BIN) \
		$(LIB)/__pycache__

# Try to generate a unique GDB port
GDBPORT := $(shell expr `id -u` % 5000 + 25000)

# QEMU's gdb stub command line changed in 0.11
QEMUGDB := $(shell \
	if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

ifndef CPUS
CPUS := 3
endif

QEMUOPTS := -machine virt -bios none -kernel $(KERNEL_BIN)/kernel -m 128M -smp $(CPUS) -nographic \
	-global virtio-mmio.force-legacy=false \
	-drive file=$(BIN)/fs.img,if=none,format=raw,id=x0 \
	-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: $(KERNEL_BIN)/kernel $(BIN)/fs.img
	$(QEMU) $(QEMUOPTS)

$(LIB)/.gdbinit: $(LIB)/.gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

qemu-gdb: $(KERNEL_BIN)/kernel $(LIB)/.gdbinit $(BIN)/fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

##
## For testing lab grading script
##

GRADEFLAGS += -v --color always

print-gdbport:
	@echo $(GDBPORT)

LAB := l4

test:
	@echo $(MAKE) clean
	@$(MAKE) clean || \
          (echo "'make clean' failed.  HINT: Do you have another running instance of xv6?" && exit 1)
	./$(LIB)/test-lab-$(LAB) $(GRADEFLAGS) 2>&1 | tee ./test.ans

prepare-handin: test
# Fixes tar error "file changed as we read it"
# https://stackoverflow.com/questions/20318852/tar-file-changed-as-we-read-it
	@touch lab-$(LAB)-handin.tar.gz

	tar --exclude=lab-$(LAB)-handin.tar.gz -zcvf lab-$(LAB)-handin.tar.gz .
