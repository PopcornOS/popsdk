CC=gcc
LIBCDEBUG=

POPCORNOS=mypopcornos/

ifeq ($(OS),Windows_NT)
EXE=.exe
else
EXE=
endif

define pop-c
	@# GCC/Clang path: freestanding compile + LD to raw binary
	$(CC) -nostdinc -ffreestanding -fno-exceptions -fno-stack-protector -nostdlib \
		  -fno-asynchronous-unwind-tables -fshort-wchar \
		  -mno-red-zone -c $(1) -o $(1).o 
	ld -e pop_main -nostdlib -T pop.ld $(1).o -o $(1).tmp$(EXE)
	objcopy -O binary $(1).tmp$(EXE) $(2)
	rm $(1).o $(1).tmp$(EXE)
endef

define pop-c+libc
	@# GCC/Clang path: freestanding compile + LD to raw binary
	$(CC) -I. -I include -nostdinc -ffreestanding -fno-exceptions -fno-stack-protector -nostdlib \
		  -fno-asynchronous-unwind-tables -fshort-wchar \
		  -mno-red-zone -c $(1) -o $(1).o
	ld -nostdlib -T pop.ld libc.o $(1).o -o $(1).tmp$(EXE)
	objcopy -O binary $(1).tmp$(EXE) $(2)
	rm $(1).o $(1).tmp$(EXE)
endef

default: setup libc
	$(call pop-c+libc,libctest.c,libctest.bin)

libc:
	$(CC) -I include -nostdinc -ffreestanding -fno-stack-protector -nostdlib \
	      -fno-asynchronous-unwind-tables -fshort-wchar \
	      -mno-red-zone -c libc.c -o libc.o -fno-common $(if $(LIBCDEBUG),-D_LIBC_DEBUG_ON=1,)

setup:
	curl https://raw.githubusercontent.com/PopcornOS/popcorn-os/refs/heads/master/popcorn.h -o popcorn.h
	curl https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_sprintf.h -o stb_sprintf.h