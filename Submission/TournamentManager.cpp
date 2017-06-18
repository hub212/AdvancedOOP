#include "TournamentManager.h"
#include "SingleGameManager.h"
#include "Board.h"
#include "Types.h"
#include "tuple"
#include "main.h"
#include <iomanip>
#include <thread>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <sstream>

using namespace std;

TournamentManager::TournamentManager(int threads, vector<std::shared_ptr<Board>> boardVec, vector<string> dllVec) : threads(threads), numOfPlayers(dllVec.size()), maxLengthName(0) {

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

		int back_slash = dll.find_last_of("\\");
		string AlgoName = dll.substr(back_slash == string::npos ? 0 : back_slash + 1, back_slash == string::npos ? 0 : dll.length()- 5 - back_slash);
		HINSTANCE hDll = LoadLibraryA(dll.c_str());

		GetAlgoType getAlgo;
		getAlgo = (GetAlgoType)GetProcAddress(hDll, "GetAlgorithm");
		
		shared_ptr<Player> player(new Player({ AlgoName, hDll, getAlgo }));
		playersDlls.push_back(player);
		maxLengthName = AlgoName.size() > maxLengthName ? AlgoName.size() : maxLengthName;
	}
}

void TournamentManager::runGameThreads()
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
	
	totalMatches = matchesQueue.size();
	matchNumber = 1;
	//for (auto &match : matchesQueue) {
	//	cout << match << endl;
	//}
}

void TournamentManager::updateStatus(Match match, vector<int> score)
{
	unique_lock<mutex> locker(mut_scores);
	string players[] = { get<0>(match)->name, get<1>(match)->name };
	bool winner = score[0] >= score[1];

	for (uint16_t i = 0; i < PLAYERS_IN_MATCH; i++) {
		bool won = i == (winner ? 1 : 0);
		int pntFor = score[i];
		int pntAgn = score[i ? 0 : 1];
		tuple<bool, int, int> state = { won, pntFor, pntAgn };
		gameStatus[players[i]].push(state);
	}

	// All players has at least one score
	if (gameStatus.size() == numOfPlayers) {
		for (auto &player : playersDlls) {
			tuple<int, int, int> state = gameStatus[player->name].front();
			// Winns
			get<0>(scores[player->name]) += get<0>(state);
			// Losses
			get<1>(scores[player->name]) += !get<0>(state);
			// Pts For
			get<2>(scores[player->name]) += get<1>(state);
			// Pts Against
			get<3>(scores[player->name]) += get<2>(state);
		}
		printStatus();
		popScores();
	}
	matchNumber++;
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
		scores[player->name] = {0,0,0,0};
	}
}

void TournamentManager::printStatus() {
	int index = 1;
	int width = 10;
	int realMatchNumber = (matchNumber < totalMatches) ? matchNumber : totalMatches;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// I didn't figure out how can one sort a map by value so I created an helper data set for sorting needs only
	vector<pair<string, double>> namePer;
	for (auto &player : playersDlls) {
		namePer.push_back({ player->name,  static_cast<double>(get<0>(scores[player->name])) / (get<0>(scores[player->name]) + get<1>(scores[player->name])) * 100.00 });
	}
	sort(namePer.begin(), namePer.end(), pairCompare);

	if (firstTable)
	{
		GetConsoleScreenBufferInfo(hStdout, &ScreenBufferInfo);
		firstTable = 0;
	}
	Utils::gotoxy(ScreenBufferInfo.dwCursorPosition.X, ScreenBufferInfo.dwCursorPosition.Y);
	cout << "Matches progress : " << realMatchNumber << "/" << totalMatches << endl;
	cout << left << setw(5) << "#" << left << setw(maxLengthName + 4) << "Team Name" << left << setw(width) << "Wins" << left << setw(width) << "Losses" << left << setw(width) << "%" << left << setw(width) << "Pts For" << left << setw(width) << "Pts Against" << endl;
	for (auto &pair : namePer) {
		string name = pair.first;
		tuple<int, int, int, int> score = scores[name];
		stringstream indxstr;
		indxstr << index << ".";
		cout << left << setw(5) << indxstr.str() << left << setw(maxLengthName + 4) << name << left << setw(width) << get<0>(score) << left << setw(width) << get<1>(score) << left << setw(width) << setprecision(2) << fixed << pair.second << left << setw(width) << get<2>(score) << left << setw(width) << get<3>(score) << endl;
		index++;
	}

}

int TournamentManager::play()
{
	// need to change this part to threads
	while (true && playersDlls.size()){
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

