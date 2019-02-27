#pragma once

#include <ostream>
#include <string>

class Token {
       public:
	enum TokenType { ATOM, LPAREN, RPAREN };

	Token(TokenType type);

	void SetString(std::string& str);
	std::string& GetString();

	TokenType GetType();

	friend std::ostream& operator<<(std::ostream& stream,
					const Token& tok);

       private:
	std::string str_;
	TokenType type_;
};
