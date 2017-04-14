#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "IBattleshipGameAlgo.h"
#include "ex1.h"
#include <set>
#include <vector>

#if(TEST == 1)
#define DEBUG 1
#else
#define DEBUG 0
#endif

// foreward decleration
class Vessel_ID;

enum class Search
{
	Down,
	Right
};

enum class Players {
	PlayerA = 0,
	PlayerB = 1
};

enum class VesselType
{
	Boat, Missiles, Sub, War
};

enum class Scores
{
	BScore = 2,
	MScore = 3,
	SScore = 7,
	WScore = 8
};

enum class Length
{
	BLength = 1,
	MLength = 2,
	SLength = 3,
	WLength = 4
};


class Vessel_ID
{
public:
	VesselType type;
	Players player;
	int score;

	Vessel_ID(VesselType type, Players player);
	Vessel_ID();
};

class Player : IBattleshipGameAlgo
{
public:

	std::string		*board = NULL;
	std::string		moves;
	int				player_num;
	int				read_pos;
	int				line_num;
	int				done;

	std::pair<int, int>	dim = std::make_pair(-1,-1);

	std::set<char>		myLetters;

	
	/**
	 * \brief copying the relevant board and updating data set.
	 *		  being called by setBoards from GameMaster
	 * \param board 
	 * \param numRows 
	 * \param numCols 
	 */
	void setBoard(const char** board, int numRows, int numCols);

	/**
	 * \brief executing the next move. exctracting the next move from move files.
	 *		  being called by GameMaster attack().
	 * \return pair of int - (row,col) tuple. (-1,-1) for failure, (0,0) -EOF.
	 */
	std::pair<int, int> attack();

	/**
	 * \brief converting string to int and handles exceptions.
	 * \param str 
	 * \param num 
	 * \return 
	 */
	int str2int(const std::string str, int* num);


	void notifyOnAttackResult(int player, int row, int col, AttackResult result);
	
	/**
	 * \brief initilze all internal variables.
	 * \param player_num 
	 * \param letters 
	 * \param moves 
	 */
	Player(int player_num, char* letters,const  char* moves);


	Player() = default;

	~Player();
};

class GameMaster
{

private:

	Player playerA;
	Player playerB;

	char**	boards;
	std::pair<int, int> dim;
	std::vector<const char*> players_moves;

	int		scores[2];
	Players turn;



	std::pair<Vessel_ID, AttackResult> attack_results(std::pair<int, int> move);

	int extractBoards(const char** board, int numRows, int numCols, char**** out_board);

	void setBoards(const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	int initGame();

	void update_state(std::pair<int,int> move, std::pair<Vessel_ID, AttackResult> results);

	bool is_defeat();

	void print_results();


public:
	/**
	* \brief init all internal variables - paths and boards. intansiating the Player intances.
	* \param boards
	* \param players_moves
	* \param numRows
	* \param numCols
	*/
	GameMaster(char** boards, std::vector<const char*>& players_moves, int numRows, int numCols);

	/**
	* \brief impliments the game running phase.
	*		  responsible for attack() and notifyOnAttackResult() and updating current state.
	*/
	int play();
};


class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	static Vessel_ID Utils::get_vessel(const char curr, Player playerA, Player playerB);

	static bool Utils::search_up(char** boards, int x, int y, char curr);

	static bool Utils::search_down(char** boards, int x, int y, char curr);

	static bool Utils::search_right(char** boards, int x, int y, char curr);

	static bool Utils::search_left(char** boards, int x, int y, char curr);

	static bool is_sink(char** boards, int x, int y, int curr);

};
#endif