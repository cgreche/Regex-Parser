#include <list>
#include <vector>
#include <set>
#include <bitset>
#include <limits>

#include <iostream>
#include <string>

#include "..\\include\\RegexParser.h"
#include "..\\include\\RegexMatcher.h"

void printNode(RegexNode *node, int level = 0)
{
	int type = node->type();
	while(level > 0) {
		printf(".");
		--level;
	}
	if(type == RETP_LITERAL) {
		printf("LITERAL(%c)\n", ((RegexLiteral*)node)->sym() == SYM_ANY ? '.' : ((RegexLiteral*)node)->sym());
	}
	else if(type == RETP_CONCAT) {
		printf("CONCATENATE\n");
	}
	else if(type == RETP_ALTERNATION) {
		printf("ALTERNATION\n");
	}
	else if(type == RETP_QUANTIFIER) {
		printf("QTD - ");
		if(((RegexQuantifier*)node)->min() == QUANT_ANY)
			printf("*");
		else
			printf("%d",((RegexQuantifier*)node)->min());
		printf("~");
		if(((RegexQuantifier*)node)->max() == QUANT_ANY)
			printf("*");
		else
			printf("%d",((RegexQuantifier*)node)->max());
		printf("\n");
	}
}

void printTree(RegexNode *tree, int level = 0)
{
	printNode(tree, level);
	RegexNodeType type = tree->type();
	RegexNode *left, *right;
	if(type == RETP_CONCAT) {
		left = ((RegexConcat*)tree)->left();
		right = ((RegexConcat*)tree)->right();
	}
	else if(type == RETP_ALTERNATION) {
		left = ((RegexAlternation*)tree)->left();
		right = ((RegexAlternation*)tree)->right();
	}
	else {
		return;
	}

	if(left)
		printTree(left,level+1);
	if(right)
		printTree(right,level+1);
}

inline void printState(State *state)
{
	if(state->isFinal())
		printf("S((%d))",state->num());
	else
		printf("S(%d)",state->num());
}

inline void printTransition(State *from, State *to, int tranSymbol)
{
	extern std::vector<Bitset *> g_symSetTable;
	printState(from);
	printf(" --");
	if((tranSymbol & 0xff000000) == 0x01000000) {
		Bitset *bitset;
		int setId = tranSymbol & 0xffff;
		bitset = g_symSetTable[setId];
		printf("[");
		int count = 0;
		for(size_t i = 1; i < bitset->size(); ++i) {
			if(bitset->test(i)) {
				if(count > 0)
					printf(",");
				printf("%c", i);
				++count;
			}
		}
		printf("]");
	}
	else {
		printf("%c", tranSymbol&0xffff);
	}
	printf("--> ");
	printState(to);
}

inline void printNFAStates(NFA *nfa)
{
	printf("Number of NFA states: %d\n", nfa->stateCount());
	printf("Initial state: %d:\n", nfa->initialState()->num());
	for(int i = 0; i < nfa->stateCount(); ++i) {
		State *state = nfa->state(i);
		bool isFinal = state->isFinal();
		int transCount = state->transitionCount();
		if(transCount > 0) {
			for(int j = 0; j < transCount; ++j) {
				printTransition(state, state->transitionState(j), state->transitionSymbol(j));
				printf("\n");
			}
		}
		else {
			printState(state);
			printf("\n");
		}
	}
}

