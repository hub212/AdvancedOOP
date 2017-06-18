#include <map>
#include <vector>
#include <memory>
#include <list>
#include <queue>
#include <mutex>
#include <thread>
#include "Types.h"
#include "BoardChecker.h"

#define PLAYERS_IN_MATCH 2

using namespace std;

class TournamentManager {

	int threads;
	int numOfPlayers;
	bool isInputOK;

	vector<shared_ptr<Board>>		BoardsVector; 
	vector<shared_ptr<Player>>		playersDlls;
	vector<Match>					matchesQueue;

	// following the game scores
	map<string, queue<tuple<int, int, int>>>	gameStatus;
	map<string, tuple<int,int,int,int>>				scores;

	mutex		mut_matches;
	mutex		mut_scores;

	// helpers for nice printing
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
	int firstTable = 1;
	int matchNumber;
	int totalMatches;
	int maxLengthName;

	void setBoards(vector<shared_ptr<Board>>& inBoards);

	void setPlayers(vector<string>& dllVec);

	// run a single game - being called by thread
	int play();

	void setMatches();

	// follows the scores status and notify on screen when round is over
	void updateStatus(Match match, vector<int> score);

	// helper function for updateStatus
	void popScores();

	// helper function for updateStatus
	void initScores();


public:

	TournamentManager(int threads, vector<std::shared_ptr<Board>> boardVec, vector<string> dllVec);

	void runGameThreads();

	void printStatus();
};