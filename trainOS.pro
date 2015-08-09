TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/console.c \
    src/init.c \
    src/interrupts.c \
    src/malloc.c \
    src/pmm.c \
    src/stdlib.c \
    src/vmm.c \
    trainscript/tsvm.cpp \
    trainscript/main.cpp

HEADERS += \
    src/console.h \
    src/cpustate.h \
    src/interrupts.h \
    src/intr_stubs.h \
    src/io.h \
    src/kernel.h \
    src/multiboot.h \
    src/pmm.h \
    src/stdlib.h \
    src/varargs.h \
    src/vmm.h \
	trainscript/common.h \
    trainscript/tsvm.hpp

DISTFILES += \
    asm/intr_common_handler.S \
    asm/multiboot.S \
    asm/start.S \
    trainscript.md \
    Makefile \
	trainscript/trainscript.l \
    trainscript/file01.ts \
	trainscript/Makefile \
	trainscript/trainscript.y

QMAKE_INCDIR =

QMAKE_CFLAGS = -m32 -Dnullptr=0 -std=c11 -Wall -fno-stack-protector -ffreestanding

QMAKE_LINK = ld
QMAKE_LFLAGS = -g -melf_i386 -Tkernel.ld
