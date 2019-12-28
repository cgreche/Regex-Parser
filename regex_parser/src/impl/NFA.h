#pragma once

#ifndef __NFA_H__
#define __NFA_H__

#include "FSM.h"

#include <list>

#define SYM_E 0x80000001

class RegexNode;
class BracketElem;

class DFA;

class NFA : public FSM
{
	//to-do - implement NFA automaton
	std::list<int> m_curStates;
	int m_onStateCount;
	//end of to-do

protected:
	State *_buildFromRegexNodeBracketElems(State *curState, BracketElem *elemList, bool negateElem);
	State *_buildFromRegexNode(State *curState, RegexNode *node);

public:
	NFA(Alphabet &alphabet);
	void buildFromRegex(const char *regex);
	DFA *toDFA();

	inline Alphabet *alphabet() const { return m_alphabet; }
	inline int stateCount() const { return m_states.size(); }
	inline State* state(int index) const { return m_states[index]; }
};

#endif