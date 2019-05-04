CXX ?= clang++
CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic -g -MMD
LINK.o = $(LINK.cc)

LIBOBJ = \
	src/lexer.o \
	src/token.o \
	src/parser.o \
	src/tree.o \
	src/symbol.o \
	src/heap.o \
	src/algo.o \
	src/mappings.o \
	src/cache.o \

DIFFOBJ = \
	src/ast-diff.o \

DISPATCHOBJ = \
	src/dispatch.o \

EXEOBJ = $(DIFFOBJ) $(DISPATCHOBJ)

OBJ = $(LIBOBJ) $(EXEOBJ)

DEP = $(OBJ:.o=.d)

LIB = libast-diff.a
DIFFEXE = ast-diff
DISPATCHEXE = dispatch
EXE = $(DIFFEXE) $(DISPATCHEXE)

opti: CXXFLAGS += -O2
opti: all

debug: CXXFLAGS += -Og
debug: all

all: $(LIB) $(EXE)

$(LIB): $(LIBOBJ)
	$(AR) rcs $(LIB) $(LIBOBJ)

src/ast-diff: $(LIB) $(DIFFOBJ)

src/dispatch: LDFLAGS += -lpthread
src/dispatch: $(LIB) $(DISPATCHOBJ)

$(DIFFEXE): src/ast-diff
	cp src/ast-diff $(DIFFEXE)

$(DISPATCHEXE): src/dispatch
	cp src/dispatch $(DISPATCHEXE)

clean:
	$(RM) $(OBJ) $(EXE) $(LIB) $(DEP) src/ast-diff src/dispatch

-include $(DEP)
