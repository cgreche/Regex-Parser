#pragma once

#ifndef __BITSET_H__
#define __BITSET_H__

#include <string>

typedef unsigned long BlockType;
#define BYTES_PER_BLOCK sizeof(BlockType)
#define BITS_PER_BLOCK (BYTES_PER_BLOCK*8)
#define SMALL_BITSET_BITCOUNT 0x80
#define SMALL_BITSET_BLOCKCOUNT (SMALL_BITSET_BITCOUNT/BITS_PER_BLOCK)

#define BLOCK_SET (~0UL)
#define BLOCK_RESET (0UL)

class Bitset
{
	BlockType m_smallBitset[SMALL_BITSET_BLOCKCOUNT];
	unsigned int m_bitCount;
	unsigned int m_blockCount;
	unsigned int m_blockSize;
	unsigned int m_totalBufferSize;
	BlockType *m_blocks;

	void _deleteBlocks();
	void _set(int bit, bool state);
	bool _test(int bit);

	void _trim();
public:
	Bitset();
	Bitset(unsigned int bitCount);
	Bitset(const Bitset &bitset);  // copy constructor

	~Bitset();

	void resize(unsigned int bitCount);

	Bitset& set(bool state = true);
	Bitset& set(int pos, bool state = true);
	Bitset& set(int pos[], unsigned int count, bool state = true);
	Bitset& reset();
	Bitset& reset(int pos);
	Bitset& reset(int pos[], unsigned int count);
	Bitset& flip();
	Bitset& flip(int pos);

	bool test(int pos);
	bool test(int pos[], unsigned int count);
	bool any();
	bool none();
	bool all();

	inline size_t size() const { return m_bitCount; }

	bool operator==(const Bitset& right);
	bool operator!=(const Bitset& right);
	Bitset& operator &=(const Bitset& right);
	Bitset& operator |=(const Bitset& right);
	Bitset& operator ^=(const Bitset& right);
	Bitset& operator<<=(unsigned int pos);
	Bitset& operator>>=(unsigned int pos);
	Bitset& operator=(const Bitset& right);

	Bitset operator~();

	friend Bitset operator&(const Bitset &left, const Bitset &right);
	friend Bitset operator|(const Bitset &left, const Bitset &right);
	friend Bitset operator^(const Bitset &left, const Bitset &right);

	std::string toString();

};

inline Bitset operator&(const Bitset &left, const Bitset &right) {
	Bitset ret(left);
	ret &= right;
	return ret;
}

inline Bitset operator|(const Bitset &left, const Bitset &right) {
	Bitset ret(left);
	ret |= right;
	return ret;
}

inline Bitset operator^(const Bitset &left, const Bitset &right) {
	Bitset ret = Bitset(left);
	ret ^= right;
	return ret;
}

#endif
