#pragma once

#ifndef __REGEX_MATCHER_H__
#define __REGEX_MATCHER_H__

#include "Alphabet.h"
#include "NFA.h"
#include "DFA.h"
class Alphabet;

class RegexMatcher
{
	NFA *m_nfa;
	DFA *m_dfa;

public:
	RegexMatcher(const char *regex, Alphabet *alphabet = NULL);
	~RegexMatcher();

	bool match(const char *input);
	bool matchPartially(const char *input, int *pMatchLength);
	bool contains(const char *input, int *startPos, int *endPos);

	NFA *nfa() const { return m_nfa; }
	DFA *dfa() const { return m_dfa; }
};

#endif