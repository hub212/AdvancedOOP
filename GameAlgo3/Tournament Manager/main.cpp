// ex1.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <Windows.h>
#include <sstream>
#include <algorithm>
#include <queue>
#include <memory>
#include <map>
#include "main.h"
#include "TournamentManager.h"
#include "SingleGameManager.h"
#include "BoardChecker.h"

using namespace std;

// this function 
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

int parseArgs(int argc, char* argv[], int& threads, bool& threadsSetFromCmdLn) {

	for (int i = 0; i < argc; i++) {
		if (!string("-threads").compare(argv[i])) {
			if (i == argc - 1) {
				cout << "Error: usage " << argv[0] << "[path] -threads <number of threads>" << endl;
				return -1;
			}
			istringstream iss(argv[i + 1]);

			if (!(iss >> threads).fail()) {
				threadsSetFromCmdLn = true;
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

	bool threadsSetFromCmdLn = false;

	// Command Line Parsing
	if (parseArgs(argc, argv, threads, threadsSetFromCmdLn) != 0) {
		return 1;
	}

	// time measurments
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double elapsedTime;

	if (TIME) {
		// get ticks per second
		QueryPerformanceFrequency(&frequency);

		// start timer
		QueryPerformanceCounter(&t1);
	}

	// Checking phase
	bool	isInputOk;
	bool	isDllFound = false;
	char	pwd[MY_MAX_PATH];
	BoardChecker::isDebug = 0;
	unique_ptr<BoardChecker> bc(new  BoardChecker());

	if (argc > 1 && string("-threads").compare(argv[1])) {
		createPath(argc, argv, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound, threadsSetFromCmdLn, threads);
	}
	else {
		GetCurrentDirectoryA(MY_MAX_PATH, pwd);
		isInputOk = bc->checkBoard(pwd, isDllFound, threadsSetFromCmdLn, threads);
	}

	BoardChecker::log << BoardChecker::getTime() << ": Number of threads: " << threads << std::endl;


	if (TIME) {
		// stop timer
		QueryPerformanceCounter(&t2);

		// compute and print the elapsed time in sec
		elapsedTime = (t2.QuadPart - t1.QuadPart)*1.0 / frequency.QuadPart;

		cout << "check boards time: " << elapsedTime << " [sec]" << endl;

		// get ticks per second
		QueryPerformanceFrequency(&frequency);

		// start timer
		QueryPerformanceCounter(&t1);
	}

	// Tournament phase
	TournamentManager manager(threads, bc->boardVec, bc->dllVec);

	if (!isInputOk) {
		BoardChecker::log << std::endl;
		BoardChecker::log.close();
		return 1;
	}

	manager.runGameThreads();
	manager.printStatus();


	BoardChecker::log << std::endl;
	BoardChecker::log.close();

	if (TIME) {

		// stop timer
		QueryPerformanceCounter(&t2);

		// compute and print the elapsed time in sec
		elapsedTime = (t2.QuadPart - t1.QuadPart) *1.0 / frequency.QuadPart;

		cout << "tournament time: " << elapsedTime << " [sec]" << endl;
	}

	return 0;

}
