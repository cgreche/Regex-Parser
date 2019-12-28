
#include "NFA.h"
#include "RegexParser.h"

BitsetList g_symSetTable;

//todo: find the best way to free the bitsets
int FSM::insertSymSetIfNew(Bitset &symSet)
{
	int i;
	for(i = 0; i < g_symSetTable.size(); ++i) {
		if(g_symSetTable[i] && *g_symSetTable[i] == symSet) {
			return i;
		}
	}

	Bitset *newSymSet = new Bitset(symSet);
	*newSymSet = symSet;
	g_symSetTable.push_back(newSymSet);
	return i;
}

State *FSM::newState()
{
	State *state = new State();
	state->m_num = m_states.size();
	m_states.push_back(state);
	return state;
}

FSM::FSM(Alphabet &alphabet)
	: m_alphabet(&alphabet)
{
	m_initialState = NULL;
}

State *FSM::findState(int intIdentifier)
{
	//todo: change to hash table futurely
	for(size_t i = 0; i < m_states.size(); ++i) {
		if(m_states[i]->m_intIdentifier == intIdentifier)
			return m_states[i];
	}
	return NULL;
}

State *FSM::findState(const char *strIdentifier)
{
	for(size_t i = 0; i < m_states.size(); ++i) {
		if(m_states[i]->m_strIdentifier == strIdentifier)
			return m_states[i];
	}
	return NULL;
}

void FSM::reset()
{
	m_curStateNum = 0;
}

int FSM::step(int sym)
{
	return -1;
}
