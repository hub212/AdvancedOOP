#ifndef UTILS_H
#define UTILS_H
#include "Types.h"
#include "CommonAlgo.h"
#include "Game.h"
class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	static Vessel_ID get_vessel(const char curr, CommonAlgo playerA, CommonAlgo playerB);

	static bool search_up(char** boards, int x, int y, char curr);

	static bool search_down(char** boards, int x, int y, char curr);

	static bool search_right(char** boards, int x, int y, char curr);

	static bool search_left(char** boards, int x, int y, char curr);

	static bool is_sink(char** boards, int x, int y, int curr);

	static int copyBoard(const char** in_board, int numCols, int numRows, char*** out_board);

};

#endif