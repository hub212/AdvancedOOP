#pragma once
#include "ex1.h"
#include "Board.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>

#define NUM_ROWS 10
#define NUM_COLS 10
class BoardChecker
{
public:
	static bool isDebug;
	ifstream* checkPath(char* path);
	BoardChecker();
	void printIllegalShapeError(string illegalShips, char ch);
	bool string_has_suffix(const std::string &str, const std::string &suffix);
	static int shipSize(char ch);
	void checkBoard(char* path);
	~BoardChecker();
};

