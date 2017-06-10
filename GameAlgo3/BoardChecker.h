#ifndef BOARD_CH_H
#define BOARD_CH_H
#include <algorithm>
#include "Board.h"
#include "TournamentManager.h"
#include "Utils.h"


class BoardChecker
{
public:

	static bool isDebug;

	static vector<string> dllVec;
	static vector<std::shared_ptr<const Board>> boardVec;

	std::shared_ptr<const Board> checkBoard(string boardPath);
	
	bool checkPath(char * path, bool & isDllFound);
	BoardChecker();
	static void printIllegalShapeError(string illegalShips, char ch);
	bool checkBoard(char* path, bool& isDllFound);

	~BoardChecker();
};
#endif
