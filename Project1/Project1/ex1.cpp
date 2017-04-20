// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include "ex1.h"
#include "Game.h"
#include "BoardChecker.h"
#include "Tests.h"
//test push michael


void del(GameMaster** game_master, BoardChecker** bc) {
	if (*bc != nullptr) {
		delete *bc;
		*bc = nullptr;
	}
	if (*game_master != nullptr) {
		delete *game_master;
		*game_master = nullptr;
	}
}

int main(int argc, char* argv[])
{

	if (TEST)
	{
		Tests tests;
		if (tests.moves_check() != 0)
		{
			cout << "Error: moves_check() failed" << endl;
			return -1;
		}
		return 0;
	} 

	GameMaster* game_master = nullptr;
	BoardChecker::isDebug = false;
	BoardChecker* bc = new BoardChecker();
	bool isInputOk;
	if (argc > 1) {
		isInputOk = bc->checkBoard(argv[1]);
	}
	else {
		char pwd[MY_MAX_PATH];
		GetCurrentDirectory(MY_MAX_PATH, pwd);
		isInputOk = bc->checkBoard(pwd);
	}

	if (!isInputOk) {
		del(&game_master, &bc);
		return 1;
	}

	char attackA[MY_MAX_PATH];
	char attackB[MY_MAX_PATH];

	strcpy_s(attackA, bc->movesA_file.c_str());
	strcpy_s(attackB, bc->movesB_file.c_str());


	vector<const char*> players_moves;
	players_moves.push_back(attackA);
	players_moves.push_back(attackB);

	char** boards = bc->board;

	game_master = new GameMaster(boards, players_moves, NUM_ROWS, NUM_COLS);
	if (game_master->play() != 0) {
		del(&game_master, &bc);
		return -1;
	}
	del(&game_master, &bc);
	return 0;
}

