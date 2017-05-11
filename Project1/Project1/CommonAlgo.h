#ifndef COM_ALGO_H
#define COM_ALGO_H
#include "IBattleshipGameAlgo.h"
#include <set>
#include <string>

class CommonAlgo : IBattleshipGameAlgo
{
public:

	std::string		*board = NULL;
	std::string		moves;
	int				player_num;
	int				read_pos;
	int				line_num;
	int				done;

	std::pair<int, int>	dim = std::make_pair(-1, -1);
	std::set<char>		myLetters;
	int player;


	/**
	* \brief copying the relevant board and updating data set.
	*		  being called by setBoards from GameMaster
	* \param board
	* \param numRows
	* \param numCols
	*/
	void setBoard(int player, const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	void notifyOnAttackResult(int player, int row, int col, AttackResult result);

	bool init(const std::string& path);

	/**
	* \brief initilze all internal variables.
	* \param player_num
	* \param letters
	* \param moves
	*/
	CommonAlgo(int player_num, char* letters, const  char* moves);

	CommonAlgo() = default;

	~CommonAlgo();
};

#endif