// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include <sstream>
#include <algorithm>
#include <queue>
#include "TournamentManager.h"
#include "SingleGameManager.h"
#include "BoardChecker.h"


void createPath(int argc, char* argv[], char* pwd) {

	int cnt = 0;

	for (int i = 1; i < argc; i++) {

		if (cnt == MY_MAX_PATH - 1) {
			break;
		}

		if (!string("-threads").compare(argv[i])) {
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

void del(SingleGameManager** game_manager, BoardChecker** bc) {
	if (*bc != nullptr) {
		delete *bc;
		*bc = nullptr;
	}
	if (*game_manager != nullptr) {
		delete *game_manager;
		*game_manager = nullptr;
	}
}

int main(int argc, char* argv[])
{
	// Game Parameters
	int threads = 4;

	// Command Line Parsing
	for (int i = 0; i < argc; i++) {
		if (!string("-threads").compare(argv[i])) {
			if (i == argc - 1) {
				cout << "Error: usage " << argv[0] << "[path] -threads <number of threads>" << endl;
				return -1;
			}
			istringstream iss(argv[i + 1]);

			if (!(iss >> threads).fail()) {
				if (DEBUG)
					cout << "threads was set to " << threads << " threads" << endl;
			}
			else {
				cout << "Error: was expecting to get integer after -threads opt. but got '" << argv[i + 1] << "' instead" << endl;
				return -1;
			}
		}
	}



	// Main Data structures
	vector<SingleGameManager*>				singleGameThreads;
	vector<Board*>							BoardsVector;
	vector<Player>							playersDlls;
	vector<tuple<Player, Player, Board>>	matchesQueue;

	bool isInputOk;
	bool isDllFound = false;
	char pwd[MY_MAX_PATH];


	// start board check
	BoardChecker::isDebug = DEBUG;
	BoardChecker* bc = new BoardChecker();

	if (argc > 1 && string("-threads").compare(argv[1])) {
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



	if (isDllFound) {
		std::sort(bc->dllsLists.begin(), bc->dllsLists.end());

		for (const auto dll : bc->dllsLists) {

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
				playersDlls.push_back(make_tuple(AlgoName, hDll, getAlgo));
			}
			//std::reverse(playersDlls.begin(), playersDlls.end());
		}
	}

	if (!isInputOk) {
		delete boardCopy;
		del(&game_master, &bc);
		return 1;
	}
	game_master = new SingleGameManager(boards, pwd, NUM_ROWS, NUM_COLS, delay, quiet, playersDlls, boardCopy->getboard());
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
