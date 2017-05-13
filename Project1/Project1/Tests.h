#ifndef TESTS_H
#define TESTS_H
#define DLL_TEST 1
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

	int newBoards();
	int deleteBoards();
	
	// dll support
	// define function of the type we expect
	vector<tuple<string, HINSTANCE, GetAlgoType>> dll_vec; // vector of <Shape Name, dll handle, GetShape function ptr>


public:
	Tests();

	~Tests();
	int Tests::moves_check();
	
};
#endif