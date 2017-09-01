#pragma once

#ifndef __BITSET_H__
#define __BITSET_H__

#include <bitset>

class Bitset
{
	std::bitset<0x80> m_bitset;

public:
	Bitset() {

	}

	Bitset(unsigned int bitCount) {

	}

	~Bitset() {
	
	}

	void resize(unsigned int bitCount) { /*todo*/ }

	inline void set() { m_bitset.set(); }
	inline void set(unsigned int pos) { m_bitset.set(pos); }
	inline void reset() { m_bitset.reset(); }
	inline void reset(unsigned int pos) { m_bitset.reset(pos); }
	inline void flip() { m_bitset.flip(); }
	inline void flip(unsigned int pos) { m_bitset.flip(pos); }
	inline size_t size() const { return m_bitset.size(); }

	inline bool test(unsigned int pos) { return m_bitset.test(pos); }
	inline bool any() { return m_bitset.any(); }
	inline bool none() { return m_bitset.none(); }
	inline bool all() { return m_bitset.all(); }

	inline bool operator==(const Bitset& right) { return this->m_bitset == right.m_bitset; }
	inline Bitset& operator &=(const Bitset& right) {
		this->m_bitset &= right.m_bitset;
		return *this;
	}
	inline Bitset& operator |=(const Bitset& right) {
		this->m_bitset |= right.m_bitset;
		return *this;
	}
	inline Bitset& operator ^=(const Bitset& right) {
		this->m_bitset ^= right.m_bitset;
		return *this;
	}

	inline Bitset operator~() {
		Bitset ret;
		ret.m_bitset = ~m_bitset;
		return ret;
	}

	inline Bitset& operator<<=(unsigned int pos) {
		this->m_bitset <<= pos;
		return *this;
	}

	inline Bitset& operator>>=(unsigned int pos) {
		this->m_bitset >>= pos;
		return *this;
	}

};

inline Bitset operator&(const Bitset &left, const Bitset &right) {
	Bitset ret = left;
	ret &= right;
	return ret;
}

inline Bitset operator|(const Bitset &left, const Bitset &right) {
	Bitset ret = left;
	ret |= right;
	return ret;
}

#endif