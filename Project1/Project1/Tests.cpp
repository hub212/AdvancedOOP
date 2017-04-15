#include "Tests.h"
#include <Windows.h>
#include <algorithm>
#include "Game.h"

Tests::Tests()
{
	// getting paths to relevant tests dir - moves, good boards, bad boards
	GetCurrentDirectory(MY_MAX_PATH, pwd);
	files = pwd;
	files.append("\\TestFiles");
	moves_dir = files + "\\Moves";
	string boards_dir = files;
	good_boards_dir = boards_dir + "\\GoodBoards";
	bad_boards_dir = boards_dir + "\\BadBoards";
	

	// init fixed board for moves checking
	newBoards();
	// builds moves vector list
	 GetFileNamesInDirectory(&moves_list, moves_dir);
	std::sort(moves_list.begin(), moves_list.end());
}

Tests::~Tests()
{
	deleteBoards();
}

int Tests::moves_check()
{
	for (int i = 0; i < moves_list.size(); i += 2) {
		vector<const char*> moves;
		moves.push_back(moves_list[i].c_str());
		moves.push_back(moves_list[i + 1].c_str());
		if (i != 0) newBoards();
		GameMaster *game_master = new GameMaster(fixed_board, moves, SIZE, SIZE);
		if (game_master->play() != 0)
		{
			cout << "Error: checking moves failed on files " << (" %s ", moves_list[i]) << ("; %s", moves_list[i + 1]) << endl;
			return -1;
		}
		delete game_master;
		deleteBoards();
	}
	return 0;
}



void Tests::GetFileNamesInDirectory(vector<string> *names,string folder)
{
	string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				(*names).push_back(moves_dir + "\\" +  fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}


int Tests::newBoards()
{
	string init_board[] = { "  B       ", " p     PP ", " p        ", "      MMM ", "m  d      ", "m  d     b", "m  d      ", "   d      ", "B         ", "   b    PP" };
	for (int i = 0; i < SIZE; i++) {
		fixed_board[i] = new char[SIZE + 1];
		strcpy_s(fixed_board[i], SIZE + 1, init_board[i].c_str());
	}
	return 0;
}

int Tests::deleteBoards()
{
	for (int i = 0; i< SIZE; i++)
	{
		if (fixed_board[i] != nullptr) delete[] fixed_board[i];
		fixed_board[i] = nullptr;
	}
	return 0;
}
