#include "CommonAlgo.h"

////--------------------------
////		CommonAlgo
////--------------------------

// copy the relevant vessels
void CommonAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->player_num = player;

	//michael 12/5/17 08:19 added code start
	rows = numRows;
	cols = numCols;

	if (possible_targets == NULL) {

		possible_targets = new int*[rows];
		for (currentRow = 0; currentRow < rows; currentRow++) {
			possible_targets[currentRow] = new int[cols] {};
		}
		for (currentRow = 0; currentRow < rows; currentRow++) {
			for (currentCol = 0; currentCol < cols; currentCol++) {
				char c = board[currentRow][currentCol];
				if ((player_num == 0 && isupper(c)) || (player_num == 1 && islower(c))) {
					removeFromRandomTargets(HERE);
					markAdjacentCells();
				}
			}
		}
	}
	//michael 12/5/17 08:19 added code end
}


std::pair<int, int> CommonAlgo::attack()
{
	//michael 12/5/17 08:19 added code start
	attackPair.first = currentRow;
	attackPair.second = currentCol;
	//michael 12/5/17 08:19 added code end
	if (attackPair.first >= 0 && attackPair.second >= 0) {
		attackPair.first++;
		attackPair.second++;
	}
	return attackPair;
}

// decising letters for each player
CommonAlgo::CommonAlgo(int player_num, char* letters) : player_num(player_num), done(0), myLetters(letters, letters + strlen(letters)) {};



CommonAlgo::~CommonAlgo()
{
	//michael 12/5/17 08:19 added code start
	if (possible_targets != NULL) {
		for (int i = 0; i < rows; i++) {
			delete[] possible_targets[i];
		}
		delete[] possible_targets;
		possible_targets = NULL;
	}
	//michael 12/5/17 08:19 added code end
	if (board != nullptr) {
		delete[] board;
		board = nullptr;
	}
	myLetters.clear();
}

//michael 12/5/17 08:19 added code start
bool CommonAlgo::removeFromRandomTargets(int direction) {
	int row = direction == UP ? currentRow + 1 : (direction == DOWN ? currentRow - 1 : currentRow);
	int col = direction == RIGHT ? currentCol + 1 : (direction == LEFT ? currentCol - 1 : currentCol);
	if (row >= 0 && row < rows && col >= 0 && col < cols && possible_targets[row][col] != NOT_TARGET) {
		possible_targets[row][col] = NOT_TARGET;
		return true;
	}
	else {
		return false;
	}
}

void CommonAlgo::markAdjacentCells() {
	removeFromRandomTargets(UP);
	removeFromRandomTargets(DOWN);
	removeFromRandomTargets(LEFT);
	removeFromRandomTargets(RIGHT);
}
//michael 12/5/17 08:19 added code end