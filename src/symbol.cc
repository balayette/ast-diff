#include "symbol.hh"

Symbol::Symbol(const std::string& str) {
	std::cout << "String: " << str << '\n';
	getSet().insert(str);
	instance_ = &(*(getSet().find(str)));
	std::cout << "instance: " << instance_ << '\n';
}

Symbol::Symbol(const char *str) : Symbol(std::string(str)) {}

std::set<std::string>& Symbol::getSet()
{
	static std::set<std::string> set{};

	return set;
}

Symbol& Symbol::operator=(const Symbol& rhs) {
	if (this == &rhs) return *this;

	instance_ = rhs.instance_;
	return *this;
}

bool Symbol::operator==(const Symbol& rhs) {
	return rhs.instance_ == instance_;
}

bool Symbol::operator!=(const Symbol& rhs) {
	return !(rhs.instance_ == instance_);
}

std::ostream& operator<<(std::ostream& ostr, const Symbol& the) {
	return ostr << the.instance_;
}
