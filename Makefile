SRCS = $(shell find . -regextype posix-egrep -regex '.*\.[cS]') src/cplusplus.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
CXX = g++
LD = ld
LEX=flex
YACC=bison

# define nullptr, but not NULL.
CFLAGS = -m32 -Dnullptr=0
ASFLAGS =
CCFLAGS = -g -std=c11 -Wall -g -fno-stack-protector -ffreestanding -Iinclude
CXXFLAGS = -g -std=c++11 -Wall -g -fno-stack-protector -fno-exceptions -ffreestanding -Wno-unused-function -Iinclude
LDFLAGS = -g -melf_i386 -Tkernel.ld

kernel: $(OBJS) obj/tsvm.o obj/lex.yy.o obj/trainscript.tab.o
	$(LD) $(LDFLAGS) -o $@ $(addprefix obj/, $(notdir $^))

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

%.o: %.S
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

obj/tsvm.o: trainscript/tsvm.cpp trainscript/tsvm.hpp trainscript/common.h
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/tsvm.cpp -o obj/tsvm.o

obj/lex.yy.o: trainscript/lex.yy.cpp trainscript/tsvm.hpp trainscript/common.h trainscript/trainscript.tab.cpp
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/lex.yy.cpp -o obj/lex.yy.o

trainscript.tab.o: trainscript/lex.yy.cpp trainscript/trainscript.tab.cpp trainscript/tsvm.hpp trainscript/common.h
	g++ $(CFLAGS) $(CXXFLAGS) -c trainscript/trainscript.tab.cpp -o obj/trainscript.tab.o

trainscript/lex.yy.cpp: trainscript/trainscript.l
	$(LEX) --header-file=trainscript/trainscript.l.h -o trainscript/lex.yy.cpp trainscript/trainscript.l

trainscript/trainscript.tab.cpp: trainscript/trainscript.y
	$(YACC) -o trainscript/trainscript.tab.cpp -d trainscript/trainscript.y

clean:
	rm $(addprefix obj/, $(notdir $(OBJS)))

run:
	qemu-system-i386 -kernel kernel

.PHONY: clean
.PHONY: run
