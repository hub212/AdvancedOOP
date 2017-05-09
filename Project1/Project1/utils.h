#include "Game.h"

class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	static Vessel_ID get_vessel(const char curr, Player playerA, Player playerB);

	static bool search_up(char** boards, int x, int y, char curr);

	static bool search_down(char** boards, int x, int y, char curr);

	static bool search_right(char** boards, int x, int y, char curr);

	static bool search_left(char** boards, int x, int y, char curr);

	static bool is_sink(char** boards, int x, int y, int curr);

	static int copyBoard(char** in_board, int numCols, int numRows, char*** out_board);

};