#pragma once

#ifndef __STATE_H__
#define __STATE_H__

#include <vector>
#include <set>
#include <string>

class State;

struct Transition
{
	int sym;
	State *state;
};

class State
{
	friend class FSM;

	int m_num;
	int m_intIdentifier;
	std::string m_strIdentifier;
	std::vector<Transition> m_transition; //in a Thompson NFA, there are at most 2 transitions
	bool m_final;

	long m_flags;

	State() {
		m_final = false;
		m_flags = 0;
	}

public:

	inline void addTransition(int sym, State* state) { m_transition.push_back({ sym,state }); }
	inline void setFinal(bool isFinal) { m_final = isFinal; }	
	inline void setStrIdentifier(const char *identifier) { m_strIdentifier = identifier; }
	inline void setFlags(long flags) { m_flags = flags; }

	inline int num() const { return m_num; }
	inline bool isFinal() const { return m_final; }
	inline int intIdentifier() const { return m_intIdentifier; }
	inline const std::string& strIdentifier() const { return m_strIdentifier; }
	inline int transitionCount() const { return m_transition.size(); }
	inline int transitionSymbol(int index) const { return m_transition[index].sym; }
	inline State *transitionState(int index) const { return m_transition[index].state; }
	inline long flags() const { return m_flags; }

	bool operator==(State &right) {
		return this->m_strIdentifier == right.m_strIdentifier;
	}
};

class StateSet
{
	std::set<int> m_states;

public:
	typedef typename std::set<int>::iterator iterator;
	typedef typename std::set<int>::const_iterator const_iterator;
	typedef typename std::set<int>::reverse_iterator reverse_iterator;
	typedef typename std::set<int>::const_reverse_iterator const_reverse_iterator;
	inline StateSet::iterator begin() { return m_states.begin(); }
	inline StateSet::iterator end() { return m_states.end(); }
	inline StateSet::const_iterator begin() const { return m_states.begin(); }
	inline StateSet::const_iterator end() const { return m_states.end(); }
	inline StateSet::reverse_iterator rbegin() { return m_states.rbegin(); }
	inline StateSet::reverse_iterator rend() { return m_states.rend(); }
	inline StateSet::const_reverse_iterator rbegin() const { return m_states.rbegin(); }
	inline StateSet::const_reverse_iterator rend() const { return m_states.rend(); }

	inline void insert(int stateNumber) { m_states.insert(stateNumber); }
	inline void insert(const StateSet &stateSet) { m_states.insert(stateSet.begin(),stateSet.end()); }
	inline bool contains(int stateNumber) { return m_states.find(stateNumber) != m_states.end(); }
	inline int stateCount() const { return m_states.size(); }

	std::string toString()
	{
		std::string str = "{";
		if(stateCount() > 0) {
			char snum[20];
			std::set<int>::iterator it, end;
			it = m_states.begin();
			end = m_states.end();

			sprintf(snum,"%d",*it);
			str += snum;

			++it;
			for(; it != end; ++it) {
				sprintf(snum,"%d",*it);
				str += ",";
				str += snum;
			}
		}
		str += "}";

		return str;
	}
};

#endif