inline void printDFAStates(DFA *dfa)
{
	printf("Number of DFA states: %d\n", dfa->stateCount());
	for(int i = 0; i < dfa->stateCount(); ++i) {
		State *state = dfa->state(i);
		bool isFinal = state->isFinal();
		int transCount = state->transitionCount();
		if(transCount > 0) {
			for(int j = 0; j < state->transitionCount(); ++j) {
				if(isFinal) {
					if(state->transitionState(j)->isFinal())
						printf("S((%s)) --%c--> ((%s))\n", state->strIdentifier().c_str(), state->transitionSymbol(j), state->transitionState(j)->strIdentifier().c_str());
					else
						printf("S((%s)) --%c--> (%s)\n", state->strIdentifier().c_str(), state->transitionSymbol(j), state->transitionState(j)->strIdentifier().c_str());
				}
				else {
					if(state->transitionState(j)->isFinal())
						printf("S(%s) --%c--> ((%s))\n", state->strIdentifier().c_str(), state->transitionSymbol(j), state->transitionState(j)->strIdentifier().c_str());
					else
						printf("S(%s) --%c--> (%s)\n", state->strIdentifier().c_str(), state->transitionSymbol(j), state->transitionState(j)->strIdentifier().c_str());
				}
			}
		}
		else {
			if(isFinal)
				printf("S((%s))\n",state->strIdentifier().c_str());
			else
				printf("S(%s)\n",state->strIdentifier().c_str());
		}
	}
}

const char *C_tokens[][2] =
{
	{ "/\\*"				, "COMMENT" },

	{ "auto"				, "AUTO" },
	{ "break"				, "BREAK" },
	{ "case"				, "CASE" },
	{ "char"				, "CHAR" },
	{ "const"				, "CONST" },
	{ "continue"			, "CONTINUE" },
	{ "default"				, "DEFAULT" },
	{ "do"					, "DO" },
	{ "double"				, "DOUBLE" },
	{ "else"				, "ELSE" },
	{ "enum"				, "ENUM" },
	{ "extern"				, "EXTERN" },
	{ "float"				, "FLOAT" },
	{ "for"					, "FOR" },
	{ "goto"				, "GOTO" },
	{ "if"					, "IF" },
	{ "int"					, "INT" },
	{ "long"				, "LONG" },
	{ "register"			, "REGISTER" },
	{ "return"				, "RETURN" },
	{ "short"				, "SHORT" },
	{ "signed"				, "SIGNED" },
	{ "sizeof"				, "SIZEOF" },
	{ "static"				, "STATIC" },
	{ "struct"				, "STRUCT" },
	{ "switch"				, "SWITCH" },
	{ "typedef"				, "TYPEDEF" },
	{ "union"				, "UNION" },
	{ "unsigned"			, "UNSIGNED" },
	{ "void"				, "VOID" },
	{ "volatile"			, "VOLATILE" },
	{ "while"				, "WHILE" },

	{ "[[:alpha:]_]([[:alpha:]_]|[[:digit:]_])*"		, "IDENTIFIER" },

	{ "0[xX][a-fA-F0-9]+(u|U|l|L)*"		, "HEX_INTEGER_CONST" },
	{ "0[[:digit:]]+(u|U|l|L)*"			, "OCT_INTEGER_CONST" },
	{ "[[:digit:]]+(u|U|l|L)*"			, "DEC_INTEGER_CONST" },
	{ "'(\\.|[^\\'])+'"	, "CHAR_CONSTANT" },
	{ "L'(\\.|[^\\'])+'"	, "WCHAR_CONSTANT" },
	
	{ "[[:digit:]]+([Ee][+-]?[[:digit:]]+)(f|F|l|L)?"					, "FLOAT_CONST" },
	{ "[[:digit:]]*\\.[[:digit:]]+([Ee][+-]?[[:digit:]]+)?(f|F|l|L)?"	, "FLOAT_CONST" },
	{ "[[:digit:]]+\\.[[:digit:]]*([Ee][+-]?[[:digit:]]+)?(f|F|l|L)?"	, "FLOAT_CONST" },
	
	{ "\"(\\\\.|[^\\\"])*\""	,"STRING_LITERAL" },
	{ "L?\"(\\.|[^\\\"])*\""	,"WSTRING_LITERAL" },

	{ "\\.\\.\\."			, "ELLIPSIS" },
	{ ">>="					, "RIGHT_ASSIGN" },
	{ "<<="					, "LEFT_ASSIGN" },
	{ "\\+="				, "ADD_ASSIGN" },
	{ "-="					, "SUB_ASSIGN" },
	{ "\\*="				, "MUL_ASSIGN" },
	{ "/="					, "DIV_ASSIGN" },
	{ "%="					, "MOD_ASSIGN" },
	{ "&="					, "AND_ASSIGN" },
	{ "\\^="				, "XOR_ASSIGN" },
	{ "\\|="				, "OR_ASSIGN" },
	{ ">>"					, "RIGHT_OP" },
	{ "<<"					, "LEFT_OP" },
	{ "\\+\\+"				, "INC_OP" },
	{ "--"					, "DEC_OP" },
	{ "->"					, "PTR_OP" },
	{ "&&"					, "AND_OP" },
	{ "\\|\\|"				, "OR_OP" },
	{ "<="					, "LE_OP" },
	{ ">="					, "GE_OP" },
	{ "=="					, "EQ_OP" },
	{ "!="					, "NE_OP" },
	{ ";"					, ";" },
	{ "\\{|<%"				, "{" },
	{ "}|%>"				, "}" },
	{ ","					, "," },
	{ ":"					, ":" },
	{ "="					, "=" },
	{ "\\("					, "(" },
	{ "\\)"					, ")" },
	{ "\\[|<:"				, "[" },
	{ "]|:>"				, "]" },
	{ "\\."					, "." },
	{ "&"					, "&" },
	{ "!"					, "!" },
	{ "~"					, "~" },
	{ "-"					, "-" },
	{ "\\+"					, "+" },
	{ "\\*"					, "*" },
	{ "/"					, "/" },
	{ "%"					, "%" },
	{ "<"					, "<" },
	{ ">"					, ">" },
	{ "\\^"					, "^" },
	{ "\\|"					, "|" },
	{ "\\?"					, "?" },

	{ "[ \t\v\n\f]"			, "BLANK" },
	{ "."					, "BAD CHARACTER" }
};

