#ifndef BOARD_CH_H
#define BOARD_CH_H
#include <algorithm>
#include "Board.h"
#include "main.h"
#include "Utils.h"


class BoardChecker
{
public:

	static std::ofstream log;

	static string getTime();

	static bool isDebug;

	static vector<string> dllVec;
	static vector<std::shared_ptr<Board>> boardVec;

	std::shared_ptr<Board> checkBoard(string boardPath);
	
	bool checkPath(char * path, bool & isDllFound);
	BoardChecker();
	static void printIllegalShapeError(string illegalShips, char ch);
	bool checkBoard(char* path, bool& isDllFound);

	~BoardChecker();
};
#endif
