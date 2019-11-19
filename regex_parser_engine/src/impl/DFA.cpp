
#include "NFA.h"
#include "DFA.h"

void DFA::buildFromNFA(const NFA *nfa)
{
	State *initialState = nfa->initialState();
	if(initialState) {
		StateSet closure = eclosure(initialState);
		buildMove(nfa, closure);
		buildTransTable();
	}
}

State *DFA::findState(StateSet &closure)
{
	std::string str = closure.toString();

	for(size_t i = 0; i < m_states.size(); ++i) {
		if(m_states[i]->strIdentifier() == str)
			return m_states[i];
	}
	return NULL;
}

int DFA::step(int sym)
{
	return m_curStateNum = m_transTable[m_curStateNum][sym];
}

DFA::DFA(const NFA *nfa)
	:FSM(*nfa->alphabet())
{
	buildFromNFA(nfa);
}