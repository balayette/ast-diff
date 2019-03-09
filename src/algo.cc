#include "algo.hh"

#include <algorithm>
#include <iostream>

#include "heap.hh"
#include "utils.hh"

struct GumtreeData
{
	Tree& SourceTree;
	Tree& DestTree;

	Heap L1;
	Heap L2;

	Mappings Candidates;
	Mappings Map;

	int MinHeight;

	inline GumtreeData(Tree& src, Tree& dst)
	    : SourceTree(src)
	    , DestTree(dst){};
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

void addPairOfIsomorphicDescendants(GumtreeData& data, Tree& t1, Tree& t2)
{
	auto desc1 = GetDescendants(t1);
	auto desc2 = GetDescendants(t2);
	foreach_pair(desc1.begin(), desc1.end(), desc2.begin(), desc2.end(),
		     [&](Tree& a, Tree& b) {
			     std::cout << a.GetValue() << " iso "
				       << b.GetValue() << '?';
			     if (a.IsIsomorphic(b))
			     {
				     data.Map.AddMapping(a, b);
				     std::cout << " Yes.\n";
			     } else
				     std::cout << " No.\n";
		     });
}

void isomorphicPair(GumtreeData& data, Tree& t1, Tree& t2)
{
	auto iso1 = FindAll(data.DestTree,
			    [&](Tree& tx) { return t1.IsIsomorphic(tx); });
	auto iso2 = FindAll(data.SourceTree,
			    [&](Tree& tx) { return tx.IsIsomorphic(t2); });

	auto it1 = std::find_if(iso1.begin(), iso1.end(),
				[&](Tree& tx) { return tx != t2; });
	auto it2 = std::find_if(iso2.begin(), iso2.end(),
				[&](Tree& tx) { return tx != t1; });

	if (it1 != iso1.end() || it2 != iso2.end())
	{
		std::cout << "Multiple matches\n";
		for (auto it = it1; it != iso1.end(); it++)
			data.Candidates.AddMapping(t1, *it);
		for (auto it = it2; it != iso2.end(); it++)
			data.Candidates.AddMapping(*it, t2);
	} else
	{
		std::cout << "Single match, cool\n";
		data.Map.AddMapping(t1, t2);
		addPairOfIsomorphicDescendants(data, t1, t2);
	}
}

void doOpenT1(GumtreeData& data, Tree& t1)
{
	for (auto& [a, b] : data.Candidates)
	{
		if (a.get() == t1)
			return;
	}

	for (auto& [a, b] : data.Map)
	{
		if (a.get() == t1)
			return;
	}

	data.L1.Open(t1);
}

void doOpenT2(GumtreeData& data, Tree& t2)
{
	for (auto& [a, b] : data.Candidates)
	{
		if (b.get() == t2)
			return;
	}

	for (auto& [a, b] : data.Map)
	{
		if (b.get() == t2)
			return;
	}

	data.L2.Open(t2);
}

void sameHeights(GumtreeData& data)
{
	auto h1 = data.L1.Pop();
	auto h2 = data.L2.Pop();

	std::cout << "h1\n";
	for (auto& it : h1)
		std::cout << it.get().GetValue() << '\n';

	std::cout << "h2\n";
	for (auto& it : h2)
		std::cout << it.get().GetValue() << '\n';

	foreach_pair(h1.begin(), h1.end(), h2.begin(), h2.end(),
		     [&](auto& t1, auto& t2) {
			     std::cout << t1.get().GetValue() << " iso to "
				       << t2.get().GetValue() << '?';
			     if (!t1.get().IsIsomorphic(t2))
			     {
				     std::cout << " No.\n";
				     return;
			     }
			     std::cout << " Yes.\n";
			     isomorphicPair(data, t1, t2);
		     });

	for (const auto& t1 : h1)
		doOpenT1(data, t1);
	for (const auto& t2 : h2)
		doOpenT2(data, t2);
}

double dice(Tree& t1, Tree& t2)
{
	auto des1 = GetDescendants(t1);
	auto des2 = GetDescendants(t2);

	double c = (double)common_elements(des1.begin(), des1.end(),
					   des2.begin(), des2.end());

	return (2.0 * c) / ((double)des1.size() + (double)des2.size());
}

Mappings Gumtree(Tree& SourceTree, Tree& DestTree)
{
	GumtreeData data(SourceTree, DestTree);

	data.MinHeight = 1;

	data.L1.Push(SourceTree);
	data.L2.Push(DestTree);

	for (int min = std::min(data.L1.PeekMax(), data.L2.PeekMax());
	     min != -1 && min > data.MinHeight;
	     min = std::min(data.L1.PeekMax(), data.L2.PeekMax()))
	{
		if (data.L1.PeekMax() != data.L2.PeekMax())
			differentHeights(data);
		else
			sameHeights(data);
	}

	std::cout << "Data candidates size " << data.Candidates.size() << '\n';
	std::sort(data.Candidates.begin(), data.Candidates.end(),
		  [&](Mappings::treepair& a, Mappings::treepair& b) {
			  return a.first.get().GetParent().Dice(
				     a.second.get().GetParent(), data.Map)
			      < b.first.get().GetParent().Dice(
				    b.second.get().GetParent(), data.Map);
		  });

	while (data.Candidates.size() > 0)
	{
		auto pair = data.Candidates[0];
		data.Candidates.erase(data.Candidates.begin());
		addPairOfIsomorphicDescendants(data, pair.first, pair.second);

		std::remove_if(data.Candidates.begin(), data.Candidates.end(),
			       [&](auto& p) {
				       return p.first.get() == pair.first.get()
					   || p.second.get()
					   == pair.second.get();
			       });
	}

	return data.Map;
}

void bottomUp(Tree& t1, Tree& T2, Mappings& M)
{
	for (auto& child : t1.GetChildren())
		bottomUp(*child, T2, M);

	std::cout << "bottom up on " << t1.GetValue() << '\n';
	if (t1.GetChildren().size() > 0)
	{
		// FIXME: Add mindice, when dice is implemented.
		auto t2 = T2.Candidate(t1, M);
		if (t2 == std::nullopt)
		{
			std::cout << "Didnt find a candidate.\n";
			return;
		}
		std::cout << "Foud a candidate " << t2->get().GetValue()
			  << '\n';

		M.AddMapping(t1, t2->get());
	} else
	{
		std::cout << "No children, not running\n";
	}
}

void BottomUp(Tree& T1, Tree& T2, Mappings& M)
{
	auto& children = T1.GetChildren();
	if (!M.ContainsMappingFirst(T1)
	    && std::any_of(children.begin(), children.end(),
			   [&](Tree::ptr& child) {
				   return M.ContainsMappingFirst(*child);
			   }))
	{
		bottomUp(T1, T2, M);
	} else
	{
		for (auto& child : children)
			bottomUp(*child, T2, M);
	}
}
