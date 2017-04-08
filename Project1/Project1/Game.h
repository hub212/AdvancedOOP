
#include "IBattleshipGameAlgo.h"
#include <string>
#include <set>
#include <vector>

class Player
{
public:

	std::string		*board = NULL;
	std::string		moves;
	int				player_num;
	int				read_pos;
	int				line_num;
	std::pair<int, int>	dim;

	std::set<char>		myLetters;

	
	/**
	 * \brief copying the relevant board and updating data set.
	 *		  being called by setBoard from GameMaster
	 * \param board 
	 * \param numRows 
	 * \param numCols 
	 */
	void setMyBoard(const char** board, int numRows, int numCols);

	/**
	 * \brief executing the next move. exctracting the next move from move files.
	 *		  being called by GameMaster attack().
	 * \return pair of int - (row,col) tuple. (-1,-1) for failure, (0,0) -EOF.
	 */
	std::pair<int, int> next_move();

	/**
	 * \brief converting string to int and handles exceptions.
	 * \param str 
	 * \param num 
	 * \return 
	 */
	int str2int(const std::string str, int* num);

	
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

class GameMaster : IBattleshipGameAlgo
{

public:

	Player playerA;
	Player playerB;
	char** boards;
	std::vector<const char*> players_moves;
	std::pair<int, int> dim;


	/**
	 * \brief init all internal variables - paths and boards. intansiating the Player intances. 
	 * \param boards 
	 * \param players_moves 
	 * \param numRows 
	 * \param numCols 
	 */
	GameMaster(const char** boards, std::vector<const char*>& players_moves, int numRows, int numCols);

	/**
	 * \brief impliments the game running phase.
	 *		  responsible for attack() and notifyOnAttackResult() and updating current state.
	 */
	void play();

	void setBoard(const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	void notifyOnAttackResult(int player, int row, int col, AttackResult result);

};
