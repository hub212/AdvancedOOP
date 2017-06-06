#ifndef BOARD_CH_H
#define BOARD_CH_H
#include <algorithm>
#include "Utils.h"
#include "TournamentManager.h"


class BoardChecker
{
public:
	vector<char**> boards;
	int num_rows;
	int num_cols;

	static bool isDebug;
	static vector<string> boardsList;
	static vector<string> dllsLists;


	int checkPath(char * path, bool & isDllFound);

	BoardChecker();

	void printIllegalShapeError(string illegalShips, char ch);

	static int shipSize(char ch);

	bool checkBoard(char* path, bool& isDllFound);

	~BoardChecker();
};
#endif
