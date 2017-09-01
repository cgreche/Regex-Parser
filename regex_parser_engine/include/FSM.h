#pragma once

#ifndef __FSM_H__
#define __FSM_H__

#include <vector>
#include <string>
#include "utils/bitset.h"
#include "Alphabet.h"
#include "State.h"

typedef std::vector<Bitset *> BitsetList;
extern BitsetList g_symSetTable;

class FSM
{
protected:
	Alphabet *m_alphabet;
	std::vector<State*> m_states;
	State *m_initialState;
	StateSet m_finalStates;

	State *newState();
	//running state
	int m_curStateNum;

	//
	int insertSymSetIfNew(Bitset &symSet);

public:
	FSM(Alphabet &alphabet);

	State *findState(int intIdentifier);
	State *findState(const char *strIdentifier);

	void reset();
	virtual int step(int sym);

	inline Alphabet *alphabet() const { return m_alphabet; }
	inline int currentStateNumber() const { return m_curStateNum; }
	inline State* state(int num) const { return m_states[num]; }
	inline int stateCount() const { return m_states.size(); }
	inline State* initialState() const { return m_initialState; }
};

#endif