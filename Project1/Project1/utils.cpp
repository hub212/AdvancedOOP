#include "utils.h"
////-------------------------
////		Utils
////-------------------------


Vessel_ID Utils::get_vessel(char curr, Player playerA, Player playerB)
{
	Vessel_ID vessel;

	if (playerA.myLetters.find(curr) != playerA.myLetters.end())
	{
		if (curr == 'B')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Boat, Players::PlayerA);
		}
		else if (curr == 'P')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Missiles, Players::PlayerA);
		}
		else if (curr == 'M')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Sub, Players::PlayerA);
		}
		else if (curr == 'D')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::War, Players::PlayerA);
		}
	}
	else if (playerB.myLetters.find(curr) != playerB.myLetters.end())
	{
		// it's Players B vessel
		if (curr == 'b')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Boat, Players::PlayerB);
		}
		else if (curr == 'p')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Missiles, Players::PlayerB);
		}
		else if (curr == 'm')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::Sub, Players::PlayerB);
		}
		else if (curr == 'd')
		{
			vessel = Vessel_ID::Vessel_ID(VesselType::War, Players::PlayerB);
		}
	}

	return vessel;
}

bool Utils::is_sink(char** boards, int x, int y, int curr)
{
	bool up, down, left, right;

	up = Utils::search_up(boards, x, y, curr);
	down = Utils::search_down(boards, x, y, curr);
	right = Utils::search_right(boards, x, y, curr);
	left = Utils::search_left(boards, x, y, curr);

	return up&down&right&left;
}

bool Utils::search_up(char** boards, int x, int y, char curr)
{
	//search up
	while (--x >= 0 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_down(char** boards, int x, int y, char curr)
{
	//search down
	while (++x < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_right(char** boards, int x, int y, char curr)
{
	//search right
	while (++y < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_left(char** boards, int x, int y, char curr)
{
	//search left
	while (--y >= 0 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}


 int Utils::copyBoard(char** in_board, int numCols, int numRows, char*** out_board) 
{

	try {
		*out_board = new char*[numRows];
	}
	catch (std::bad_alloc& exc)
	{
		cout << "Error: string array allocation failed for player A while allocating board; " << exc.what() << endl;
		return 1;
	}

	for (int row = 0; row < numRows; row++)
	{
		try {
			(*out_board)[row] = new char[numCols + 1];
		}
		catch (std::bad_alloc& exc)
		{
			cout << "Error: string array allocation failed for player B while allocating row strings; " << exc.what() << endl;
			for (int delete_row = 0; delete_row < row; delete_row++)
				delete[](*out_board)[delete_row];
			delete[](*out_board);
			return 1;
		}

		if (strcpy_s((*out_board)[row], (rsize_t)numCols + 1, in_board[row])) {
			cout << "Error: copy failed failed for boards separating" << endl;
			for (int delete_row = 0; delete_row < numRows; delete_row++)
				delete[](*out_board)[delete_row];
			delete[](*out_board);
			return 1;
		}
	}
}
