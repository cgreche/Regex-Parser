
#undef NULL
#define NULL 0

#include <cstdio>
#include "RegexParser.h"

//POSIX reference: http://pubs.opengroup.org/onlinepubs/9699919799/
//

enum Sym
{
	SYM_INVALID = -1,
	SYM_EOS = 0, //end of string
	ORD_CHAR,
	SPEC_CHAR,
	QUOTED_CHAR,
	DUP_COUNT,
//	BACKREF, L_ANCHOR, R_ANCHOR,       BRE only
	Back_Open_Paren, Back_Close_Paren,
	COLL_ELEM_SINGLE, COLL_ELEM_MULTI, META_CHAR,
	Open_equal,
	Equal_close,
	Open_dot,
	Dot_close,
	Open_colon,
	Colon_close,
	class_name
};

int RegexParser::nextSym()
{
	char *c = m_curChar;
	int sym = SYM_INVALID;
	if(*c == '\0') {
		return m_curSym = SYM_EOS;
	}

	m_curTok = "";

	if(m_onOpenColon) {
		while(*c >= 'a' && *c <= 'z') {
			m_curTok += *c++;
		}

		sym = class_name;
	}
	else
	if(m_onBracketExpression) {
		if(c[0] == '[') {
			if(c[1] == '=') {
				sym = Open_equal;
				c += 2;
			}
			else if(c[1] == '.') {
				sym = Open_dot;
				c += 2;
			}
			else if(c[1] == ':') {
				sym = Open_colon;
				c += 2;
			}
			else {
				sym = *c++;
			}
		}
		else if(c[0] == '=' && c[1] == ']') {
			sym = Equal_close;
			c += 2;
		}
		else if(c[0] == '.' && c[1] == ']') {
			sym = Dot_close;
			c += 2;
		}
		else if(c[0] == ':' && c[1] == ']') {
			sym = Colon_close;
			c += 2;
		}
		else {
			if(*c == '^')
				sym = '^';
			else if(*c == ']') {
				sym = ']';
			}
			else {
				if(*c == '-' && !m_onRangeExpression) {
					sym = '-';
				}
				else
					sym = COLL_ELEM_SINGLE;
			}
			m_curTok = *c++;
		}
	}
	else {
		if(*c == '\\') {
			++c;
			//quoted char
			if(*c == '^' || *c == '.' || *c == '[' || *c == '$' || *c == '('
				|| *c == ')' || *c == '|' || *c == '*' || *c == '+' || *c == '?'
				|| *c == '{' || *c == '\\') {
				m_curTok = *c;
				sym = QUOTED_CHAR;
			}
			else {
				if(*c == 't') { m_curTok = '\t'; }
				else if(*c == 'r') { m_curTok = '\r'; }
				else if(*c == 'n') { m_curTok = '\n'; }
				else if(*c == 'v') { m_curTok = '\v'; }
				else if(*c == 'f') { m_curTok = '\f'; }
				else {
					//todo: invalid escape character?
					m_curTok = *c;
				}
				sym = ORD_CHAR;
			}
			++c;
		}
		else {
			if(*c != '^' && *c != '.' && *c != '[' && *c != '$' && *c != '('
				&& *c != ')' && *c != '|' && *c != '*' && *c != '+'
				&& *c != '?' && *c != '{') {
				sym = ORD_CHAR;
			}
			else {
				sym = *c; //todo: change to SPEC_CHAR?
			}
			m_curTok = *c++;
		}
	}
	m_curChar = c;
	return m_curSym = sym;
}

int RegexParser::accept(int sym)
{
	if(m_curSym == sym) {
		nextSym();
		return 1;
	}
	return 0;
}

void RegexParser::expect(int sym)
{
	if(!accept(sym))
		error("Fudeu.");
}

void RegexParser::error(const char *msg)
{
	throw msg;
}

bool RegexParser::acceptNumeric(int *pOut)
{
	int num = 0;
	int digitCount = 0;
	while(m_curSym == ORD_CHAR && m_curTok[0] >= '0' && m_curTok[0] <= '9') {
		num *= 10;
		num += (m_curTok[0] - '0');
		nextSym();
		++digitCount;
	}

	if(pOut)
		*pOut = num;
	return digitCount > 0;
}

ClassNameID RegexParser::classNameToId(const char *className)
{
	static const char *classNames[] =
	{
		"upper",
		"lower",
		"alpha",
		"digit",
		"alnum",
		"space",
		"cntrl",
		"punct",
		"graph",
		"print",
		"xdigit",
		"blank",
		""
	};

	int id = 0;
	while(*classNames[id] != '\0') {
		if(strcmp(classNames[id], className) == 0)
			return (ClassNameID)id;
		++id;
	}

	return INVALID;
}


RegexNode * RegexParser::_regex()
{
	RegexNode *res;
	res = _union();
	return res;
}

RegexNode *RegexParser::_union()
{
	RegexNode *op1 = _concat();
	if(op1) {
		if(accept('|')) {
			RegexNode *op2 = _union();
			if(!op2)
				error("Syntax error.");
			return newAlternationNode(op1,op2);
		}
	}
	return op1;
}

RegexNode *RegexParser::_concat()
{
	RegexNode *op1 = _atom();
	if(op1) {
		RegexNode *op2 = _concat();
		if(op2) {
			return newConcatenationNode(op1, op2);
		}
	}
	return op1;
}

