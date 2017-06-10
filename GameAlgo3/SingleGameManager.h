#ifndef GAMEMASTER_H
#define GAMEMASTER_H
#include <windows.h>
#include <set>
#include <vector>
#include <tuple>
#include <windows.h>
#include <set>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "Types.h"
#include "Board.h"

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
	Player0Board board0;
	Player1Board board1;


	char**	boards;
	int rows;
	int cols;
	int depth;
	const char* players_moves;

	int		scores[2];
	Players turn;

	int delay;
	int quiet;

	vector<tuple<string, HINSTANCE, GetAlgoType>> dll_vec;

	tuple<shared_ptr<Player>, shared_ptr<Player>, shared_ptr<Board>> match;


	std::pair<Vessel_ID, AttackResult> attack_results(std::pair<int, int> move);

	int extractBoards(const char** board, int numRows, int numCols, char**** out_board);

	void setBoards(const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	void update_state(std::pair<int,int> move, std::pair<Vessel_ID, AttackResult> results);

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

	void print_board(int x,int y,int delay);


public:
	/**
	* \brief init all internal variables - paths and boards. intansiating the Player intances.
	* \param boards
	* \param players_moves
	* \param numRows
	* \param numCols
	*/
	SingleGameManager(char** boards, const char* players_moves, int numRows, int numCols, int delay, int quiet, char **boardCopy);

	/**
	* \brief init all internal variables - paths and boards. intansiating the Player intances.
	* \param boards
	* \param players_moves
	* \param numRows
	* \param numCols
	*/
	SingleGameManager(char** boards, const char* players_moves, int numRows, int numCols, int delay, int quiet, vector<tuple<string, HINSTANCE, GetAlgoType>> dll_vec, char **boardCopy);


	SingleGameManager(tuple<shared_ptr<Player>, shared_ptr<Player>, shared_ptr<Board>> match);

	/**
	* \brief impliments the game running phase.
	*		  responsible for attack() and notifyOnAttackResult() and updating current state.
	*/
	int play();

	bool init(string path);
};

#endif