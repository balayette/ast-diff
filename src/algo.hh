#pragma once

#include <map>
#include <memory>

#include "mappings.hh"
#include "tree.hh"

Mappings Gumtree(Tree& t1, Tree& t2);

void BottomUp(Tree& T1, Tree& T2, Mappings& M);
