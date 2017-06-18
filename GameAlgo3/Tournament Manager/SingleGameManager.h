#ifndef GAMEMASTER_H
#define GAMEMASTER_H
#include <windows.h>
#include <set>
#include <vector>
#include <tuple>
#include<memory>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "Types.h"
#include "Board.h"

using namespace std;

// this class is the game managment class
class SingleGameManager
{

private:

	IBattleshipGameAlgo* player0;
	IBattleshipGameAlgo* player1;
	
	std::set<char>		lettersA = {'B','P','M','D' };
	std::set<char>		lettersB = {'b','p','m','d' };

	Board board;
	Board origBoard;
	Player0Board board0;
	Player1Board board1;

	Coordinate dims;

	int		scores[2];
	Players turn;

	tuple<string, HINSTANCE, GetAlgoType> dll0;
	tuple<string, HINSTANCE, GetAlgoType> dll1;

	MatchHard match;

	std::pair<Vessel_ID, AttackResult> attack_results(Coordinate move);

	/* call set_board() for each player*/
	void setBoards();

	/* call set_player() for each player*/
	void setPlayers();

	/* call attack() for each player in his turn*/
	Coordinate attack();

	/* updating scores after each turn*/
	void updateScores(Coordinate move, std::pair<Vessel_ID, AttackResult> results);

	/* check if the game is over*/
	bool anyWinner();

	/* transfrom from char type vessel to more readble vessel type (e.g War, Missile and etc.)*/
	Vessel_ID getVessel(const char curr);

	/* debug function for tracking a game flow*/
	void print_results();

public:

	/**
	* brief init all internal variables - paths and boards. intansiating the Player intances.
	* param match
	*/
	SingleGameManager(MatchHard match);

	/**
	* impliments the game running phase.
	* responsible for attack() and notifyOnAttackResult() and updating current state.
	*/
	int play();

	vector<int> getScores();
};

#endif