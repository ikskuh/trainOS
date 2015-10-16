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
    src/timer.c \
    src/cplusplus.cpp \
    src/vm.cpp \
	src/serial.c \
    csl/cpustatetype.cpp \
    csl/io.cpp

HEADERS += \
	include/console.h \
	include/cpustate.h \
	include/interrupts.h \
	include/intr_stubs.h \
	include/io.h \
	include/kernel.h \
	include/multiboot.h \
	include/pmm.h \
	include/kstdlib.h \
	include/varargs.h \
	include/vmm.h \
	include/timer.h \
    include/ker/string.hpp \
    include/ker/pair.hpp \
    include/ker/vector.hpp \
    include/ker/dictionary.hpp \
	include/kstring.h \
    include/ker/new.hpp \
    include/dynamic.h \
    include/config.h \
    include/serial.h \
	include/malloc.h \
    csl/cpustatetype.hpp \
    csl/io.hpp \
    include/ker/config.hpp

DISTFILES += \
    asm/intr_common_handler.S \
    asm/multiboot.S \
    asm/start.S \
    trainscript.md \
	Makefile \
    kernel.ld \
    Depfile \
    Makefile.new \
    asm/dynamic.S \
	README.md \
    scripts/main.spark \
    scripts/main.cu \
    scripts/keyboard.cu

INCLUDEPATH += include
DEPENDPATH += include

INCLUDEPATH += $$quote("/home/felix/projects/Electronics/Electronics/Conductance")
DEPENDPATH += $$quote("/home/felix/projects/Electronics/Electronics/Conductance")


QMAKE_CFLAGS = -m32 -Dnullptr=0 -std=c11 -Wall -fno-stack-protector -ffreestanding

QMAKE_LINK = ld
QMAKE_LFLAGS = -g -melf_i386 -Tkernel.ld
