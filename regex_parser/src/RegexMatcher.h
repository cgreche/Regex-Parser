#ifndef __REGEX_MATCHER_H__
#define __REGEX_MATCHER_H__

#include "Alphabet.h"

class RegexMatcher
{
public:
	virtual bool match(const char* input) = 0;
	virtual bool matchPartially(const char* input, int* pMatchLength) = 0;
	virtual bool contains(const char* input, int* startPos, int* endPos) = 0;
};

extern "C"
RegexMatcher* createRegexMatcher(const char* regex, Alphabet* alphabet);

#endif