const char *defRegex[][2] =
{
	{ "<.+>", "Greedy Tag" },
	{ "<.+?>", "Non-greedy Tag" },
	{ "0[bB][01]+", "BINARY DIGIT" },
	{ ".+ [1-9][0-9]+([.,][0-9]{2})?", "MONEY" },
	{ "([A-Za-z][A-Za-z0-9+.-]*:)"
	  "(//[[:alnum:]+.-]+)"
	  "(:?[0-9]*)"
	  "((//[[:alnum:]+.-]+))?", "url" },
	{ "[0-9]{3}\\.?[0-9]{3}\\.?[0-9]{3}-?[0-9]{2}", "CPF" },
};

struct MatchInfo
{
	const char *input;
	int sm;
	int em;
	void *param;
	DFA *dfaFrom;
	int curState;
};

typedef int(*MatchingCallbackFunction)(MatchInfo matchInfo);
typedef int(*ErrorCallbackFunction)(int type, const char *input, int pos);

struct RegexEntry
{
public:
	std::string regex;
	void *param;
	RegexMatcher *matcher;
	NFA *associatedNFA;
	DFA *associatedDFA;
	MatchingCallbackFunction m_matchingInputCallbackFunction;

	RegexEntry(Alphabet* alphabet, const char *regex, MatchingCallbackFunction matchFunction, void *param) {
		this->regex = regex;
		this->param = param;
		matcher = new RegexMatcher(regex,alphabet);
		associatedNFA = matcher->nfa();
		associatedDFA = matcher->dfa();
		m_matchingInputCallbackFunction = matchFunction;
	}

	inline void setMatchingInputCallbackFunction(MatchingCallbackFunction matchFunction) { m_matchingInputCallbackFunction = matchFunction; }
};

class RegexManager
{
	Alphabet* m_defaultAlphabet;
	MatchingCallbackFunction m_defaultMatchingFunction;
	ErrorCallbackFunction m_defaultErrorFunction;
	std::vector<RegexEntry*> m_regexList;

public:

