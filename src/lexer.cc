#include "lexer.hh"
#include <iostream>
#include <string>

Lexer::Lexer(std::istream& stream)
    : stream_(stream), state_(START), rollback_(false), pos_(0), line_(0) {
	Eat();
}

std::shared_ptr<Token> Lexer::Peek() {
	if (stream_.eof()) return nullptr;
	return tok_;
}

void Lexer::Eat() {
	if (stream_.eof()) tok_ = nullptr;

	char c = 0;
	while (!stream_.eof()) {
		if (rollback_) {
			rollback_ = false;
			c = last_char_;
		} else {
			stream_.get(c);
			last_char_ = c;
			if (c == '\n') {
				line_++;
				pos_ = 0;
			} else
				pos_++;
		}

		switch (state_) {
			case START:
				handleStart(c);
				break;
			case ATOM:
				handleAtom(c);
				break;
			case STRING_LIT:
				handleStringLit(c);
				break;
		}
		if (state_ == START) break;
	}
}

void Lexer::handleStart(char c) {
	switch (c) {
		case '(':
			state_ = START;
			tok_ = std::make_shared<Token>(Token::LPAREN);
			break;
		case ')':
			state_ = START;
			tok_ = std::make_shared<Token>(Token::RPAREN);
			break;
		case '"':
			state_ = STRING_LIT;
			break;
		case ' ':
		case '\n':
			stream_.get(c);
			last_char_ = c;
			handleStart(c);
			break;
		case EOF:
			state_ = START;
			return;
		default:
			state_ = ATOM;
			rollback_ = true;
			break;
	}
}

void Lexer::handleAtom(char c) {
	std::string str("");
	str += c;

	while(stream_.get(c))
	{
		switch (c) {
			case ' ':
			case '(':
			case ')':
			case '\n':
				state_ = START;
				tok_ = std::make_shared<Token>(Token::ATOM);
				tok_->SetString(str);
				if (c == '(' || c == ')') {
					rollback_ = true;
					last_char_ = c;
				}
				return;
			default:
				str += c;
				break;
		}
	}
}

void Lexer::handleStringLit(char c) {
	std::string str("\"");
	str += c;

	while (stream_.get(c))
	{
		switch (c) {
			case '"':
				str += '"';
				state_ = START;
				tok_ =
				    std::make_shared<Token>(Token::ATOM);
				tok_->SetString(str);
				return;
			case '\\':
				stream_.get(c);
				if (c == EOF) {
					std::cerr << "Unexpected EOF at line "
						  << line_ << " char " << pos_
						  << '\n';
					std::exit(1);
				}
				str += c;
				break;
			default:
				str += c;
				break;
		}
	}
}
