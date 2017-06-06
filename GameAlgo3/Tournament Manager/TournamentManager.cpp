// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include <sstream>
#include <algorithm>
#include <queue>
#include <memory>
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

		for (uint16_t j = 0; j < arg.length(); j++) {

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

int parseArgs(int argc, char* argv[], int& threads) {

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
	return 0;
}


int main(int argc, char* argv[])
{
	// Game Parameters
	int threads = 4;

	// Command Line Parsing
	if (parseArgs(argc, argv, threads) != 0) {
		return 1;
	}



	// Main Data structures
	vector<int>											singleGameThreads;
	vector<shared_ptr<Board>>							BoardsVector;
	vector<shared_ptr<Player>>							playersDlls;
	vector<tuple<shared_ptr<Player>, shared_ptr<Player>, shared_ptr<Board>>>	matchesQueue;
	vector<queue<int>>									scores;

	bool	isInputOk;
	bool	isDllFound = false;
	char	pwd[MY_MAX_PATH];


	// init threads vector
	for (int i = 0; i < threads; i++) {
		singleGameThreads.push_back(-1);
	}
	// start boards check - in this section we need to add multiple boards check 
	BoardChecker::isDebug = DEBUG;
	unique_ptr<BoardChecker> bc(new  BoardChecker());

	if (argc > 1 && string("-threads").compare(argv[1])) {
		createPath(argc, argv, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound);
	}
	else {
		GetCurrentDirectoryA(MY_MAX_PATH, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound);
	}

	// copy boards - need to add multile boards copy as well - maybe change bc->boards to bc->boards and iterate to copy all the boards
	if (isInputOk) {
		for (uint16_t board = 0; board < bc->boards.size(); board++) {
			for (uint16_t row = 0; row < NUM_ROWS; row++) {
				for (uint16_t col = 0; col < NUM_COLS; col++) {
					shared_ptr<Board> boardCopy(new Board(NUM_ROWS, NUM_COLS));
					boardCopy->set(row, col, bc->boards[board][row][col]);
					BoardsVector.push_back(boardCopy);
				}
			}
		}
	}




	// initializing the dll's vector
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
				shared_ptr<Player> player(new Player({ AlgoName, hDll, getAlgo }));
				playersDlls.push_back(player);
			}
		}
	}


	if (!isInputOk) {
		return 1;
	}


	// init matches queue
	for (auto boards : BoardsVector) {
		for (auto player1 : playersDlls) {
			for (auto player2 : playersDlls) {
				if (player1 == player2) continue;
				matchesQueue.push_back({ player1, player2, boards });
			}
		}
	}

		// randomizing queue
		std::random_shuffle(matchesQueue.begin(), matchesQueue.end());

		/*
			// need to change this part to threads
			for (auto match : matchesQueue) {

				std:auto_ptr<SingleGameManager> game_master (new SingleGameManager(boards, pwd, NUM_ROWS, NUM_COLS, delay, quiet, playersDlls, boardCopy->getboard()));
				if (!game_master->init(pwd))
					return 1;
				if (game_master->play() != 0) {
					return -1;
				}
			}
		*/
		return 0;

}
