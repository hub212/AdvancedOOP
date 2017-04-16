#ifndef BOARD_CH_H
#define BOARD_CH_H

#include "ex1.h"

class BoardChecker
{
public:
	string movesA_file;
	string movesB_file;
	char** board;
	int num_rows;
	int num_cols;
	static bool isDebug;
	ifstream* checkPath(char* path);
	BoardChecker();
	void printIllegalShapeError(string illegalShips, char ch);
	bool string_has_suffix(const std::string &str, const std::string &suffix);
	static int shipSize(char ch);
	bool checkBoard(char* path);
	~BoardChecker();
};
#endif
