#include "NaiiveAlgo.h"

////--------------------------
////		NaiiveAlgo
////--------------------------



bool NaiiveAlgo::init(const std::string& path) {

	currentCol = currentRow = -2;//michael 12/5/17 08:19 - added this line
	setNextAttack();
	return true;
};


void NaiiveAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	//michael 12/5/17 08:19 added code start
	if (this->player_num != player) {
		return;
	}
	setNextAttack();
	//michael 12/5/17 08:19 added code end
}


void NaiiveAlgo::setNextAttack() {
	if (currentCol == -1 || currentRow == -1) {
		currentCol = currentRow = -1;
		return;
	}
	if (currentCol == -2 || currentRow == -2) {
		currentRow = 0;
		currentCol = -1;
	}
	do {
		currentCol++;
		if (currentCol >= cols) {
			currentCol = 0;
			currentRow++;
		}
		if (currentRow >= rows) {
			currentCol = currentRow = -1;
			return;
		}
	} while (possible_targets[currentRow][currentCol] != TARGET);
}
//michael 12/5/17 08:19 added code end

IBattleshipGameAlgo* GetAlgorithm() {
	return (IBattleshipGameAlgo*) new NaiiveAlgo();
}