#include <map>
#include <vector>
#include <memory>
#include <list>
#include <queue>
#include "Types.h"

#define NUM_OF_PLAYERS 2

using namespace std;

class TournamentManager {

	int threads;
	bool isInputOK;

	vector<int>						singleGameThreads;
	vector<shared_ptr<Board>>		BoardsVector;
	vector<shared_ptr<Player>>		playersDlls;
	vector<Match>					matchesQueue;

	map<string, queue<int>>			gameStatus;
	map<string, int>					scores;


	void setBoards(vector<shared_ptr<Board>>& inBoards);

	void setPlayers(vector<string>& dllVec);

	void setThreads(int threads);

	void setMatches();

	void updateStatus(Match match, vector<int> score);

	void popScores();

public:

	TournamentManager(int threads, vector<std::shared_ptr<Board>> boardVec, vector<string> dllVec);
	int play();
};