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
    trainscript/main.cpp \
    src/timer.c \
    src/cplusplus.cpp \
    src/vm.cpp

HEADERS += \
	include/console.h \
	include/cpustate.h \
	include/interrupts.h \
	include/intr_stubs.h \
	include/io.h \
	include/kernel.h \
	include/multiboot.h \
	include/pmm.h \
	include/stdlib.h \
	include/varargs.h \
	include/vmm.h \
	trainscript/common.h \
    trainscript/tsvm.hpp \
    trainscript/typeid.hpp \
	include/timer.h \
    include/ker/string.hpp \
    include/ker/pair.hpp \
    include/ker/vector.hpp \
    include/ker/dictionary.hpp \
    include/string.h \
    include/ker/new.hpp

DISTFILES += \
    asm/intr_common_handler.S \
    asm/multiboot.S \
    asm/start.S \
    trainscript.md \
    Makefile \
	trainscript/trainscript.l \
    trainscript/file01.ts \
	trainscript/Makefile \
	trainscript/trainscript.y \
    trainscript/file02.ts \
    kernel.ld \
    Depfile \
    Makefile.new

QMAKE_INCDIR =

QMAKE_CFLAGS = -m32 -Dnullptr=0 -std=c11 -Wall -fno-stack-protector -ffreestanding

QMAKE_LINK = ld
QMAKE_LFLAGS = -g -melf_i386 -Tkernel.ld
