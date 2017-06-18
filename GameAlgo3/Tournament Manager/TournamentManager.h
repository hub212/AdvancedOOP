#include <map>
#include <vector>
#include <memory>
#include <list>
#include <queue>
#include <mutex>
#include <thread>
#include "Types.h"

#define PLAYERS_IN_MATCH 2

using namespace std;

class TournamentManager {

	int threads = 4;
	int numOfPlayers;
	bool isInputOK;

	vector<shared_ptr<Board>>		BoardsVector;
	vector<shared_ptr<Player>>		playersDlls;
	vector<Match>					matchesQueue;

	map<string, queue<tuple<int, int, int>>>	gameStatus;
	map<string, tuple<int,int,int,int>>				scores;

	mutex		mut_matches;
	mutex		mut_scores;

	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
	int firstTable = 1;

	void setBoards(vector<shared_ptr<Board>>& inBoards);

	void setPlayers(vector<string>& dllVec);

	int play();

	void setMatches();

	void updateStatus(Match match, vector<int> score);

	void popScores();

	void initScores();


public:

	TournamentManager(int threads, vector<std::shared_ptr<Board>> boardVec, vector<string> dllVec);

	void setThreads();

	void printStatus();
};