// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include <sstream>
#include <string>
#include "ex1.h"
#include "Game.h"
#include "BoardChecker.h"
#include "Tests.h"


void createPath(int argc, char* argv[], char* pwd) {

	int cnt = 0;

	for (int i = 1; i < argc; i++) {

		if (cnt == MY_MAX_PATH-1) {
			break;
		}

		if (i > 1) {
			pwd[cnt++] = ' ';
		}

		string arg(argv[i]);

		for (int j = 0; j < arg.length(); j++) {

			if (cnt == MY_MAX_PATH - 1) {
				break;
			}

			if (j == arg.length()-1 && !(i == 1 && j==0) && ((arg.at(j)=='/') || (arg.at(j)=='\\'))) {
				continue;
			}
			
			else if ((argc > 2 || arg.length() > 1) && j == 0 && i == 1 && ((arg.at(j) == '/') || (arg.at(j) == '\\'))) {
				continue;
			}

			else if (arg.at(j) == '\'' || arg.at(j) == '"') {
				continue;
			}
			
			else {
				pwd[cnt++] = arg.at(j);
			}
		}
	}

	pwd[cnt] = '\0';

	for (int i=0; i < MY_MAX_PATH; i++) {

		if (pwd[i] == '/') {
			pwd[i] = '\\';
		}

	}
}

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
	int delay = 300;
	int quiet = 0;

	for (int i = 0; i < argc; i++) {
		if (!string("-delay").compare(argv[i])) {
			if (i == argc - 1) {
				cout << "Error: usage " << argv[0] << "[path] -delay <delay in ms> -quiet" << endl;
					return -1;
			}
			istringstream iss(argv[i+1]);

			if (!(iss >> delay).fail()) {
				if (DEBUG)
					cout << "delay was set to " << delay << " ms" << endl;
			}
			else {
				cout << "Error: was expecting to get integer after -delay opt. but got '" << argv[i + 1] << "' instead" << endl;
				return -1;
			}
		}

		if (!string("-quiet").compare(argv[i])) {
			quiet = 1;
			cout << "quit on" << endl;
		}
	}

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

	char pwd[MY_MAX_PATH];

	if (argc > 1 && !string("-quiet").compare(argv[1]) && !string("-delay").compare(argv[1])) {
		createPath(argc, argv, pwd);
		isInputOk = bc->checkBoard(pwd);
	}
	else {
		
		GetCurrentDirectoryA(MY_MAX_PATH, pwd);
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

	game_master = new GameMaster(boards, players_moves, NUM_ROWS, NUM_COLS, delay, quiet);
	if (game_master->play() != 0) {
		del(&game_master, &bc);
		return -1;
	}
	del(&game_master, &bc);
	return 0;
}
