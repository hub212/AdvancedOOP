#ifndef BOARD_CHECK_H
#define BOARD_CHECK_H
#include "TournamentManager.h"
#include <algorithm>
#include <vector>

using namespace std;


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