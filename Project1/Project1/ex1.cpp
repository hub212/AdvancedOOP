// ex1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "ex1.h"
#include "Game.h"
#include <vector>
#include <Windows.h>
#include <WinBase.h>


#define MAX_PATH 1024

int main(int argc, char* argv[])
{
	TCHAR pwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pwd);
	MessageBox(NULL, pwd, pwd, 0);

	vector<const char*> players_moves;
	players_moves.push_back("C:\\Users\\Shlomi\\Source\\Repos\\AdvancedOOP2\\Project1\\clean_movesA.attack-a");
	players_moves.push_back("C:\\Users\\Shlomi\\Source\\Repos\\AdvancedOOP2\\Project1\\clean_movesA.attack-b");

	// FIXME - manual board input for testing.
	char** boards = (char**)malloc(sizeof(char*) * 10);
	char* temp[] = { "  B       ", " p        ", " p        ", "      MMM ", "m  d      ", "m  d     b", "m  d      ", "   d      ", "B          ", "   b    PP"
	};

	for (int i = 0; i<10; i++)
	{
		boards[i] = (char*)malloc(11 * sizeof(char));
		strcpy(boards[i], temp[i]);
	}

	// FIXME - debug leftovers
	for (int i = 0; i<10; i++)
	{
		printf("%s\n", boards[i]);
	}

	// DOTO - connect parsed board to gameMaster
	GameMaster game_master = GameMaster(const_cast<const char**>(boards), players_moves, 10, 10);
	game_master.setBoard(const_cast<const char**>(boards), game_master.dim.first, game_master.dim.second);

	game_master.attack();

	
	return 0;
}

