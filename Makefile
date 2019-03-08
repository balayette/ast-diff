CXX ?= clang++
CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic -g -MMD
LINK.o = $(LINK.cc)

SRC = \
	src/lexer.cc \
	src/ast-diffing.cc \
	src/token.cc \
	src/parser.cc \
	src/tree.cc \
	src/symbol.cc \
	src/heap.cc \
	src/algo.cc \
	src/mappings.cc \

OBJ = $(SRC:.cc=.o)

DEP = $(OBJ:.o=.d)

opti: CXXFLAGS += -O2
opti: all

all: ast-diffing

debug: CXXFLAGS += -Og
debug: all

src/ast-diffing: $(OBJ)

ast-diffing: src/ast-diffing
	cp src/ast-diffing ast-diffing

clean:
	$(RM) $(OBJ) src/ast-diffing
	$(RM) $(DEP)
	$(RM) ast-diffing

-include $(DEP)
