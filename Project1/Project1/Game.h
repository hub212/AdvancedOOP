#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "ex1.h"
#include "utils.h"
#include "Types.h"
#include "PreMovesAlgo.h"
#include <windows.h>
#include <set>
#include <vector>

// this class is the game managment class
class GameMaster
{

private:

	PreMovesAlgo playerA;
	PreMovesAlgo playerB;

	char**	boards;
	std::pair<int, int> dim;
	std::vector<const char*> players_moves;

	int		scores[2];
	Players turn;

	int delay = 400;



	std::pair<Vessel_ID, AttackResult> attack_results(std::pair<int, int> move);

	int extractBoards(const char** board, int numRows, int numCols, char**** out_board);

	void setBoards(const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	void update_state(std::pair<int,int> move, std::pair<Vessel_ID, AttackResult> results);

	bool is_defeat();

	void print_results();

	void print_board(int x,int y,int delay);


public:
	/**
	* \brief init all internal variables - paths and boards. intansiating the Player intances.
	* \param boards
	* \param players_moves
	* \param numRows
	* \param numCols
	*/
	GameMaster(char** boards, std::vector<const char*>& players_moves, int numRows, int numCols, int delay);

	/**
	* \brief impliments the game running phase.
	*		  responsible for attack() and notifyOnAttackResult() and updating current state.
	*/
	int play();
};

#endif