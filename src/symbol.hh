#pragma once

#include <set>
#include <string>
#include <iostream>

class Symbol {
       public:
	Symbol(const std::string& str);
	Symbol(const char *str = "");

	Symbol& operator=(const Symbol& rhs);

	bool operator==(const Symbol& rhs);
	bool operator!=(const Symbol& rhs);
	friend std::ostream& operator<<(std::ostream& ostr, const Symbol& the);

       private:
	std::set<std::string>& getSet();
	const std::string* instance_;
};