	RegexManager() {
		m_defaultAlphabet = NULL;
		m_defaultMatchingFunction = NULL;
		m_defaultErrorFunction = NULL;
	}

	int findPatterns(const char *input)
	{
		const char *c = input;
		int startPos = 0;
		int matchLen = 0;
		int curPos = 0;
		int count = 0;

		while(*c) {
			bool res = false;
			int biggestMatchLen = 0;
			int biggestMatchEntryIndex = -1;

			for(int i = 0; i < m_regexList.size(); ++i) {
				RegexMatcher *matcher = m_regexList[i]->matcher;
				if(res |= matcher->matchPartially(c, &matchLen)) {
					if(matchLen > biggestMatchLen) {
						biggestMatchLen = matchLen;
						biggestMatchEntryIndex = i;
					}
				}
			}

			if(res) {
				if(m_regexList[biggestMatchEntryIndex]->m_matchingInputCallbackFunction) {
					struct MatchInfo mf;
					mf.input = input;
					mf.sm = curPos;
					mf.em = curPos + biggestMatchLen;
					mf.dfaFrom = m_regexList[biggestMatchEntryIndex]->associatedDFA;
					mf.param = m_regexList[biggestMatchEntryIndex]->param;
					mf.curState =  m_regexList[biggestMatchEntryIndex]->associatedDFA->currentStateNumber();
					res = (*m_regexList[biggestMatchEntryIndex]->m_matchingInputCallbackFunction)(mf);
				}
			}

			//matching function may change res value
			if(!res) {
				if(m_defaultErrorFunction)
					(*m_defaultErrorFunction)(0, input, curPos);
				biggestMatchLen = 1;
			}
			else {
				++count;
			}

			c += biggestMatchLen;
			curPos += biggestMatchLen;
		}

		return count;
	}

	int insertRegex(const char *regex, const char *param, MatchingCallbackFunction matchingFunction = NULL, Alphabet *alphabet = NULL) {
		if(alphabet == NULL)
			alphabet = m_defaultAlphabet;
		if(matchingFunction == NULL)
			matchingFunction = m_defaultMatchingFunction;
		RegexEntry *entry = new RegexEntry(alphabet,regex,matchingFunction,(void*)param);
		m_regexList.push_back(entry);
		return m_regexList.size() - 1;
	}

	int removeRegex(int index) {
		if(index >= 0 && index < m_regexList.size()) {
			delete m_regexList[index];
			m_regexList.erase(m_regexList.begin() + index);
			return 1;
		}
		return -1;
	}

	void clearRegexList()
	{
		if(!m_regexList.empty()) {
			for(int i = 0; i < m_regexList.size(); ++i) {
				delete m_regexList[i];
			}
			m_regexList.clear();
		}
	}

	void loadRegexList(const char *defRegex[][2], int size) {
		for(int i = 0; i < size; ++i) {
			insertRegex(defRegex[i][0], defRegex[i][1]);
		}
	}

	void setDefaultMatchingFunction(MatchingCallbackFunction matchingFunction) {
		m_defaultMatchingFunction = matchingFunction;
	}

	void setDefaultErrorFunction(ErrorCallbackFunction errorFunction) {
		m_defaultErrorFunction = errorFunction;
	}

	int regexCount() const {
		return m_regexList.size();
	}

	RegexEntry *regexEntry(int index) const {
		return m_regexList[index];
	}
};

static bool g_running;

void exit(void *param) {
	g_running = false;
}

void insert_regex(void *param) {
	RegexManager *manager = (RegexManager*)param;
	std::string input;
	std::cout << "Regex: ";
	std::getline(std::cin,input);
	std::cout << "Tag: ";
	std::string tag;
	std::getline(std::cin,tag);
	printf("Parsing: %s...\n",input.c_str());
	int index = manager->insertRegex(input.c_str(), tag.c_str());
	RegexEntry *entry = manager->regexEntry(index);
	NFA *nfa = entry->associatedNFA;
	DFA *dfa = entry->associatedDFA;
	printNFAStates(nfa);
	int count = 0;
	for(int i = 0; i < dfa->stateCount(); ++i) {
		count += dfa->state(i)->transitionCount();
	}
	printDFAStates(dfa);
	printf("Transition count: %d\n", count);
}

