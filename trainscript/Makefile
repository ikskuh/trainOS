LEX=flex
YACC=bison

all: trainscript.tab.o lex.yy.o tsvm.o main.o
	g++ -o tsvm lex.yy.o trainscript.tab.o tsvm.o main.o

main.o: main.cpp tsvm.hpp common.h
	g++ -std=c++11 -c main.cpp -o main.o

tsvm.o: tsvm.cpp tsvm.hpp common.h
	g++ -std=c++11 -c tsvm.cpp -o tsvm.o

lex.yy.o: lex.yy.cpp tsvm.hpp common.h
	g++ -std=c++11 -c lex.yy.cpp -o lex.yy.o

trainscript.tab.o: trainscript.tab.cpp tsvm.hpp common.h
	g++ -std=c++11 -c trainscript.tab.cpp -o trainscript.tab.o

lex.yy.cpp: trainscript.l
	$(LEX) --header-file=trainscript.l.h trainscript.l
	mv lex.yy.c lex.yy.cpp
	
trainscript.tab.cpp: trainscript.y
	$(YACC) -d trainscript.y
	mv trainscript.tab.c trainscript.tab.cpp
