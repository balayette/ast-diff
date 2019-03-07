#pragma once

template <typename Iterator, typename Func>
void foreach_pair(Iterator b1, Iterator e1, Iterator b2, Iterator e2, Func f);

template <typename Iterator>
int common_elements(Iterator b1, Iterator e1, Iterator b2, Iterator e2);

#include "utils.hxx"
