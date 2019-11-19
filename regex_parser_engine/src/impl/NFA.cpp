
#include "DFA.h"
#include "NFA.h"
#include "RegexParser.h"

/////
State *NFA::_buildFromRegexNodeBracketElems(State *initialState, BracketElem* elemList, bool negateElem)
{
	Bitset bitset;
	int sym;
	
	BracketElem* curElem = elemList;
	while(curElem) {
		int type = curElem->type();
		if(type == BET_COLL_ELEM_SINGLE) { //collating/literal (one symbol)
			sym = curElem->symStart()->sym() & 0xffff;
			bitset.set(sym);
		}
		else if(type == BET_COLL_ELEM_MULTI) {
			//to-do
		}
		else if(type == BET_RANGE) { //collating/literal (range)
			BracketElemAtom *start = curElem->symStart();
			BracketElemAtom *end = curElem->symEnd();
			int rangeCount = end->sym() - start->sym() + 1;
			sym = start->sym() & 0xffff;
			for(int i = 0; i < rangeCount; ++i) {
				bitset.set(sym + i);
			}
		}
		else if(type == BET_EQUIVALENCE_CLASS) { //equivalence class
			//to-do
		}
		else if(type == BET_CHARACTER_CLASS) { //character class
			int classId = curElem->classId();
			bitset |= *g_symSetTable[classId];
		}

		curElem = curElem->next();
	}

	if(negateElem)
		bitset.flip();

	int setId = insertSymSetIfNew(bitset);
	sym = 0x01000000 | (setId & 0xffff);
	
	State *finalSt = newState();
	initialState->addTransition(sym, finalSt);
	return finalSt;
}

State *NFA::_buildFromRegexNode(State *initialState, RegexNode *node)
{
	State *finalState = NULL;
	RegexNodeType type = node->type();
	switch(type) {
		case RETP_LITERAL:
		{
			int sym = ((RegexLiteral*)node)->sym();
			if(sym == SYM_ANY) {
				sym = 0x01000000 | 30;
			}
			finalState = newState();
			initialState->addTransition(sym, finalState);
			break;
		}

		case RETP_CONCAT:
		{
			RegexNode *left = ((RegexConcat*)node)->left();
			RegexNode *right = ((RegexConcat*)node)->right();
			
			State *st = _buildFromRegexNode(initialState, left);
			finalState = _buildFromRegexNode(st, right);
			break;
		}

		case RETP_ALTERNATION:
		{
			RegexNode *left = ((RegexAlternation*)node)->left();
			RegexNode *right = ((RegexAlternation*)node)->right();

			State *initialpath = newState();
			State *finalpath1 = _buildFromRegexNode(initialpath, left);
			initialState->addTransition(SYM_E, initialpath);

			initialpath = newState();
			State *finalpath2 = _buildFromRegexNode(initialpath, right);
			initialState->addTransition(SYM_E, initialpath);

			//link paths to a common state
			State *finalSt = newState();
			finalpath1->addTransition(SYM_E,finalSt);
			finalpath2->addTransition(SYM_E,finalSt);
			return finalSt;
		}
		case RETP_QUANTIFIER:
		{
			RegexNode *expression = (((RegexQuantifier*)node)->expression());
			int min = ((RegexQuantifier*)node)->min();
			int max = ((RegexQuantifier*)node)->max();
			bool lazy = ((RegexQuantifier*)node)->isLazy();

			if(min < 2) {
				if(max == QUANT_ANY) {
					//cases: *, +, {0,}, {1,}
					State *newSt = newState();
					State *st = _buildFromRegexNode(newSt, expression);
					finalState = newState();
					st->addTransition(SYM_E, newSt);
					st->addTransition(SYM_E, finalState);
					initialState->addTransition(SYM_E, newSt);
					if(min == 0)
						initialState->addTransition(SYM_E, finalState);

					//todo: set a proper lazy flag instead of just assigning a boolean to m_flags which is long
					finalState->setFlags(lazy);
				}
				else {
					int n = min;
					State *newSt = initialState;
					while(n > 0) {
						newSt = _buildFromRegexNode(newSt, expression);
						--n;
					}

					n = max - min;
					while(n > 0) {
						State *newSt2 = _buildFromRegexNode(newSt, expression);
						newSt->addTransition(SYM_E, newSt2);
						newSt = newSt2;
						--n;
					}
					finalState = newSt;
				}
			}
			else {
				if(max == QUANT_ANY) {
					int n = min-1;
					State *newSt = initialState;
					while(n > 0) {
						newSt = _buildFromRegexNode(newSt, expression);
						--n;
					}

					State *st = _buildFromRegexNode(newSt, expression);
					st->addTransition(SYM_E,newSt);
					finalState = newState();
					st->addTransition(SYM_E, finalState);
				}
				else {
					int n = min;
					State *newSt = initialState;
					while(n > 0) {
						newSt = _buildFromRegexNode(newSt, expression);
						--n;
					}

					n = max - min;
					while(n > 0) {
						State *newSt2 = _buildFromRegexNode(newSt, expression);
						newSt->addTransition(SYM_E, newSt2);
						newSt = newSt2;
						--n;
					}
					finalState = newSt;
				}
			}

			break;
		}

		case RETP_BRACKET_EXPR:
		{
			BracketElem *elem = ((RegexBracketExp *)node)->firstElem();
			finalState = _buildFromRegexNodeBracketElems(initialState,elem,((RegexBracketExp *)node)->negateElems());
			break;
		}

		default:
			printf("Invalid node type\n");
	}

	return finalState;
}

