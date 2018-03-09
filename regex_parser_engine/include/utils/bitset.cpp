#include "bitset.h"

void Bitset::_deleteBlocks() {
	if (m_blocks && m_blocks != m_smallBitset) {
		delete[] m_blocks;
	}
}

void Bitset::_trim() {
	int pad = m_bitCount % BITS_PER_BLOCK;
	if (pad != 0)
		m_blocks[m_blockCount - 1] &= (1 << pad) - 1;
}

void Bitset::_set(int pos, bool state) {
	int block = pos / BITS_PER_BLOCK;
	int _pos = pos % BITS_PER_BLOCK;
	if (state)
		m_blocks[block] |= 1 << _pos;
	else
		m_blocks[block] &= ~(1 << _pos);
}

bool Bitset::_test(int pos) {
	int block = pos / BITS_PER_BLOCK;
	int _pos = pos % BITS_PER_BLOCK;
	return (m_blocks[block] & (1 << _pos)) != 0;
}

Bitset::Bitset() {
	m_bitCount = SMALL_BITSET_BITCOUNT;
	m_blockCount = SMALL_BITSET_BLOCKCOUNT;
	m_blockSize = BYTES_PER_BLOCK;
	m_totalBufferSize = m_blockCount * BYTES_PER_BLOCK;
	m_blocks = m_smallBitset;
	memset(m_blocks, 0, m_totalBufferSize);
}

Bitset::Bitset(unsigned int bitCount) {
	m_bitCount = bitCount;
	m_blockCount = (bitCount + (BITS_PER_BLOCK - 1)) / BITS_PER_BLOCK;
	m_blockSize = BYTES_PER_BLOCK;
	m_totalBufferSize = m_blockCount * BYTES_PER_BLOCK;
	if (bitCount <= SMALL_BITSET_BITCOUNT) {
		m_blocks = m_smallBitset;
	}
	else {
		m_blocks = new BlockType[m_blockCount];
	}
	memset(m_blocks, 0, m_totalBufferSize);
}

Bitset::Bitset(const Bitset &bitset) {
	int bitCount = bitset.m_bitCount;
	m_bitCount = bitset.m_bitCount;
	m_blockCount = bitset.m_blockCount;
	m_blockSize = bitset.m_blockSize;
	m_totalBufferSize = bitset.m_totalBufferSize;
	if (bitCount <= SMALL_BITSET_BITCOUNT) {
		m_blocks = m_smallBitset;
	}
	else {
		m_blocks = new BlockType[m_blockCount];
	}
	memcpy(m_blocks, bitset.m_blocks, m_totalBufferSize);
}

Bitset::~Bitset() {
	_deleteBlocks();
}

void Bitset::resize(unsigned int bitCount) {
	if (m_bitCount == bitCount)
		return;

	unsigned int oldBufferSize = m_totalBufferSize;
	unsigned int oldBitCount = bitCount;

	m_bitCount = bitCount;
	m_blockCount = (bitCount + (BITS_PER_BLOCK - 1)) / BITS_PER_BLOCK;
	m_blockSize = BYTES_PER_BLOCK;
	m_totalBufferSize = m_blockCount * BYTES_PER_BLOCK;

	if (m_totalBufferSize > oldBufferSize) {

		if (bitCount > BITS_PER_BLOCK) {
			BlockType *source;
			BlockType *target;
			source = m_blocks;
			target = new BlockType[m_blockCount];
			memcpy(target, source, oldBufferSize);
			_deleteBlocks();
			m_blocks = target;
		}
		memset(((unsigned char*)m_blocks) + oldBufferSize, 0, m_totalBufferSize - oldBufferSize);

	}
	else {
		memset(((unsigned char*)m_blocks) + m_totalBufferSize, 0, oldBufferSize - m_totalBufferSize);
	}

	_trim();
}

Bitset& Bitset::set(bool state) {
	memset(m_blocks, state ? ~0UL : 0, m_totalBufferSize);
	_trim();
	return *this;
}

Bitset& Bitset::set(int pos, bool state) {
	_set(pos, state);
	return *this;
}

Bitset& Bitset::set(int pos[], unsigned int count, bool state) {
	unsigned int i = 0;
	while (i < count) {
		_set(pos[i], state);
		++i;
	}
	return *this;
}

Bitset& Bitset::reset() {
	memset(m_blocks, 0, m_totalBufferSize);
	return *this;
}

Bitset& Bitset::reset(int pos) {
	_set(pos, false);
	return *this;
}

Bitset& Bitset::reset(int pos[], unsigned int count) {
	unsigned int i = 0;
	while (i < count) {
		_set(pos[i], 0);
		++i;
	}
	return *this;
}

Bitset& Bitset::flip() {
	unsigned int i = 0;
	while (i < m_blockCount) {
		m_blocks[i] = ~m_blocks[i];
		++i;
	}
	_trim();
	return *this;
}

Bitset& Bitset::flip(int pos) {
	int block = pos / BITS_PER_BLOCK;
	int _pos = pos % BITS_PER_BLOCK;
	m_blocks[block] ^= 1 << _pos;
	return *this;
}

bool Bitset::test(int pos) {
	return _test(pos);
}

bool Bitset::test(int pos[], unsigned int count) {
	unsigned int i = 0;
	if (count == 0)
		return false;
	do {
		if (!_test(pos[i]))
			return false;
		++i;
	} while (i < count);
	return true;
}