RegexNode *RegexParser::_atom()
{
	RegexNode *result;
	if(accept('(')) {
		result = _regex();
		expect(')');
	}
	else if(accept('^')) {
		int a = 1;
	}
	else if(accept('$')) {
		int a = 1;
	}
	else {
		result = _elem();
	}

	while(result) {
		int min, max;
		bool lazy = false;
		if(accept('*')) {
			//QUANTIFICATION - KLEENE STAR
			min = 0;
			max = QUANT_ANY;
		}
		else if(accept('+')) {
			//QUANTIFICATION - KLEENE PLUS
			min = 1;
			max = QUANT_ANY;
		}
		else if(accept('?')) {
			//QUANTIFICATION - QUESTION MARK
			min = 0;
			max = 1;
		}
		else if(accept('{')) {
			//expect an integer value now
			if(!acceptNumeric(&min))
				error("Syntax Error.");
			if(m_curTok[0] ==',') {
				nextSym();
				if(!acceptNumeric(&max))
					max = QUANT_ANY;
			}
			else {
				max = min;
			}
			if(m_curTok[0] != '}')
				error("Syntax error.");
			nextSym();
		}
		else
			break;

		if(accept('?')) {
			//make Quantifier operator non greedy (extension to POSIX)
			lazy = true;
		}

		result = newQuantifierNode(result, min, max, lazy);
	}

	return result;
}

RegexNode *RegexParser::_elem()
{
	RegexNode *result = NULL;
	if(m_curSym == '\0')
		return NULL;

	if(accept('.')) {
		return newLiteralNode(SYM_ANY);
	}
	else if(m_curSym == '[') { //can't use accept
		//bracket expression
		m_onBracketExpression = true;
		nextSym();
		RegexNode* bracketElem = _bracketElemList();
		if(m_curSym != ']')
			error("Syntax error.");
		m_onBracketExpression = false;
		nextSym();
		return bracketElem;
	}
	else if(m_curSym == QUOTED_CHAR || m_curSym == ORD_CHAR) {
		result = newLiteralNode(m_curTok[0]);
		nextSym();
	}

	return result;
}


//bracket expression
RegexNode *RegexParser::_bracketElemList()
{
	BracketElem* elemStart = NULL, *curElem = NULL, *elem;
	bool negateElems = false;
	if(accept('^')) {
		negateElems = true;
	}

	do {
		elem = NULL;
		int sym = m_curSym;
		if(sym == Open_colon) {
			int classId;
			m_onOpenColon = true;
			if(nextSym() != class_name) {
				error("Syntax error.");
			}
			classId = classNameToId(m_curTok.c_str());
			m_onOpenColon = false;
			nextSym();
			expect(Colon_close);

			elem = new BracketElem(BET_CHARACTER_CLASS, NULL, NULL);
			elem->m_classId = classId;
		}
		else if(accept(Open_equal)) {
			if(accept(COLL_ELEM_SINGLE)) {

			}
			else if(accept(COLL_ELEM_MULTI)) {

			}
			else {
				//syntax error
				error("Syntax error.");
			}
			expect(Equal_close);
			return NULL;
		}
		else {

			if(!elemStart && m_curSym == ']') {
				//']' is accept as first char in a bracket expression
				BracketElemAtom *_closeBracket = new BracketElemAtom(0, (int)']');
				elem = new BracketElem(BET_COLL_ELEM_SINGLE, _closeBracket, NULL);
				nextSym();
			}
			else {

				BracketElemAtom *_collElem1;
				if(!elemStart && sym == '-') {
					//'-' is accepted as first elem in a bracket
					_collElem1 = new BracketElemAtom(0, (int)'-');
					nextSym();
				}
				else {
					_collElem1 = _collElem();
				}

				if(_collElem1) {

					if(m_curSym == '-') {
						nextSym();
						if(m_curSym == ']') {
							//'-' is accepted as last elem in a bracket
							BracketElemAtom *atom = new BracketElemAtom(0, (int)'-');
							elem = new BracketElem(BET_COLL_ELEM_SINGLE, atom, NULL);
							BracketElem *collElem = new BracketElem(BET_COLL_ELEM_SINGLE, _collElem1, NULL);
							elem->setNext(collElem);
						}
						else {
							BracketElemAtom *_collElem2 = _collElem();
							if(!_collElem2)
								error("Syntax Error.");
							elem = new BracketElem(BET_RANGE, _collElem1, _collElem2);
						}

					}
					else {
						elem = new BracketElem(BET_COLL_ELEM_SINGLE, _collElem1, NULL);
					}
				}
			}
		}

		if(elem) {
			if(!elemStart) elemStart = elem;
			if(curElem) curElem->setNext(elem);
			curElem = elem;
		}
	} while(elem);

	if(elemStart)
		return newBracketExpression(elemStart,negateElems);

	return NULL;
}

BracketElemAtom *RegexParser::_collElem()
{
	if(accept(Open_dot)) {
		if(accept(COLL_ELEM_SINGLE)) {
		}
		else if(accept(COLL_ELEM_MULTI)) {
		}
		else if(accept(META_CHAR)) {
		}
		else {
			//syntax error
			error("Syntax error.");
		}

		expect(Dot_close);
		return NULL;
	}
	else {
		if(m_curSym == COLL_ELEM_SINGLE) {
			BracketElemAtom *atom = new BracketElemAtom(0, m_curTok[0]);
			nextSym();
			return atom;
		}
	}

	return NULL;
}

RegexParser::RegexParser()
{
	m_curSym = 0x80000000;
	m_onBracketExpression = false;
	m_onRangeExpression = false;
	m_onOpenColon = false;
}

RegexNode *RegexParser::parse(const char *input)
{	m_input = input;
	m_curChar = (char*)input;
	nextSym();
	RegexNode *regex = _regex();
	expect('\0');
	return regex;
}
