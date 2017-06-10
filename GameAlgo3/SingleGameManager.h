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
	char **boardCopy;

	std::set<char>		lettersA = {'B','P','M','D' };
	std::set<char>		lettersB = {'b','p','m','d' };

	shared_ptr<Board> board;
	shared_ptr<Board> origBoard;
	Player0Board board0;
	Player1Board board1;

	Coordinate dims;
	const char* players_moves;

	int		scores[2];
	Players turn;

	int delay;
	int quiet;

	tuple<string, HINSTANCE, GetAlgoType> dll0;
	tuple<string, HINSTANCE, GetAlgoType> dll1;


	tuple<shared_ptr<Player>, shared_ptr<Player>, shared_ptr<Board>> match;


	std::pair<Vessel_ID, AttackResult> attack_results(Coordinate move);

	void setBoards(Board board);

	Coordinate attack();

	void update_state(Coordinate move, std::pair<Vessel_ID, AttackResult> results);

	bool is_defeat();

	// input - 
	//		const char curr - (@,' ', BPMD, bpmd) a charecter representation of board point
	//		CommonAlgo PlayerA/B - two players with different char representation (BPMD vs. BPMD)
	//
	// output - 
	//		the function return the vessel type of the given char (transforming from char to vessel type)
	//
	// given a charecter representation of board point (@,' ', BPMD, bpmd) and two players with 
	// different char representation (BPMD vs. bpmd) the functino nill return the vessel on board (common game master board)
	// the function is being used for updating the game master board and for later (opt.) for complex algorithms
	Vessel_ID get_vessel(const char curr, IBattleshipGameAlgo* playerA, IBattleshipGameAlgo* playerB);

	void print_results();

public:

	/**
	* \brief init all internal variables - paths and boards. intansiating the Player intances.
	* \param match
	*/
	SingleGameManager(tuple<shared_ptr<Player>, shared_ptr<Player>, shared_ptr<Board>> match);

	/**
	* \brief impliments the game running phase.
	*		  responsible for attack() and notifyOnAttackResult() and updating current state.
	*/
	int play();
};

#endif