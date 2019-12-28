#include "RegexMatcherImpl.h"
#include "RegexNode.h"
#include "AsciiAlphabet.h"
#include "NFA.h"
//#include "DFA.h"

static AsciiAlphabet defaultAlphabet;

void createDefaultSymSets(Alphabet *alphabet)
{
	if(g_symSetTable.size() == 0) {
		g_symSetTable.reserve(100);
		g_symSetTable.resize(50);
		//set table is not created yet
		//create with default sets:
		//[0] = [[:upper:]]
		//[1] = [[:lower:]]
		//[2] = [[:alpha:]]
		//[3] = [[:digit:]]
		//[4] = [[:alnum:]]
		//[5] = [[:space:]]
		//[6] = [[:cntrl:]]
		//[7] = [[:punct:]]
		//[8] = [[:graph:]]
		//[9] = [[:print:]]
		//[10] = [[:xdigit:]]
		//[11] = [[:blank:]]
		//[12~19] = reserved1
		//[30] = . (any)
		//[31~49] = reserved2
		//[50~...] = userDefined
#define ANY 30
		g_symSetTable[UPPER] = new Bitset;
		g_symSetTable[LOWER] = new Bitset;
		g_symSetTable[ALPHA] = new Bitset;
		g_symSetTable[DIGIT] = new Bitset;
		g_symSetTable[ALNUM] = new Bitset;
		g_symSetTable[SPACE] = new Bitset;
		g_symSetTable[CNTRL] = new Bitset;
		g_symSetTable[PUNCT] = new Bitset;
		g_symSetTable[GRAPH] = new Bitset;
		g_symSetTable[PRINT] = new Bitset;
		g_symSetTable[XDIGIT] = new Bitset;
		g_symSetTable[BLANK] = new Bitset;
		g_symSetTable[ANY] = new Bitset;

		//. accepts any character (including '\n') except NUL
		g_symSetTable[ANY]->set();
		g_symSetTable[ANY]->reset(0); 
		 
#define _SET(cl,f) if(alphabet->f(i)) g_symSetTable[cl]->set(i);
		for(int i = 0; i < 0x80; ++i) {
			_SET(UPPER,isUpper)
			_SET(LOWER,isLower)
			_SET(ALPHA,isAlpha)
			_SET(DIGIT,isDigit)
			_SET(ALNUM,isAlnum)
			_SET(SPACE,isSpace)
			_SET(CNTRL,isCntrl)
			_SET(PUNCT,isPunct)
			_SET(GRAPH,isGraph)
			_SET(PRINT,isPrint)
			_SET(XDIGIT,isXDigit)
			_SET(BLANK,isBlank)
		}
#undef _SET

		//NULL is a control character
		//reset its bit anyway, we don't want to match a NULL character
		g_symSetTable[CNTRL]->reset(0);
	}
}

RegexMatcherImpl::RegexMatcherImpl(const char *regex, Alphabet *alphabet) {
	if(!alphabet)
		alphabet = &defaultAlphabet;
	createDefaultSymSets(alphabet);
	NFA *nfa = new NFA(*alphabet);	
	nfa->buildFromRegex(regex);
	DFA *dfa = nfa->toDFA();
	m_nfa = nfa;
	m_dfa = dfa;
}

RegexMatcherImpl::~RegexMatcherImpl() {
	if(m_nfa)
		delete m_nfa;
	if(m_dfa)
		delete m_dfa;
}

bool RegexMatcherImpl::match(const char *input) {
	const char *c = input;
	int curState = 0;
	m_dfa->reset();
	while(*c) {
		curState = m_dfa->step((int)*c);
		if(curState < 0)
			return false;
		++c;
	}

	return m_dfa->state(curState)->isFinal();
}

bool RegexMatcherImpl::matchPartially(const char *input, int *pMatchLength) {
	const char *c = input;
	int lastState;
	int curState;
	int lastAcceptingState = -1;
	int matchLength = 0;
	
	curState = 0;
	m_dfa->reset();
	c = input;
	while(*c) {
		lastState = curState;
		curState = m_dfa->step((int)*c);
		if(curState < 0)
			break;
		++c;

		if(m_dfa->state(curState)->isFinal()) {
			lastAcceptingState = curState;
			matchLength = c - input;
			if(m_dfa->state(curState)->flags() == 1) //1 = lazy (non-greedy)
				break;
		}
	}

	if(pMatchLength)
		*pMatchLength = matchLength;

	if(lastAcceptingState >= 0) {
		return true;
	}

	return false;
}

bool RegexMatcherImpl::contains(const char *input, int *pStartPos, int *pEndPos) {
	const char *c = input;
	int lastState;
	int curState;
	int lastAcceptingState;
	int startPos = 0;
	int endPos = 0;
	while(input[startPos]) {
		curState = 0;
		lastAcceptingState = -1;
		c = &input[startPos];
		while(*c) {
			lastState = curState;
			curState = m_dfa->step((int)*c);
			if(curState < 0)
				break;
			++c;
			if(m_dfa->state(curState)->isFinal()) {
				lastAcceptingState = curState;
				endPos = c - input;
				if(m_dfa->state(curState)->flags() == 1)
					break;
			}
		}

		if(lastAcceptingState >= 0) {
			if(pStartPos)
				*pStartPos = startPos;
			if(pEndPos)
				*pEndPos = endPos;
			return true;
		}

		++startPos;
	}

	return false;
}

RegexMatcher* createRegexMatcher(const char* regex, Alphabet* alphabet) {
	return new RegexMatcherImpl(regex, alphabet);
}