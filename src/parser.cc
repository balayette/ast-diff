#include "parser.hh"

#include <iostream>
#include <stack>

#include "token.hh"

Parser::Parser(Lexer &lexer) : lexer_(lexer) {}

Tree::sptr Parser::Parse() {
  auto stack = std::stack<Tree::sptr>{};

  stack.push(std::make_shared<Tree>());

  for (auto tok = lexer_.Peek(); tok != nullptr;
       lexer_.Eat(), tok = lexer_.Peek()) {
    auto type = tok->GetType();
    if (type == Token::TokenType::LPAREN)
      stack.push(std::make_shared<Tree>());
    else if (type == Token::TokenType::RPAREN) {
      auto top = stack.top();
      stack.pop();
      if (stack.size() == 0) {
        std::cerr << "Unexpected )\n";
        std::exit(2);
      }

      auto t = stack.top();
      t->AddChild(top);
    } else {
      if (stack.top()->GetValue().size() == 0) {
        stack.pop();
        stack.push(std::make_shared<Tree>(tok->GetString()));
      } else {
        auto add = std::make_shared<Tree>(tok->GetString());
        stack.top()->AddChild(add);
      }
    }
  }

  auto ret = stack.top();

  while (ret->GetValue().size() == 0)
    ret = ret->GetChildren()[0];

  return ret;
}
