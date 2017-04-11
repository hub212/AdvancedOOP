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

