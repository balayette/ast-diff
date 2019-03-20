#!/usr/bin/python3

import sys
import clang
from clang.cindex import Index, CursorKind
from os import path

def str_of_tokens(tokens):
    return "".join((x.spelling for x in tokens))

def get_binary_type(cursor):
    binary_operators = {
        '=': 'BINARY_ASSIGN',
        '==': 'BINARY_EQ',
        '!=': 'BINARY_NE',
        '||': 'BINARY_OR',
        '&&': 'BINARY_AND',
        '->': 'BINARY_PTR_MBR_ACCESS',
        '.': 'BINARY_MBR_ACCESS',
        '&': 'BINARY_LOGICAL_AND',
        '|': 'BINARY_LOGICAL_OR',
        '^': 'BINARY_LOGICAL_XOR',
        '+': 'BINARY_ADD',
        '-': 'BINARY_MINUS',
        '/': 'BINARY_DIVIDE',
        '*': 'BINARY_MULT',
        '%': 'BINARY_MOD',
        '&=': 'BINARY_LOGICAL_AND',
        '|=': 'BINARY_LOGICAL_OR',
        '^=': 'BINARY_LOGICAL_XOR',
        '+=': 'BINARY_ADD',
        '-=': 'BINARY_MINUS',
        '/=': 'BINARY_DIVIDE',
        '*=': 'BINARY_MULT',
        '%=': 'BINARY_MOD',
        '<': 'BINARY_LT',
        '>': 'BINARY_GT',
        '<=': 'BINARY_LE',
        '>=': 'BINARY_GE',
    }

    for tok in cursor.get_tokens():
        if tok.spelling in binary_operators:
            return binary_operators[tok.spelling]

    return 'UNKNOWN_BINARY'

    # Same problem as with unary operators
    # raise Exception("Unknown binary operator")

def get_unary_type(cursor):
    """
    This doesn't handle pre/post {inc,dec}
    This is needed because https://reviews.llvm.org/D10833?id=39158 isn't merged.
    """

    unary_operators = {
        '--': 'UNARY_DECREMENT',
        '++': 'UNARY_INCREMENT',
        '*': 'UNARY_DEREF',
        '&': 'UNARY_REF',
        '-': 'UNARY_MINUS',
        '!': 'UNARY_NOT',
    }

    for tok in cursor.get_tokens():
        if tok.spelling in unary_operators:
            return unary_operators[tok.spelling]


    # Clang is broken, when expanding some macros from different files.
    # EOF is broken, for example...
    # NULL is also broken, but not here, it just doesn't appear in the AST

    return 'UNKNOWN'
    # raise Exception(f'Unknown unary operator at {cursor.location}')

def print_token(cursor):
    for tok in cursor.get_tokens():
        print(f'{tok.spelling}', end='')
        break

def print_tokens(cursor):
    print('"', "".join((x.spelling for x in cursor.get_tokens())), '"', end="")

def handle_integer(cursor):
    print(f'{cursor.type.spelling} ', end='')
    print_token(cursor)

def handle_char(cursor):
    print(f'{cursor.type.spelling} ', end='')
    print_token(cursor)

def handle_unary(cursor):
    print(get_unary_type(cursor), end='')

def handle_unary_cxx(cursor):
    print(f'{cursor.spelling} ', end='')
    print_tokens(cursor)

def print_name_type(cursor):
    print(f'{cursor.spelling} "{cursor.type.spelling}"', end='')

def handle_var_decl(cursor):
    print(f'{cursor.spelling} "{cursor.type.spelling}"', end='')

def handle_binary(cursor):
    print(get_binary_type(cursor), end='')

def handle_string(cursor):
    print(f'{cursor.spelling}', end='')

def handle_decl_ref(cursor):
    print(f'{cursor.spelling}', end='')

class Dumper:
    handlers = {
        CursorKind.FUNCTION_DECL: print_name_type,
        CursorKind.INTEGER_LITERAL: handle_integer,
        CursorKind.CHARACTER_LITERAL: handle_char,
        CursorKind.UNARY_OPERATOR: handle_unary,
        CursorKind.BINARY_OPERATOR: handle_binary,
        CursorKind.COMPOUND_ASSIGNMENT_OPERATOR: handle_binary,
        CursorKind.CXX_UNARY_EXPR: handle_unary_cxx,
        CursorKind.FIELD_DECL: print_name_type,
        CursorKind.PARM_DECL: print_name_type,
        CursorKind.VAR_DECL: handle_var_decl,
        CursorKind.CSTYLE_CAST_EXPR: print_name_type,
        CursorKind.STRING_LITERAL: handle_string,
        CursorKind.DECL_REF_EXPR: handle_decl_ref,
    }

    def __init__(self, sources):
        self.sources = set([path.abspath(x) for x in sources])
        self.index = Index.create()
        self.units = [self.index.parse(
            path=source
            ,options=clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
        ) for source in self.sources]

    def dump_all(self):
        if len(self.units) > 1:
            print('(UNIT_LIST')
        for unit in self.units:
            self.test(unit.cursor, 1)
            print('')
        if len(self.units) > 1:
            print(')')

    def test(self, node, level=0):
        if node.kind == CursorKind.UNEXPOSED_EXPR:
            for c in node.get_children():
                self.test(c, level)
            return

        should_print = node.location and node.location.file and node.location.file.name in self.sources
        if not should_print and node.kind == CursorKind.TRANSLATION_UNIT:
            should_print = True

        children = list(node.get_children())

        if should_print:
            print(f"{' ' * level}({node.kind.name} ", end='')
            if node.kind in self.handlers:
                self.handlers[node.kind](node)
            else:
                if node.spelling != '':
                    print(f'"{node.spelling}"', end='')
            if len(children) > 0:
                print('')

        for i, c in enumerate(children):
            self.test(c, level + 1)
            if i != len(children) - 1:
                print('')

        if should_print:
            print(')', end='')


if __name__ == '__main__':
    d = Dumper(sys.argv[1:])
    d.dump_all()