NFA::NFA(Alphabet &alphabet)
	: FSM(alphabet)
{
}

void NFA::buildFromRegex(const char *regex)
{
	State *startingState = newState();
	RegexParser parser;
	try {
		RegexNode *tree = parser.parse(regex);
		if(tree) {
			State *state = _buildFromRegexNode(startingState, tree);
			state->setFinal(true);
			delete tree;
		}

		/* the commented lines are used to link 2 machines
		if(m_initialState) {
			State *newStartingState = newState();
			newStartingState->addTransition(SYM_E, m_initialState);
			newStartingState->addTransition(SYM_E, startingState);
			m_initialState = newStartingState;
		}
		else
		*/
		m_initialState = startingState;
	}
	catch(const char* msg) {
		printf(msg);
		printf("\n");
	}
}

DFA* NFA::toDFA()
{
	return new DFA(this);
}

////
////
////

void DFA::eclosure(StateSet &curSet, State *state)
{
	int i;
	curSet.insert(state->num());
	for(i = 0; i < state->transitionCount(); ++i) {
		if(state->transitionSymbol(i) == SYM_E) {
			//only insert if new
			if(!curSet.contains(state->transitionState(i)->num()))
				eclosure(curSet, state->transitionState(i));
		}
	}
}

StateSet DFA::eclosure(State *state)
{
	StateSet _set;
	eclosure(_set,state);
	return _set;
}

State *DFA::newStateFromClosure(StateSet &closure)
{
	State *state = newState();
	state->setStrIdentifier(closure.toString().c_str());
	return state;
}

void DFA::buildTransTable()
{
	int i, j;
	int charCount = 256;
	int stateCount = m_states.size();
	m_transTable = new int*[stateCount];
	for(i = 0; i < stateCount; ++i) {
		m_transTable[i] = new int[charCount];
		memset(m_transTable[i], -1, charCount);
	}

	for(i = 0; i < stateCount; ++i) {
		State *state = m_states[i];
		for(j = 0; j < state->transitionCount(); ++j) {
			int transSym = state->transitionSymbol(j);
			int symType = transSym & 0xff000000;
			int targetSym = transSym & 0xffff;
			if(symType == 0x01000000) {
				Bitset &symSet = *g_symSetTable[targetSym];
				for(int k = 0; k < symSet.size(); ++k) {
					if(symSet.test(k)) {
						m_transTable[i][k] = state->transitionState(j)->num();
					}
				}
			}
			else {
				m_transTable[i][targetSym] = state->transitionState(j)->num();
			}
		}
	}
}

