#include "CommonAlgo.h"

////--------------------------
////		CommonAlgo
////--------------------------

// copy the relevant vessels
void CommonAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->dim = make_pair(numRows, numCols);
	this->player_num = player;
}

bool CommonAlgo::init(const std::string& path) {
	return true;
};


void CommonAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

std::pair<int, int> CommonAlgo::attack()
{
	return make_pair(0, 0);
}

// decising letters for each player
CommonAlgo::CommonAlgo(int player_num, char* letters) : player_num(player_num), done(0), myLetters(letters, letters + strlen(letters)) {};



CommonAlgo::~CommonAlgo()
{
	if (board != nullptr) {
		delete[] board;
		board = nullptr;
	}
	myLetters.clear();
}


