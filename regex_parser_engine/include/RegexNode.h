#pragma once

#ifndef __REGEX_NODE_H__
#define __REGEX_NODE_H__

#define SYM_ANY 0x80000000
#define QUANT_ANY 0xffffffff

enum ClassNameID
{
	INVALID = -1,
	UPPER,
	LOWER,
	ALPHA,
	DIGIT,
	ALNUM,
	SPACE,
	CNTRL,
	PUNCT,
	GRAPH,
	PRINT,
	XDIGIT,
	BLANK,
};

enum RegexNodeType
{
	RETP_LITERAL,
	RETP_CONCAT,
	RETP_ALTERNATION,
	RETP_QUANTIFIER,
	RETP_BRACKET_EXPR,
};

enum BracketElemType
{
	BET_COLL_ELEM_SINGLE,
	BET_COLL_ELEM_MULTI,
	BET_RANGE,
	BET_CHARACTER_CLASS,
	BET_EQUIVALENCE_CLASS
};

class BracketElemAtom
{
	int m_type;
	int m_sym;

public:
	BracketElemAtom(int type, int sym) { m_type = type; m_sym = sym; }
	inline int type() const { return m_type; }
	inline int sym() const { return m_sym; }
};

class BracketElem
{
	int m_type;
	BracketElemAtom *m_symStart; //for range elements
	BracketElemAtom *m_symEnd;
	int m_classId;

	BracketElem *m_next;

public:
	friend class RegexParser;
	BracketElem(int type, BracketElemAtom *symStart, BracketElemAtom* symEnd) {
		m_type = type;
		m_symStart = symStart;
		m_symEnd = symEnd;
		m_next = NULL;
	}

	~BracketElem() {
		if(m_next)
			delete m_next;
	}

	inline void setNext(BracketElem *elem) {
		m_next = elem;
	}

	inline int type() const { return m_type; }
	inline BracketElem *next() const { return m_next; }
	inline BracketElemAtom *symStart() const { return m_symStart; }
	inline BracketElemAtom *symEnd() const { return m_symEnd; }

	inline int classId() const { return m_classId; }
};

class RegexNode
{
protected:
	RegexNodeType m_type;

public:
	friend class RegexParser;
	RegexNode(RegexNodeType type)
		: m_type(type) {
	}

	virtual ~RegexNode() { }

	inline RegexNodeType type() const {
 		return m_type;
	}
};

class RegexLiteral : public RegexNode
{
	int m_sym;

public:
	RegexLiteral(int sym)
		: RegexNode(RETP_LITERAL) {
		m_sym = sym;
	}

	inline int sym() const { return m_sym; }
};

class RegexConcat : public RegexNode
{
	RegexNode *m_left;
	RegexNode *m_right;

public:
	RegexConcat(RegexNode *left, RegexNode *right)
		: RegexNode(RETP_CONCAT) {
		m_left = left;
		m_right = right;
	}

	~RegexConcat() {
		if(m_left)
			delete m_left;
		if(m_right)
			delete m_right;
	}

	inline RegexNode *left() const { return m_left; }
	inline RegexNode *right() const { return m_right; }
};

class RegexAlternation : public RegexNode
{
	RegexNode *m_left;
	RegexNode *m_right;

public:
	RegexAlternation(RegexNode *left, RegexNode *right)
		: RegexNode(RETP_ALTERNATION) {
		m_left = left;
		m_right = right;
	}

	~RegexAlternation() {
		if(m_left)
			delete m_left;
		if(m_right)
			delete m_right;
	}

	inline RegexNode *left() const { return m_left; }
	inline RegexNode *right() const { return m_right; }
};

class RegexQuantifier : public RegexNode
{
	RegexNode *m_expression;
	short m_min;
	short m_max;
	bool m_lazy; //extension to POSIX

public:
	RegexQuantifier(RegexNode *expression, int min, int max, bool lazy)
		: RegexNode(RETP_QUANTIFIER) {
		m_expression = expression;
		m_min = min;
		m_max = max;
		m_lazy = lazy;
	}

	inline RegexNode *expression() const { return m_expression; }
	inline int min() const { return m_min; }
	inline int max() const { return m_max; }
	inline bool isLazy() const { return m_lazy; }
};

class RegexBracketExp : public RegexNode
{
	BracketElem *m_firstBracketElem;
	bool m_negateElems;

public:
	RegexBracketExp(BracketElem *firstBracketElem, bool negateElems)
		: RegexNode(RETP_BRACKET_EXPR) {
		m_firstBracketElem = firstBracketElem;
		m_negateElems = negateElems;
	}

	~RegexBracketExp() {
		if(m_firstBracketElem)
			delete m_firstBracketElem;
	}

	inline BracketElem *firstElem() const { return m_firstBracketElem; }
	inline bool negateElems() const { return m_negateElems; }
};

#endif