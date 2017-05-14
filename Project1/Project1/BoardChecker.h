#ifndef BOARD_CH_H
#define BOARD_CH_H

#include "ex1.h"
#include "utils.h"

class BoardChecker
{
public:
	vector<string> dllVec = {};
	char** board;
	int num_rows;
	int num_cols;
	static bool isDebug;
	ifstream* checkPath(char* path);
	BoardChecker();
	void printIllegalShapeError(string illegalShips, char ch);
	static int shipSize(char ch);
	bool checkBoard(char* path);
	~BoardChecker();
};
#endif
