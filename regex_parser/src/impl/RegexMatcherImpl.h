#pragma once

#ifndef __REGEX_MATCHER_IMPL_H__
#define __REGEX_MATCHER_IMPL_H__

#include "../RegexMatcher.h"
#include "NFA.h"
#include "DFA.h"

class Alphabet;

class RegexMatcherImpl : public RegexMatcher
{
	NFA *m_nfa;
	DFA *m_dfa;

public:
	RegexMatcherImpl(const char *regex, Alphabet *alphabet = NULL);
	~RegexMatcherImpl();

	virtual bool match(const char *input);
	virtual bool matchPartially(const char *input, int *pMatchLength);
	virtual bool contains(const char *input, int *startPos, int *endPos);

	NFA *nfa() const { return m_nfa; }
	DFA *dfa() const { return m_dfa; }
};

#endif