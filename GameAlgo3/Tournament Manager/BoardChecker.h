#ifndef BOARD_CH_H
#define BOARD_CH_H
#include <algorithm>
#include "TournamentManager.h"
#include "Utils.h"

class BoardChecker
{
public:
	char** board;
	int num_rows;
	int num_cols;
	static bool isDebug;
	ifstream* checkPath(char * path, bool & isDllFound);
	BoardChecker();
	void printIllegalShapeError(string illegalShips, char ch);
	static int shipSize(char ch);
	bool checkBoard(char* path, bool& isDllFound);

	static vector<string> boardsList;
	static vector<string> dllsLists;

	~BoardChecker();
};
#endif
