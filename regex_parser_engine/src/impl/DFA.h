#pragma once

#ifndef __DFA_H__
#define __DFA_H__

#include "FSM.H"

class NFA;

class DFA : public FSM
{
	//current parsing input; Move to FSM class if it or NFA implement parsing methods
	const char *m_curInput;

	int **m_transTable;
	void buildTransTable();

	void eclosure(StateSet &curSet, State *state);
	StateSet eclosure(State *state);
	State *newStateFromClosure(StateSet &closure);
	State *buildMove(const NFA *nfa, StateSet &closure);
	State *findState(StateSet &closure);

public:
	DFA(const NFA *nfa);
	void buildFromNFA(const NFA *nfa);

	//override
	virtual int step(int sym);
};

#endif