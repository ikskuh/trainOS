# -fsanitize=address
CFLAGS=-g
CCFLAGS=$(CFLAGS) -std=c++11 -c
LFLAGS=$(CFLAGS) -o tsvm

LEX=flex
YACC=bison

all: trainscript.tab.o lex.yy.o tsvm.o main.o
	g++ $(LFLAGS) lex.yy.o trainscript.tab.o tsvm.o main.o

main.o: main.cpp tsvm.hpp common.h
	g++ $(CCFLAGS) -c main.cpp -o main.o

tsvm.o: tsvm.cpp tsvm.hpp common.h
	g++ $(CCFLAGS) -c tsvm.cpp -o tsvm.o

lex.yy.o: lexyy.cpp tsvm.hpp common.h
	g++ $(CCFLAGS) -c lex.yy.cpp -o lex.yy.o

trainscript.tab.o: lex.yy.cpp trainscript_tab.cpp tsvm.hpp common.h
	g++ $(CCFLAGS) -c trainscript.tab.cpp -o trainscript.tab.o

lex.yy.cpp: trainscript.l
	$(LEX) --header-file=trainscript.l.h trainscript.l
	mv lex.yy.c lex.yy.cpp
	
trainscript.tab.cpp: trainscript.y
	$(YACC) -d trainscript.y
	mv trainscript.tab.c trainscript.tab.cpp

.PHONY: clean
clean:
	rm *.o trainscript.tab.cpp trainscript.l.h lex.yy.cpp
