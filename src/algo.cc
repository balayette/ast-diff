#include "algo.hh"

#include <algorithm>
#include <iostream>

#include "heap.hh"
#include "utils.hh"

struct GumtreeData
{
	std::shared_ptr<Tree> SourceTree;
	std::shared_ptr<Tree> DestTree;

	Heap L1;
	Heap L2;

	std::vector<std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>>
	    Candidates;
	std::vector<std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>>
	    Map;

	int MinHeight;
};

void differentHeights(GumtreeData& data)
{
	if (data.L1.PeekMax() > data.L2.PeekMax())
	{
		for (auto t : data.L1.Pop())
			data.L1.Open(t);
		return;
	}

	for (auto t : data.L2.Pop())
		data.L2.Open(t);
}

void addPairOfIsomorphicDescendants(GumtreeData& data,
				    std::shared_ptr<Tree>& t1,
				    std::shared_ptr<Tree>& t2)
{
	auto desc1 = GetDescendants(t1);
	auto desc2 = GetDescendants(t2);
	foreach_pair(desc1.begin(), desc1.end(), desc2.begin(), desc2.end(),
		     [&](std::shared_ptr<Tree>& a, std::shared_ptr<Tree>& b) {
			     if (a->IsIsomorphic(b))
				     data.Map.emplace_back(a, b);
		     });
}

void isomorphicPair(GumtreeData& data, std::shared_ptr<Tree>& t1,
		    std::shared_ptr<Tree>& t2)
{
	auto iso1 = FindAll(data.DestTree, [&](std::shared_ptr<Tree>& tx) {
		return t1->IsIsomorphic(tx);
	});
	auto iso2 = FindAll(data.SourceTree, [&](std::shared_ptr<Tree>& tx) {
		return tx->IsIsomorphic(t2);
	});

	auto it1 =
	    std::find_if(iso1.begin(), iso1.end(),
			 [&](std::shared_ptr<Tree>& tx) { return tx != t2; });
	auto it2 =
	    std::find_if(iso2.begin(), iso2.end(),
			 [&](std::shared_ptr<Tree>& tx) { return tx != t1; });

	if (it1 != iso1.end() || it2 != iso2.end())
		data.Candidates.emplace_back(t1, t2);
	else
		addPairOfIsomorphicDescendants(data, t1, t2);
}

void doOpenT1(GumtreeData& data, std::shared_ptr<Tree>& t1)
{
	for (auto& [a, b] : data.Candidates)
	{
		if (a == t1)
			return;
	}

	for (auto& [a, b] : data.Map)
	{
		if (a == t1)
			return;
	}

	data.L1.Open(t1);
}

void doOpenT2(GumtreeData& data, std::shared_ptr<Tree>& t2)
{
	for (auto& [a, b] : data.Candidates)
	{
		if (b == t2)
			return;
	}

	for (auto& [a, b] : data.Map)
	{
		if (b == t2)
			return;
	}

	data.L2.Open(t2);
}

void sameHeights(GumtreeData& data)
{
	auto h1 = data.L1.Pop();
	auto h2 = data.L2.Pop();

	foreach_pair(h1.begin(), h1.end(), h2.begin(), h2.end(),
		     [&](auto& t1, auto& t2) {
			     if (!t1->IsIsomorphic(t2))
				     return;
			     isomorphicPair(data, t1, t2);
		     });

	for (auto& t1 : h1)
		doOpenT1(data, t1);
	for (auto& t2 : h2)
		doOpenT2(data, t2);
}

double dice(std::shared_ptr<Tree>& t1, std::shared_ptr<Tree>& t2)
{
	auto des1 = GetDescendants(t1);
	auto des2 = GetDescendants(t2);

	double c = (double)common_elements(des1.begin(), des1.end(),
					   des2.begin(), des2.end());

	return (2.0 * c) / ((double)des1.size() + (double)des2.size());
}

std::vector<std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>>
Gumtree(std::shared_ptr<Tree> SourceTree, std::shared_ptr<Tree> DestTree)
{
	GumtreeData data;
	data.SourceTree = SourceTree;
	data.DestTree = DestTree;

	data.MinHeight = 2;

	data.L1.Push(SourceTree);
	data.L2.Push(DestTree);

	while (std::min(data.L1.PeekMax(), data.L2.PeekMax()) > data.MinHeight)
	{
		if (data.L1.PeekMax() != data.L2.PeekMax())
			differentHeights(data);
		else
			sameHeights(data);
	}

	std::sort(
	    data.Candidates.begin(), data.Candidates.end(),
	    [&](std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>& a,
		std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>& b) {
		    return dice(a.first->GetParent(), a.second->GetParent())
			< dice(b.first->GetParent(), b.second->GetParent());
	    });

	while (data.Candidates.size() > 0)
	{
		auto pair = data.Candidates[0];
		data.Candidates.erase(data.Candidates.begin());
		addPairOfIsomorphicDescendants(data, pair.first, pair.second);

		std::remove_if(data.Candidates.begin(), data.Candidates.end(),
			       [&](auto& p) {
				       return p.first == pair.first
					   || p.second == pair.second;
			       });
	}

	return data.Map;
}

