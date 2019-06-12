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
	src/pool.o \

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

.PHONY: opti
opti: CXXFLAGS += -O3
opti: all

.PHONY: lto
lto: CXXFLAGS += -flto
lto: LDFLAGS += -flto
lto: opti

.PHONY: debug
debug: CXXFLAGS += -Og
debug: all

.PHONY: all
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

.PHONY: install
install: ast-diff dispatch
	cp ast-diff /usr/local/bin/ast-diff
	cp dispatch /usr/local/bin/dispatch

.PHONY: uninstall
uninstall:
	rm /usr/local/bin/ast-diff
	rm /usr/local/bin/dispatch

.PHONY: clean
clean:
	$(RM) $(OBJ) $(EXE) $(LIB) $(DEP) src/ast-diff src/dispatch

-include $(DEP)
