#ifndef COM_ALGO_H
#define COM_ALGO_H
#include "IBattleshipGameAlgo.h"
#include <set>
#include <string>
using namespace std;

// this class is the most common Algo class - ALL algo classes shuld inherite it
class CommonAlgo : IBattleshipGameAlgo
{
public:

	std::string			*board = NULL;					// will hold the board after copy is done
	std::pair<int, int>	dim = std::make_pair(-1, -1);	// (MAX_ROW,MAX_COL)
	std::set<char>		myLetters;						// the battle ships representation (BPMD vs. bpmd)

	int					player_num;		

	int					done;			// this is an indication for no more moves (NEED to be turned on when moves are done)
	int					focused_search;	// flag for random vs. focused search - only for none reading from file algo.
	std::pair<int, int>	last_coor;		// coor are in range of (1,10)

	/// NOTE - the coordiantes are board associated - [1,MAX_ROW/COL] and not array associated [1, MAX_ROW/COL-1]


	

	/**
	* \brief copying the relevant board and updating data set.
	*		  being called by setBoards from GameMaster
	* \param board
	* \param numRows
	* \param numCols
	*/
	void setBoard(int player, const char** board, int numRows, int numCols);

	std::pair<int, int> attack();

	// empty implementation by default
	void notifyOnAttackResult(int player, int row, int col, AttackResult result);

	// empty implementation by default
	bool init(const std::string& path);

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