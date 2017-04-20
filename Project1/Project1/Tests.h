#include <Windows.h>
#include <vector>
#include <iostream>
#include "Game.h"
#include "BoardChecker.h"
#include <algorithm>


#define SIZE 10

class Tests {

	char pwd[MY_MAX_PATH];
	string files;
	string moves_dir;
	string good_boards_dir;
	string bad_boards_dir;
	char* fixed_board[SIZE];
	vector<string> moves_list;

	void GetFileNamesInDirectory(std::vector<string> *names, std::string folder);
	int newBoards();
	int deleteBoards();

public:
	Tests();

	~Tests();
	int Tests::moves_check();
	
};