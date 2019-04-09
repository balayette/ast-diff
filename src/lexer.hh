#pragma once

#include <istream>
#include <memory>
#include <variant>

#include "token.hh"

class Lexer {
public:
  explicit Lexer(std::istream &stream);

  std::shared_ptr<Token> Peek();
  void Eat();

private:
  enum States { START, ATOM, STRING_LIT };

  void handleStart(char c);
  void handleAtom(char c);
  void handleStringLit(char c);

  char nextChar();

  std::shared_ptr<Token> tok_;
  std::istream &stream_;
  States state_;

  char last_char_;
  bool rollback_;

  int pos_;
  int line_;
};
