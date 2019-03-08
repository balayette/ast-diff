#include "mappings.hh"

#include <algorithm>

void Mappings::AddMapping(treeptr& t1, treeptr& t2)
{
	if (ContainsMapping(t1, t2))
		return;

	mappings_.emplace_back(t1, t2);
}

bool Mappings::ContainsMapping(treeptr& t1, treeptr& t2)
{
	return std::any_of(
	    mappings_.begin(), mappings_.end(),
	    [&](const auto& p) { return p.first == t1 && p.second == t2; });
}

bool Mappings::ContainsMappingFirst(treeptr& t)
{
	return std::any_of(mappings_.begin(), mappings_.end(),
			   [&](const auto& p) { return p.first == t; });
}

bool Mappings::ContainsMappingSecond(treeptr& t)
{
	return std::any_of(mappings_.begin(), mappings_.end(),
			   [&](const auto& p) { return p.second == t; });
}

Mappings::treepair& Mappings::operator[](const int idx)
{
	return mappings_[idx];
}

int Mappings::size()
{
	return mappings_.size();
}

Mappings::mapping_iterator Mappings::begin()
{
	return mappings_.begin();
}

Mappings::mapping_iterator Mappings::end()
{
	return mappings_.end();
}

Mappings::mapping_iterator Mappings::erase(Mappings::mapping_iterator position)
{
	return mappings_.erase(position);
}