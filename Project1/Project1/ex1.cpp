// ex1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "ex1.h"
#include "Game.h"
#include <vector>
#include <Windows.h>
#include <WinBase.h>
#include "BoardChecker.h"
//test push michael

#define NUM_ROWS 10
#define NUM_COLS 10
#define MAX_PATH 1024
#define MY_MAX_PATH 1024

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
	GameMaster* game_master = nullptr;
	BoardChecker::isDebug = false;
	BoardChecker* bc = new BoardChecker();
	bool isInputOk = false;
	if (argc > 1) {
		isInputOk = bc->checkBoard(argv[1]);
	}
	else {
		TCHAR pwd[MY_MAX_PATH];
		GetCurrentDirectory(MY_MAX_PATH, pwd);
		isInputOk = bc->checkBoard(pwd);
	}

	if (!isInputOk) {
		del(&game_master, &bc);
		return 1;
	}

	char attackA[MAX_PATH*2];
	char attackB[MAX_PATH*2];

	strcpy_s(attackA, bc->movesA_file.c_str());
	strcpy_s(attackB, bc->movesB_file.c_str());

	printf("playerA attack file path: %s\n", attackA);
	printf("playerB attack file path: %s\n", attackB);

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

