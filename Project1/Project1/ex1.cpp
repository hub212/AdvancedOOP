// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include <sstream>
#include <string>
#include <algorithm>
#include "ex1.h"
#include "Game.h"
#include "BoardChecker.h"
#include "Tests.h"


void createPath(int argc, char* argv[], char* pwd) {

	int cnt = 0;

	for (int i = 1; i < argc; i++) {

		if (cnt == MY_MAX_PATH - 1) {
			break;
		}

		if (string("-delay").compare(argv[i]) || string("-quiet").compare(argv[i])) {
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

			if (j == arg.length() - 1 && !(i == 1 && j == 0) && ((arg.at(j) == '/') || (arg.at(j) == '\\'))) {
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

	for (int i = 0; i < MY_MAX_PATH; i++) {

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
			istringstream iss(argv[i + 1]);

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
	bool isDllFound = false;

	char pwd[MY_MAX_PATH];

	if (argc > 1 && !string("-quiet").compare(argv[1]) && !string("-delay").compare(argv[1])) {
		createPath(argc, argv, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound);
	}
	else {
		GetCurrentDirectoryA(MY_MAX_PATH, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound);
	}


	char** boards = bc->board;

	Board *boardCopy = new Board(NUM_ROWS, NUM_COLS);
	if (isInputOk) {
		for (int i = 0; i < NUM_ROWS; i++) {
			for (int j = 0; j < NUM_COLS; j++) {
				boardCopy->set(i, j, boards[i][j]);
			}
		}
	}

	// dlls handling
	vector<tuple<string, HINSTANCE, GetAlgoType>> dllVec; // vector of <Shape Name, dll handle, GetShape function ptr>

	if (isDllFound) {
		std::sort(bc->dllVec.begin(), bc->dllVec.end());

		for (const auto dll : bc->dllVec) {

			string AlgoName = dll.substr(dll.find_last_of("\\") == string::npos ? 0 : dll.find_last_of("\\") + 1, dll.find_first_of('.'));
			HINSTANCE hDll = LoadLibraryA(dll.c_str());
			bool dllOk = true;
			if (!hDll) {
				cout << "Cannot not load dll: " << dll << std::endl;
				dllOk = false;
				isInputOk = false;
			}

			GetAlgoType getAlgo;
			getAlgo = (GetAlgoType)GetProcAddress(hDll, "GetAlgorithm");
			if (!getAlgo)
			{
				if (dllOk) {
					cout << "Cannot not load dll: " << dll << std::endl;
					dllOk = false;
					isInputOk = false;
				}
			}
			if (isInputOk) {
				dllVec.push_back(make_tuple(AlgoName, hDll, getAlgo));
			}

		}
	}

	if (!isInputOk) {
		delete boardCopy;
		del(&game_master, &bc);
		return 1;
	}
	game_master = new GameMaster(boards, pwd, NUM_ROWS, NUM_COLS, delay, quiet, dllVec, boardCopy->getboard());
	if (!game_master->init(pwd))
		return 1;
	if (game_master->play() != 0) {
		del(&game_master, &bc);
		return -1;
	}
	delete boardCopy;
	del(&game_master, &bc);
	return 0;
}
