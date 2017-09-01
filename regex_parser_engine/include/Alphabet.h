#pragma once

#ifndef __ALPHABET_H__
#define __ALPHABET_H__

class Alphabet
{
public:
	virtual bool isUpper(int c) = 0;
	virtual bool isLower(int c) = 0;
	virtual bool isAlpha(int c) = 0;
	virtual bool isDigit(int c) = 0;
	virtual bool isAlnum(int c) = 0;
	virtual bool isSpace(int c) = 0;
	virtual bool isCntrl(int c) = 0;
	virtual bool isPunct(int c) = 0;
	virtual bool isGraph(int c) = 0;
	virtual bool isPrint(int c) = 0;
	virtual bool isXDigit(int c) = 0;
	virtual bool isBlank(int c) = 0;
	virtual int char2sym(int c) = 0;
	virtual bool isValid(int c) = 0;
};

#endif