State *DFA::buildMove(const NFA *nfa, StateSet &closure)
{
	//auxiliar struct to map a symbol to a list of states
	struct SymMap
	{
		Bitset symSet;
		std::vector<int> states;
	};

	int i, j, k;
	std::vector<SymMap> symMap;
	int symAnyMapIndex = -1;
	StateSet symAnyClosure;

	bool isFinal = false;
	int flags = 0;

	//create DFA state
	State *dfaState = newStateFromClosure(closure);

	//get a set of all accepted symbols from this state
	Bitset accepts;
	for(StateSet::iterator it = closure.begin(); it != closure.end(); ++it) {
		int stateNum = *it;
		if(stateNum < 0) {
			continue;
		}

		State *nfaState = nfa->state(*it);
		if(nfaState->isFinal()) {
			isFinal = true;
		}
		flags |= nfaState->flags();

		//Let's build a set of states that will be followed by each unique transition symbol
		for(j = 0; j < nfaState->transitionCount(); ++j) {
			int sym = nfaState->transitionSymbol(j);
			if(sym != SYM_E) {
				if((sym & 0xff000000) == 0x01000000) {
					//set of elems
					int setId = sym & 0xffff;
					accepts = *g_symSetTable[setId];
				}
				else {
					//single elem
					accepts.set(sym & 0xffff);
				}

				int k;
				for(k = 0; k < symMap.size(); ++k) {
					Bitset &dest = symMap[k].symSet;
					Bitset intersect = accepts & dest;

					if(intersect.none())
						continue;

					Bitset remains = ~accepts & dest;
					accepts = accepts & ~dest;

					if(remains.any()) {
						//Update remains set
						//No new target states needed
						symMap[k].symSet = remains;
#if 0
						printf("RE: ");
						for(int l = 0; l < remains.size(); ++l) {
							if(remains.test(l)) {
								printf("%c,", l);
							}
						}
						printf("\n");
#endif

						//For intersection set, create a new transiction and merge the states
						//in symMap
						SymMap sm;
						sm.symSet = intersect;
						sm.states.push_back(nfaState->transitionState(j)->num());
						for(int l = 0; l < symMap[k].states.size(); ++l) {
							sm.states.push_back(symMap[k].states[l]);
						}
#if 0
						printf("IN: ");
						for(int l = 0; l < intersect.size(); ++l) {
							if(intersect.test(l)) {
								printf("%c,", l);
							}
						}
						printf("\n");
#endif
						symMap.push_back(sm);
					}
					else {
						symMap[k].states.push_back(nfaState->transitionState(j)->num());
					}
					
					if(accepts.none())
						break;
				}

				if(accepts.any()) {
					SymMap sm;
					sm.symSet = accepts;
#if 0
					printf("NE: ");
					for(int l = 0; l < accepts.size(); ++l) {
						if(accepts.test(l)) {
							printf("%c,", l);
						}
					}
					printf("\n");
#endif
					sm.states.push_back(nfaState->transitionState(j)->num());
					symMap.push_back(sm);
					
					accepts.reset();
				}
			}
		}
	}
#if 0
	//debugging
	for(i = 0; i < symMap.size(); ++i) {
		printf("(Dest %d) Symbols: ",i);
		Bitset &symSet = symMap[i].symSet;
		for(j = 0; j < symSet.size(); ++j) {
			if(symSet.test(j)) {
				printf("%c", j);
			}
		}
		printf(" - ");
		for(j = 0; j < symMap[i].states.size(); ++j) {
			printf("%d,", symMap[i].states[j]);
		}
		printf("\n");
	}
#endif

	//Now, for each unique symbol/set, add a transition to a new state (grouped by the e-closure of each symbol/set destination states)
	for(k = 0; k < symMap.size(); ++k) {
		StateSet newClosure;
		for(int l = 0; l < symMap[k].states.size(); ++l) {
			eclosure(newClosure, nfa->state(symMap[k].states[l]));
		}

		State *newDfaState = findState(newClosure);
		if(!newDfaState) {
			newDfaState = buildMove(nfa,newClosure);
		}

		int setId = insertSymSetIfNew(symMap[k].symSet);
		dfaState->addTransition(0x01000000|setId, newDfaState);
	}

	dfaState->setFinal(isFinal);
	dfaState->setFlags(flags);
	return dfaState;
}

