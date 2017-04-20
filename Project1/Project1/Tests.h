#ifndef TESTS_H
#define TESTS_H
#include "BoardChecker.h"
#include <vector>
#include "windows.h"



class Tests {

	char pwd[MY_MAX_PATH];
	string files;
	string moves_dir;
	string good_boards_dir;
	string bad_boards_dir;
	char* fixed_board[NUM_ROWS];
	vector<string> moves_list;

	void GetFileNamesInDirectory(std::vector<string> *names, std::string folder);
	int newBoards();
	int deleteBoards();

public:
	Tests();

	~Tests();
	int Tests::moves_check();
	
};
#endif