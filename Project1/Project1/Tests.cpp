#include "Tests.h"
#include <Windows.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <tuple>
#include "Game.h"

Tests::Tests()
{
	// getting paths to relevant tests dir - moves, good boards, bad boards
	GetCurrentDirectoryA(MY_MAX_PATH, pwd);
	files = pwd;
	files.append("\\TestFiles");
	moves_dir = files + "\\Moves";
	string boards_dir = files;
	good_boards_dir = boards_dir + "\\GoodBoards";
	bad_boards_dir = boards_dir + "\\BadBoards";

	// init fixed board for moves checking
	newBoards();
	// builds moves vector list
	Utils::GetFileNamesInDirectory(&moves_list, moves_dir);
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
		Board *boardCopy = new Board(NUM_ROWS, NUM_COLS);
		for (int i = 0; i < NUM_ROWS; i++) {
			for (int j = 0; j < NUM_COLS; j++) {
				boardCopy->set(i, j, fixed_board[i][j]);
			}
		}
		GameMaster *game_master;
		if (DLL_TEST) {
			HINSTANCE hDll = LoadLibraryA("C:\\Users\\User7\\Source\\Repos\\AdvancedOOP\\Project1\\x64\\Debug\\NaiiveAlgo.dll");
			if (!hDll){
				std::cout << "could not load the dynamic library" << std::endl;
				return EXIT_FAILURE;
			}

			GetAlgoType getAlgo;
			getAlgo = (GetAlgoType)GetProcAddress(hDll, "GetAlgorithm");
			if (!getAlgo)
			{
				std::cout << "could not load function GetShape()" << std::endl;
				return EXIT_FAILURE;
			}
			string AlgoName = "NaiiveAlgo";
			dll_vec.push_back(make_tuple(AlgoName, hDll, getAlgo));




			HINSTANCE hDll2 = LoadLibraryA("C:\\Users\\User7\\Source\\Repos\\AdvancedOOP\\Project1\\x64\\Debug\\SmartAlgo.dll");
			if (!hDll) {
				std::cout << "could not load the dynamic library" << std::endl;
				return EXIT_FAILURE;
			}

			GetAlgoType getAlgo2;
			getAlgo2 = (GetAlgoType)GetProcAddress(hDll2, "GetAlgorithm");
			if (!getAlgo2)
			{
				std::cout << "could not load function GetShape()" << std::endl;
				return EXIT_FAILURE;
			}
			string AlgoName2 = "SmartAlgo";




			dll_vec.push_back(make_tuple(AlgoName2, hDll2, getAlgo2));
			//GameMaster *game_master = new GameMaster(fixed_board, pwd, NUM_ROWS, NUM_COLS, 100, 0);
			game_master = new GameMaster(fixed_board, pwd, NUM_ROWS, NUM_COLS, 100, 0, dll_vec, boardCopy->getboard());
		}
		else {
			game_master = new GameMaster(fixed_board, pwd, NUM_ROWS, NUM_COLS, 100, 0, boardCopy->getboard());
		}
		if (game_master->play() != 0)
		{
			cout << "Error: checking moves failed on files " << (" %s ", moves_list[i]) << ("; %s", moves_list[i + 1]) << endl;
			return -1;
		}
		delete boardCopy;
		delete game_master;
		deleteBoards();
	}
	return 0;
}



int Tests::newBoards()
{
	string init_board[] = { "  B       ", " p     PP ", " p        ", "      MMM ", "m  d      ", "m  d     b", "m  d      ", "   d      ", "B         ", "   b    PP" };
	for (int i = 0; i < NUM_ROWS; i++) {
		fixed_board[i] = new char[NUM_COLS + 1];
		strcpy_s(fixed_board[i], NUM_COLS + 1, init_board[i].c_str());
	}
	return 0;
}

int Tests::deleteBoards()
{
	for (int i = 0; i< NUM_ROWS; i++)
	{
		if (fixed_board[i] != nullptr) delete[] fixed_board[i];
		fixed_board[i] = nullptr;
	}
	return 0;
}
