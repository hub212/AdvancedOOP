#include "Utils.h"
#include "Board.h"
#include <ostream>
#include <istream>
#include <vector>
#include <string>

////-------------------------
////		Utils
////-------------------------

using namespace std;


bool Utils::is_sink(Board origBoard, Coordinate coor, Board gameBoard, Coordinate dims)
{

	Board singleShipBoard(origBoard, coor.depth, coor.row, coor.col);

	for (int dpt = 0; dpt < singleShipBoard.depth(); dpt++) {
		for (int row = 0; row < singleShipBoard.rows(); row++) {
			for (int col = 0; col < singleShipBoard.cols(); col++) {
				if (singleShipBoard.get(dpt,row,col) != ' ') {
					if (gameBoard.get(dpt, row, col) != '@') {
						if (coor.col != col || coor.row != row || coor.depth != dpt) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
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


 int Utils::copyBoard(const char** in_board, int numCols, int numRows, char*** out_board) 
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
	return 0;
}


 void Utils::gotoxy(int x, int y)
 {
	 static HANDLE h = NULL;
	 if (!h)
		 h = GetStdHandle(STD_OUTPUT_HANDLE);
	 COORD c = {(short) x, (short) y };
	 SetConsoleCursorPosition(h, c);
 }

 void Utils::ShowConsoleCursor(bool showFlag)
 {
	 HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	 CONSOLE_CURSOR_INFO     cursorInfo;

	 GetConsoleCursorInfo(out, &cursorInfo);
	 cursorInfo.bVisible = showFlag; // set the cursor visibility
	 SetConsoleCursorInfo(out, &cursorInfo);
 }


 void Utils::GetFileNamesInDirectory(vector<string> *names, string folder)
{
	 string search_path = folder + "/*.*";
	 WIN32_FIND_DATAA  fd;
	 HANDLE hFind = FindFirstFileA(search_path.c_str(), &fd);
	 if (hFind != INVALID_HANDLE_VALUE) {
		 do {
			 // read all (real) files in current folder
			 // , delete '!' read other 2 default folder . and ..
			 if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				 (*names).push_back(folder + "\\" + fd.cFileName);
			 }
		 } while (::FindNextFileA(hFind, &fd));
		 ::FindClose(hFind);
	 }
 }


 bool Utils::string_has_suffix(const std::string &str, const std::string &suffix)
 {
	 return str.size() >= suffix.size() &&
		 str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
 }


 // Coordinate
 bool operator==(const Coordinate coor0, const Coordinate coor1)
 {
	 return coor0.col == coor1.col && coor0.row == coor1.row && coor0.depth == coor1.depth;
 }

 bool operator!=(const Coordinate coor0, const Coordinate coor1)
 {
	 return !(coor0 == coor1);
 }

 std::ostream& operator<<(std::ostream & out, const Coordinate& coor)
 {
	 out << "(" << coor.row << "," << coor.col << "," << coor.depth << ")";
	 return out;
 }

 Coordinate operator + (const Coordinate coor0, const Coordinate coor1)
 {
	 return{ coor0.col + coor1.col, coor0.row + coor1.row, coor0.depth + coor1.depth};
 }



