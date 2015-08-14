SRCS = $(shell find . -regextype posix-egrep -regex '.*\.[cS]') src/cplusplus.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
CXX = g++
LD = g++
LEX=flex
YACC=bison

# define nullptr, but not NULL.
CFLAGS = -m32
ASFLAGS =
CCFLAGS = -g -std=c11 -Wall -Dnullptr=0 -g -fno-stack-protector -ffreestanding -Iinclude
CXXFLAGS = -g -std=c++11 -Wall -g -fno-stack-protector -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wall -Wextra -ffreestanding -Wno-unused-function -Iinclude
LDFLAGS = -g -m32 -nostdlib -fno-builtin -Tkernel.ld

all: clean kernel

kernel: $(OBJS) obj/tsvm.o obj/lex.yy.o obj/trainscript.tab.o obj/vm.o
	$(LD) $(LDFLAGS) -o $@ $(addprefix obj/, $(notdir $^))

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

%.o: %.S
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

obj/vm.o: src/vm.cpp trainscript/tsvm.hpp
	g++ $(CFLAGS) $(CXXFLAGS) -c src/vm.cpp -o obj/vm.o

obj/tsvm.o: trainscript/tsvm.cpp trainscript/tsvm.hpp
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/tsvm.cpp -o obj/tsvm.o

obj/lex.yy.o: trainscript/lex.yy.cpp trainscript/tsvm.hpp
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/lex.yy.cpp -o obj/lex.yy.o

obj/trainscript.tab.o: trainscript/trainscript.tab.cpp trainscript/tsvm.hpp
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/trainscript.tab.cpp -o obj/trainscript.tab.o

trainscript/lex.yy.cpp: trainscript/trainscript.l
	$(LEX) --header-file=trainscript/trainscript.l.h -o trainscript/lex.yy.cpp trainscript/trainscript.l

trainscript/trainscript.tab.cpp: trainscript/trainscript.y
	$(YACC) -o trainscript/trainscript.tab.cpp -d trainscript/trainscript.y

obj/file01.o:
	objcopy -I binary -O elf32-i386 --redefine-sym _binary_trainscript_file01_ts_start=file01_start --redefine-sym _binary_trainscript_file01_ts_end=file01_end --redefine-sym _binary_trainscript_file01_ts_size=file01_size trainscript/file01.ts obj/file01.o

clean:
	rm obj/*.o

run:
	qemu-system-i386 -kernel kernel

.PHONY: clean
.PHONY: run
