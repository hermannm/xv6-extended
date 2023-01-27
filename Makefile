# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
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

USER_LIB := \
  lib/user/ulib.o \
  lib/user/usys.o \
  lib/user/printf.o \
  lib/user/umalloc.o

.PHONY: clean

HELLO__OBJS := \
	obj/hello-world/main.o \
	obj/hello-world/greetings.o

bin/hello: ${HELLO_OBJS}
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

PS_OBJS := \
	obj/ps/main.o \
	obj/ps/process_list.o \
	obj/utils/realloc.o

lib/user/_ps: ${PS_OBJS} ${USER_LIB}
	$(LD) $(LDFLAGS) -T lib/user/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > lib/user/ps.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > lib/user/ps.sym

obj/ps/sys_getprocs.o: src/ps/sys_getprocs.c src/ps/sys_getprocs.h
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

obj/%.o: src/%.c $(wildcard src/*.h)
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf obj bin
