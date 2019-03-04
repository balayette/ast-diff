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

OBJ = $(SRC:.cc=.o)

DEP = $(OBJ:.o=.d)

all: CXXFLAGS += -O2
all: ast-diffing

debug: CXXFLAGS += -Og -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: CPPFLAGS += -D_GLIBCXX_DEBUG
debug: all

src/ast-diffing: $(OBJ)

ast-diffing: src/ast-diffing
	cp src/ast-diffing ast-diffing

clean:
	$(RM) $(OBJ) src/ast-diffing
	$(RM) $(DEP)
	$(RM) ast-diffing

-include $(DEP)
