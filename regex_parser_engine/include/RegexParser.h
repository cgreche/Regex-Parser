#pragma once

#ifndef __REGEX_PARSER_H__
#define __REGEX_PARSER_H__

#include "RegexNode.h"

#include <string>

class RegexParser
{
	int nextSym();
	int accept(int sym);
	void expect(int sym);
	void error(const char *msg);

	ClassNameID classNameToId(const char *className);
	RegexNode * _elem();
	RegexNode * _atom();
	RegexNode * _concat();
	RegexNode * _union();
	RegexNode * _regex();

	//
	RegexNode * _bracketElemList();
	BracketElemAtom * _collElem();

	const char *m_input;
	char *m_curChar;
	std::string m_curTok;
	int m_curSym;

	bool m_onBracketExpression;
	bool m_onRangeExpression;
	bool m_onOpenColon;

	//For DEBUG Only:
	inline RegexNode *newDummyNode() { return new RegexLiteral(-3); }

	inline RegexLiteral *newLiteralNode(int sym) { return new RegexLiteral(sym); }
	inline RegexConcat *newConcatenationNode(RegexNode* left, RegexNode* right) { return new RegexConcat(left,right);	}
	inline RegexAlternation *newAlternationNode(RegexNode* left, RegexNode* right) { return new RegexAlternation(left, right); }
	inline RegexQuantifier *newQuantifierNode(RegexNode* expression, int min, int max, bool lazy = false) { return new RegexQuantifier(expression,min,max,lazy); }
	inline RegexBracketExp *newBracketExpression(BracketElem *firstBracketElem, bool negateElems = false) { return new RegexBracketExp(firstBracketElem, negateElems); }

	bool acceptNumeric(int *pOut);

public:
	RegexParser();
	RegexNode *parse(const char *input);
};

#endif