#include "token.hh"

#include <iostream>

Token::Token(Token::TokenType type, int line, int pos)
    : type_(type), line_(line), pos_(pos) {}

void Token::SetString(std::string &str) { str_ = str; }

std::string &Token::GetString() { return str_; }

Token::TokenType Token::GetType() { return type_; }

int Token::GetLine() { return line_; }
int Token::GetPos() { return pos_; }

std::ostream &operator<<(std::ostream &stream, const Token &tok) {
  switch (tok.type_) {
  case Token::TokenType::ATOM:
    stream << "ATOM(" << tok.str_ << ')';
    break;
  case Token::TokenType::LPAREN:
    stream << "LPAREN";
    break;
  case Token::TokenType::RPAREN:
    stream << "RPAREN";
    break;
  }

  return stream;
}
