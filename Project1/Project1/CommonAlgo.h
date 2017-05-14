#ifndef COM_ALGO_H
#define COM_ALGO_H
#include "IBattleshipGameAlgo.h"
#include "utils.h"
#define DOWN 0
#define RIGHT 1
#define UP 2
#define LEFT 3
#define HERE 4
#define NOT_TARGET 1
#define TARGET 0
#include <set>
#include <string>
using namespace std;

class CommonAlgo : public IBattleshipGameAlgo
{
public:

	//michael 12/5/17 08:19 added start
	bool				removeFromRandomTargets(int direction);
	void				markAdjacentCells();

	std::pair<int, int>	attackPair;
	int					currentRow = 0; //the row coordinate of currently attacked cell
	int					currentCol = 0; //the column coordinate of currently attacked cell
	int**				possible_targets = NULL;	// will hold the board after copy is done
	int					rows; //number of rows in the game table
	int					cols; //number of columns in the game table
	//michael 12/5/17 08:19 added end


	std::string			*board = NULL;	// will hold the board after copy is done
	std::set<char>		myLetters;

	int					player_num;

	int					done;			//this is an indication for no more moves (NEED to be turned on when moves are done)
	int					focused_search;
	std::pair<int, int>	last_coor;		// coor are in range of (1,10)




	/**
	* \brief copying the relevant board and updating data set.
	*		  being called by setBoards from GameMaster
	* \param board
	* \param numRows
	* \param numCols
	*/
	void setBoard(int player, const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	/**
	* \brief initilze all internal variables.
	* \param player_num
	* \param letters
	* \param moves
	*/
	CommonAlgo(int player_num, char* letters);

	CommonAlgo() = default;

	~CommonAlgo();
};

#endif
