#ifndef BOARD_CH_H
#define BOARD_CH_H
#include <algorithm>
#include "ex2.h"
#include "Utils.h"

class BoardChecker
{
public:
	vector<string> dllVec = {};
	char** board;
	int num_rows;
	int num_cols;
	static bool isDebug;
	ifstream* checkPath(char * path, bool & isDllFound);
	BoardChecker();
	void printIllegalShapeError(string illegalShips, char ch);
	static int shipSize(char ch);
	bool checkBoard(char* path, bool& isDllFound);

	~BoardChecker();
};
#endif
