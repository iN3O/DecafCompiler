COMMON_DIR = ..
include_dir = $(COMMON_DIR)/include
objects = bison.tab.o lex.yy.o Driver.o
CXX = g++
CXXFLAGS = -std=c++11 -static -ldl -lc -Wno-deprecated -I$(include_dir) `llvm-config --cppflags --libs core jit native` `llvm-config --ldflags`
includes = $(include_dir)/AstStructure.hpp

main:	$(objects) 
	$(CXX)  -o problem_1 $(objects) $(CXXFLAGS)
	strip problem_1

bison.tab.hpp bison.tab.cpp:	bison.ypp
	bison -d -v bison.ypp

lex.yy.c: flex.l bison.tab.hpp
	flex flex.l



bison.tab.o:	bison.tab.cpp
Driver.o: Driver.cpp

lex.yy.o:	bison.tab.hpp lex.yy.c
	$(CXX) -lfl $(CXXFLAGS) -c bison.tab.cpp lex.yy.c


.PHONY :	clean
clean:
	-echo "Removing all object files!"
	-rm -f compiler Driver.o bison.tab.o lex.yy.o
	-echo "Removing intermediate C files!"
	-rm -f bison.tab.cpp bison.tab.hpp
	-rm -rf lex.yy.c bison.output
