#pragma once

#ifndef __ASCII_ALPHABET_H__
#define __ASCII_ALPHABET_H__

#include "../Alphabet.h"

//Char flags
#define CF_UPPER 0x00000001
#define CF_LOWER 0x00000002
#define CF_ALPHA 0x00000004
#define CF_DIGIT 0x00000008
#define CF_ALNUM 0x00000010
#define CF_SPACE 0x00000020
#define CF_CNTRL 0x00000040
#define CF_PUNCT 0x00000080
#define CF_GRAPH 0x00000100
#define CF_PRINT 0x00000200
#define CF_XDIGIT 0x00000400
#define CF_BLANK 0x00000800

class AsciiAlphabet : public Alphabet
{
	static const unsigned int asciiCharFlagsTable[0x80];

	inline bool isType(int c, int flags) { return (asciiCharFlagsTable[c] & flags) != 0; }

public:
	virtual bool isUpper(int c) { return isType(c,CF_UPPER); }
	virtual bool isLower(int c) { return isType(c,CF_UPPER);  }
	virtual bool isAlpha(int c) { return isType(c,CF_ALPHA);  }
	virtual bool isDigit(int c) { return isType(c,CF_DIGIT);  }
	virtual bool isAlnum(int c) { return isType(c,CF_ALNUM); }
	virtual bool isSpace(int c) { return isType(c,CF_SPACE);  }
	virtual bool isCntrl(int c) { return isType(c,CF_CNTRL);  }
	virtual bool isPunct(int c) { return isType(c,CF_PUNCT);  }
	virtual bool isGraph(int c) { return isType(c,CF_GRAPH); }
	virtual bool isPrint(int c) { return isType(c,CF_PRINT);  }
	virtual bool isXDigit(int c) { return isType(c,CF_XDIGIT);  }
	virtual bool isBlank(int c) { return isType(c,CF_BLANK);  }

	virtual bool isValid(int c) { return c > 0 && c < 0x80; }
	virtual unsigned int characterCount() { return 0x80; }
};

#endif