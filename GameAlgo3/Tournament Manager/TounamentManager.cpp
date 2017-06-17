#include "TournamentManager.h"
#include "SingleGameManager.h"
#include "Board.h"
#include "Types.h"
#include "tuple"
#include "main.h"
#include <thread>
#include <memory>
#include <vector>
#include <list>
#include <map>

using namespace std;

TournamentManager::TournamentManager(int threads, vector<std::shared_ptr<Board>> boardVec, vector<string> dllVec) : threads(threads), numOfPlayers(dllVec.size()) {

	setBoards(boardVec);
	setPlayers(dllVec);
	setMatches();
	initScores();
}

void TournamentManager::setBoards(vector<shared_ptr<Board>>& inBoards)
{
	for (auto board : inBoards) {
		this->BoardsVector.push_back(board);
	}
}

void TournamentManager::setPlayers(vector<string>& dllVec)
{

	std::sort(dllVec.begin(), dllVec.end());

	for (const auto dll : dllVec) {

		string AlgoName = dll.substr(dll.find_last_of("\\") == string::npos ? 0 : dll.find_last_of("\\") + 1, dll.find_first_of('.'));
		HINSTANCE hDll = LoadLibraryA(dll.c_str());
		bool dllOk = true;
		if (!hDll) {
			cout << "Cannot not load dll: " << dll << std::endl;
			dllOk = false;
			isInputOK = false;
		}

		GetAlgoType getAlgo;
		getAlgo = (GetAlgoType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgo)
		{
			if (dllOk) {
				cout << "Cannot not load dll: " << dll << std::endl;
				dllOk = false;
				isInputOK = false;
			}
		}
		
		shared_ptr<Player> player(new Player({ AlgoName, hDll, getAlgo }));
		playersDlls.push_back(player);

	}
}

void TournamentManager::setThreads()
{
	vector<thread>		singleGameThreads;
	for (int i = 0; i < threads; i++) {
		singleGameThreads.push_back(thread(&TournamentManager::play, this));
	}

	for (auto& thread : singleGameThreads) {
		thread.join();
	}
}

void TournamentManager::setMatches()
{
	// init matches queue
	for (auto board : BoardsVector) {
		for (auto player0 : playersDlls) {
			for (auto player1 : playersDlls) {
				if (player0->name == player1->name) continue;
				Match match = { make_shared<Player>(0, *player0), make_shared<Player>(1, *player1), board };
				matchesQueue.push_back(match);
			}
		}
	}
	// randomizing queue
	std::random_shuffle(matchesQueue.begin(), matchesQueue.end());
	
	for (auto &match : matchesQueue) {
		cout << match << endl;
	}
}

void TournamentManager::updateStatus(Match match, vector<int> score)
{
	unique_lock<mutex> locker(mut_scores);
	string players[] = { get<0>(match)->name, get<1>(match)->name };

	for (uint16_t i = 0; i < PLAYERS_IN_MATCH; i++) {
		gameStatus[players[i]].push(score[i]);
	}

	// All players has at least one score
	if (gameStatus.size() == numOfPlayers) {
		for (auto &player : playersDlls) {
			scores[player->name] += gameStatus[player->name].front();
			cout << player->name << ": " << scores[player->name] << endl;
		}
		popScores();
	}
	locker.unlock();
}

void TournamentManager::popScores()
{
	auto itr = gameStatus.begin();
	while (itr != gameStatus.end()) {
		itr->second.pop();
		if (itr->second.size() == 0) {
			itr = gameStatus.erase(itr);
		}
		else {
			itr = next(itr);
		}
	}
}

void TournamentManager::initScores()
{
	for (auto &player : playersDlls) {
		scores[player->name] = 0;
	}
}

int TournamentManager::play()
{
	// need to change this part to threads
	while (true){
		unique_lock<mutex> locker(mut_matches);
		if (matchesQueue.empty()) {
			locker.unlock();
			break;
		}
		Match match = matchesQueue.back();
		matchesQueue.pop_back();
		locker.unlock();

		if (DEBUG) {
			get<2>(match)->printBoard();
		}
		MatchHard hardenMatch = { get<0>(match), get<1>(match), *get<2>(match) };
		std::auto_ptr<SingleGameManager> game_master(new SingleGameManager(hardenMatch));
		if (game_master->play() != 0) {
			return -1;
		}

		TournamentManager::updateStatus(match, game_master->getScores());

	}
	return 0;
}

