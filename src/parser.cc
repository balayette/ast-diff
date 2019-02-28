#include "parser.hh"
#include <iostream>
#include <stack>
#include "token.hh"

Parser::Parser(Lexer& lexer) : lexer_(lexer) {}

std::shared_ptr<Tree> Parser::Parse() {
	auto stack = std::stack<std::shared_ptr<Tree>>{};

	stack.push(std::make_shared<Tree>());

	for (auto tok = lexer_.Peek(); tok != nullptr;
	     lexer_.Eat(), tok = lexer_.Peek()) {
		auto type = tok->GetType();
		if (type == Token::TokenType::LPAREN)
			stack.push(std::make_shared<Tree>());
		else if (type == Token::TokenType::RPAREN)
		{
			auto top = stack.top();
			stack.pop();
			if (stack.size() == 0) {
				std::cerr << "Unexpected )\n";
				std::exit(2);
			}

			auto t = stack.top();
			t->AddChild(top);
		}
		else
		{
			if (stack.top()->GetValue().size() == 0)
			{
				stack.pop();
				stack.push(std::make_shared<Tree>(tok->GetString(), stack.top().get()));
			}
			else
			{
				auto add = std::make_shared<Tree>(tok->GetString(), stack.top().get());
				stack.top()->AddChild(add);
			}
		}
	}

	auto ret = stack.top();

	while (ret->ChildrenSize() == 1)
		ret = ret->GetChildren()[0];

	return ret;
}

