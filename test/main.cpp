#define _CRT_SECURE_NO_WARNINGS

#include <list>
#include <vector>
#include <set>
#include <bitset>
#include <limits>

#include <iostream>
#include <string>

#include <regex_parser_engine/src/RegexMatcher.h>

int main()
{
	RegexMatcher* regexMatcher = createRegexMatcher("ab*c", 0);
	bool b = regexMatcher->match("hello");
	printf("Matches: hello -> %d\n", (int)b);
	b = regexMatcher->match("abbbbc");
	printf("Matches: abbbbc -> %d\n", (int)b);
	system("PAUSE");
	return 0;
}