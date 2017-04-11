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

int main(int argc, char* argv[])
{

	BoardChecker::isDebug = true;
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
		return 1;
	}

	// getting curent path
	TCHAR pwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pwd);
	printf("pwd: %s\n", pwd);

	char attackA[MAX_PATH*2];
	char attackB[MAX_PATH*2];
	/*
	strcpy_s(attackA, pwd);
	strcat(attackA, "\\clean_movesA.attack-a");
	strcpy_s(attackB, pwd);
	strcat(attackB, "\\clean_movesB.attack-b");
	*/

	strcpy_s(attackA, bc->movesA_file.c_str());
	strcpy_s(attackB, bc->movesB_file.c_str());

	printf("playerA attack file path: %s\n", attackA);
	printf("playerB attack file path: %s\n", attackB);

	vector<const char*> players_moves;
	players_moves.push_back(attackA);
	players_moves.push_back(attackB);

	// FIXME - manual board input for testing.
	char** boards = bc->board;
	/*
	boards = (char**)malloc(sizeof(char*) * 10);
	char* temp[] = { "  B       ", " p        ", " p        ", "      MMM ", "m  d      ", "m  d     b", "m  d      ", "   d      ", "B          ", "   b    PP"
	};

	for (int i = 0; i<10; i++)
	{
		boards[i] = (char*)malloc(11 * sizeof(char));
		strcpy(boards[i], temp[i]);
	}
	*/

	strcpy_s(attackA, bc->movesA_file.c_str());
	strcpy_s(attackB, bc->movesB_file.c_str());

	printf("temporary board:\n");
	// FIXME - debug leftovers
	for (int i = 0; i<10; i++)
	{
		printf("%s\n", boards[i]);
	}

	// DOTO - connect parsed board to gameMaster
	GameMaster game_master = GameMaster(boards, players_moves, 10, 10);
	if (game_master.play() != 0)
		return -1;
	
	delete bc;
	return 0;
}