void remove_regex(void *param) {
	RegexManager *manager = (RegexManager*)param;
	int opt;
	std::cout << "Regex number: ";
	std::cin >> opt;
	std::cin.ignore();
	manager->removeRegex(opt);
}

void print_regex_list(void *param) {
	RegexManager *manager = (RegexManager*)param;
	int regexCount = manager->regexCount();
	for(int i = 0; i < regexCount; ++i) {
		RegexEntry *entry = manager->regexEntry(i);
		printf("[%d] %s (%s)\n", i, entry->regex.c_str(), (const char*)entry->param);
	}
}

void clear_regex_list(void *param) {
	RegexManager *manager = (RegexManager*)param;
	manager->clearRegexList();
}

void load_c_tokens_list(void *param) {
	RegexManager *manager = (RegexManager*)param;
	manager->loadRegexList(C_tokens,sizeof(C_tokens) / sizeof(C_tokens[0]));
}

void do_regex(void *param) {
	RegexManager *manager = (RegexManager*)param;
	std::string input;
	std::getline(std::cin,input);
	while(input.c_str()[0] != '\0') {
		std::cout << "Analyzing: " << input << "\n";

		int result = manager->findPatterns(input.c_str());
		printf("Found %d ocurrences.\n",result);
		std::getline(std::cin,input);
	}
}

struct Option
{
	const char *description;
	void (*optProc)(void *param);
};

struct Option g_optList[] = {
	{ "Exit", exit },
	{ "Insert regex", insert_regex },
	{ "Remove regex", remove_regex },
	{ "Show regex list", print_regex_list },
	{ "Clear regex list", clear_regex_list },
	{ "Load C tokens list", load_c_tokens_list }, 
	{ "Search for patterns in text", do_regex }
};

//todo: add parameter of type MatchingFunction on insertRegex
int MatchFunction(MatchInfo matchInfo)
{
	if(strcmp((const char*)matchInfo.param, "BLANK") != 0) {
		printf("Found at: %d - %d (%s)\n", matchInfo.sm, matchInfo.em, (const char*)matchInfo.param);
	}
	return 1;
}

int ErrorFunction(int type, const char *input, int pos)
{
	printf("Invalid symbol at: %d (%c)\n", pos, input[pos]);
	return 1;
}

int main()
{
	RegexManager manager;
	manager.setDefaultMatchingFunction(MatchFunction);
	manager.setDefaultErrorFunction(ErrorFunction);
	manager.loadRegexList(defRegex,sizeof(defRegex)/sizeof(defRegex[0]));

	std::string input;
	int opt;

	g_running = true;

	int optCount = sizeof(g_optList) / sizeof(g_optList[0]);
	while(g_running) {
		std::cout << "Select an option:\n";
		for(int i = 0; i < optCount; ++i) {
			std::cout << '[' << i << "] " << g_optList[i].description << "\n";
		}

		std::cout << "Option: ";

		std::cin >> opt;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

		if(opt < optCount) {
			g_optList[opt].optProc((void*)&manager);
		}
	}


	//printf("\n");
	/*
	try {
		RegexParser *parser = new RegexParser;

		/*
		//RegexNode *tree = parser->parse("(a*b)*.|.|c");
		RegexNode *tree = parser->parse("a{3}b{4,}c{5,6}");

		if(tree) {
			printf("Parsed successfully.\n");
			printTree(tree);
		}
		else
			printf("Syntax error!\n");
			*/
	/*}
	catch(const char* msg) {
		printf(msg);
		printf("\n");
	}
	*/
	system("PAUSE");
	return 0;
}