bool Bitset::any() {
	unsigned int i = 0;
	while (i < m_blockCount) {
		if (m_blocks[i] != BLOCK_RESET)
			return true;
		++i;
	}
	return false;
}

bool Bitset::none() {
	unsigned int i = 0;
	while (i < m_blockCount) {
		if (m_blocks[i] != BLOCK_RESET)
			return false;
		++i;
	}
	return true;
}

bool Bitset::all() {
	unsigned int i = m_blockCount - 1;
	int remainingBits = m_bitCount % BITS_PER_BLOCK;
	if (remainingBits > 0) {
		if (m_blocks[i] != (1 << remainingBits) - 1)
			return false;
		--i;
	}

	while (i >= 0) {
		if (m_blocks[i] != BLOCK_SET)
			return false;
		--i;
	}
	return true;
}

bool Bitset::operator==(const Bitset& right) {
	unsigned int i;

	if (right.m_bitCount < m_bitCount)
		return ((Bitset&)right).operator==(*this);

	for (i = 0; i < m_blockCount; ++i) {
		if (m_blocks[i] != right.m_blocks[i])
			return false;
	}

	int trimBits = m_bitCount % BITS_PER_BLOCK;
	if (trimBits != 0) {
		if (m_blocks[m_blockCount - 1] != (right.m_blocks[m_blockCount-1]&((1 << trimBits) - 1)))
			return false;
	}

	for (; i < right.m_blockCount; ++i) {
		if (right.m_blocks[i])
			return false;
	}
	return true;
}

bool Bitset::operator!=(const Bitset& right) {
	return !this->operator==(right);
}

Bitset& Bitset::operator &=(const Bitset& right) {
	unsigned int i = 0;
	unsigned int blockCount = right.m_blockCount < m_blockCount ? right.m_blockCount : m_blockCount;
	while (i < blockCount) {
		m_blocks[i] &= right.m_blocks[i];
		++i;
	}
	return *this;
}

Bitset& Bitset::operator |=(const Bitset& right) {
	unsigned int i = 0;
	unsigned int blockCount = right.m_blockCount < m_blockCount ? right.m_blockCount : m_blockCount;
	while (i < blockCount) {
		m_blocks[i] |= right.m_blocks[i];
		++i;
	}
	_trim();
	return *this;
}

Bitset& Bitset::operator ^=(const Bitset& right) {
	unsigned int i = 0;
	unsigned int blockCount = right.m_blockCount < m_blockCount ? right.m_blockCount : m_blockCount;
	while (i < blockCount) {
		m_blocks[i] ^= right.m_blocks[i];
		++i;
	}
	_trim();
	return *this;
}

Bitset Bitset::operator~() {
	return Bitset(*this).flip();
}

Bitset& Bitset::operator<<=(unsigned int pos) {
	unsigned int shiftSize = pos / BITS_PER_BLOCK;
	if (shiftSize > 0) {
		for (unsigned int i = m_blockCount - 1; i >= 0; --i)
			m_blocks[i] = shiftSize <= i ? m_blocks[i - shiftSize] : 0;
	}

	int remainingBits = pos % BITS_PER_BLOCK;
	if (remainingBits > 0) {
		unsigned int i;
		for (i = m_blockCount - 1; i > 0; --i)
			m_blocks[i] = (m_blocks[i] << remainingBits) | (m_blocks[i - 1] >> (BITS_PER_BLOCK - remainingBits));
		m_blocks[i] <<= remainingBits;
	}
	_trim();
	return *this;
}

Bitset& Bitset::operator>>=(unsigned int pos) {
	unsigned int shiftSize = pos / BITS_PER_BLOCK;
	if (shiftSize > 0) {
		for (unsigned int i = 0; i < m_blockCount; ++i) {
			m_blocks[i] = shiftSize < m_blockCount - i ? m_blocks[i + shiftSize] : 0;
		}
	}

	int remainingBits = pos % BITS_PER_BLOCK;
	if (remainingBits > 0) {
		unsigned int i;
		for (i = 0; i < m_blockCount - 1; ++i)
			m_blocks[i] = (m_blocks[i] >> remainingBits) | (m_blocks[i + 1] << (BITS_PER_BLOCK - remainingBits));
		m_blocks[i] >>= remainingBits;
	}

	return *this;
}

Bitset& Bitset::operator=(const Bitset& right) {
	memcpy(m_blocks, right.m_blocks, right.m_blockCount < m_blockCount ? right.m_totalBufferSize : m_totalBufferSize);
	_trim();
	return *this;
}

std::string Bitset::toString() {
	std::string ret;

	int i = m_blockCount - 1;
	int remainingBits = m_bitCount % BITS_PER_BLOCK;
	if (remainingBits > 0) {
		for (int j = remainingBits - 1; j >= 0; --j) {
			int v = m_blocks[m_blockCount - 1] & 1;
			if (m_blocks[m_blockCount - 1] & (1 << j))
				ret += '1';
			else
				ret += '0';
		}
		--i;
	}

	while (i >= 0) {
		unsigned long block = m_blocks[i];
		for (int j = BITS_PER_BLOCK - 1; j >= 0; --j) {
			int v = block & 1;
			if (block & (1 << j))
				ret += '1';
			else
				ret += '0';
		}
		--i;
	}

	return ret;
}
