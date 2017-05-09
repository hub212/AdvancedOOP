#include "Game.h"

class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	static Vessel_ID Utils::get_vessel(const char curr, Player playerA, Player playerB);

	static bool		Utils::search_up(char** boards, int x, int y, char curr);

	static bool		Utils::search_down(char** boards, int x, int y, char curr);

	static bool		Utils::search_right(char** boards, int x, int y, char curr);

	static bool		Utils::search_left(char** boards, int x, int y, char curr);

	static bool		Utils::is_sink(char** boards, int x, int y, int curr);

	static void	Utils::copyBoard(char** in_board, int numCols, int NumRow, char*** out_board)
};