#include "SmartAlgo.h"


int SmartAlgo::randomGen(int min, int max) {
	std::random_device rd;     
	std::mt19937 rng(rd());   
	std::uniform_int_distribution<int> uni(min, max);

	int random_integer = uni(rng);
	return random_integer;
}

bool SmartAlgo::init(const std::string& path) {



	aimRange = new int[4]{};
	initRandomTargets();
	return true;
}

void SmartAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result) {
	int attackResult = result == AttackResult::Miss ? NOT_TARGET : IS_HIT;
	attackResult = result == AttackResult::Sink ? IS_SINK : attackResult;
	if (this->player_num != player) {
		int tmpRow = currentRow;
		int tmpCol = currentCol;
		currentRow = row - 1;
		currentCol = col - 1;
		int cell = possible_targets[currentRow][currentCol];
		if (result == AttackResult::Sink && cell == TARGET) {
			//the other player sinked his own ship. the cells around it are empty.
			markAdjacentCells();
		}
		if (result == AttackResult::Hit && possible_targets[currentRow][currentCol] == TARGET) {
			//the other player hit his own ship but didn't sink it. add it to the target bank.
			targetBank.resize(targetBank.size() + 1, std::vector<int>(2, 0));
			targetBank[targetBank.size() - 1][0] = currentRow;
			targetBank[targetBank.size() - 1][1] = currentCol;
		}
		visitCell(HERE, attackResult, true);
		currentRow = tmpRow;
		currentCol = tmpCol;
		return;
	}
	visitCell(HERE, attackResult, true);
	if (result == AttackResult::Miss) {
		attackSucceeded = false;
	}
	else if (result == AttackResult::Hit) {
		attackSucceeded = true;
	}
	else if (result == AttackResult::Sink) {
		attackSucceeded = false;
		markAdjacentCells();
		if (seekAndDestroy) {
			currentRow = firstHitRow;
			currentCol = firstHitCol;
			markAdjacentCells();
		}
		seekAndDestroy = false;
	}
}

std::pair<int, int> SmartAlgo::attack()
{
	if (seekAndDestroy || attackSucceeded) {
		calcAttack();
	}
	while (!seekAndDestroy && targetBank.size() > 0) {
		//we are not busy chasing after a ship, so load a target from the target bank.
		attackSucceeded = true;
		currentRow = targetBank[targetBank.size() - 1][0]; //the next attacks will be centered around these coordinates
		currentCol = targetBank[targetBank.size() - 1][1]; //the next attacks will be centered around these coordinates
		targetBank.resize(targetBank.size() - 1);
		calcAttack();
	}
	if (!seekAndDestroy) {
		pickRandTarget();
	}
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

SmartAlgo::SmartAlgo()
{
}


SmartAlgo::~SmartAlgo()
{
	targetBank.clear();
	if (randomSpots != nullptr) {
		for (int i = 0; i < rows*cols; i++) {
			delete[] randomSpots[i];
		}
		delete[] randomSpots;
		randomSpots = nullptr;
	}
	if (aimRange != nullptr) {
		delete[] aimRange;
		aimRange = nullptr;
	}
}

void SmartAlgo::initRandomTargets() {
	int cnt = 0;
	int row, col;
	int tmp = rows*cols;
	randomSpots = new int*[tmp];
	for (row = 0; row<rows; row++) {
		for (col = 0; col<cols; col++) {

			randomSpots[cnt] = new int[2];
			randomSpots[cnt][0] = row;
			randomSpots[cnt][1] = col;
			cnt++;
		}
	}
	int tmpPair[2] = {};
	for (int shuffleIndex = rows*cols - 1; shuffleIndex > 0; shuffleIndex--) {
		randVar = randomGen(0, shuffleIndex);
		tmpPair[0] = randomSpots[randVar][0];
		tmpPair[1] = randomSpots[randVar][1];
		randomSpots[randVar][0] = randomSpots[shuffleIndex][0];
		randomSpots[randVar][1] = randomSpots[shuffleIndex][1];
		randomSpots[shuffleIndex][0] = tmpPair[0];
		randomSpots[shuffleIndex][1] = tmpPair[1];
	}
}

void SmartAlgo::pickRandTarget() {
	int iters = 0;
	do {
		iters++;
		currentRow = randomSpots[randomSpotIndex][0];
		currentCol = randomSpots[randomSpotIndex][1];
		int spotValue = possible_targets[currentRow][currentCol];
		randomSpotIndex = randomSpotIndex<rows*cols - 1 ? ++randomSpotIndex : 0;
	} while (possible_targets[currentRow][currentCol] != TARGET && iters <= rows*cols);
}

void SmartAlgo::calcAttack() {
	if (attackSucceeded && !seekAndDestroy) {
		seekAndDestroy = true; //we hit a ship and didn't sink it - continue to target it
		firstHitRow = currentRow; //the next attacks will be centered around these coordinates
		firstHitCol = currentCol; //the next attacks will be centered around these coordinates
		aimRange[UP] = aimRange[DOWN] = aimRange[RIGHT] = aimRange[LEFT] = NOT_TRIED;
	}
	else if (attackSucceeded && seekAndDestroy) {
		blockIrrelevantDirections(aimDirection);
	}
	else if (!attackSucceeded && seekAndDestroy) {
		aimRange[aimDirection] = 0;
	}
	determineAimDirection();
	calcCurrentCoords(aimDirection);
}

void SmartAlgo::blockIrrelevantDirections(int direction) {
	if (direction == RIGHT || direction == LEFT) {
		aimRange[UP] = aimRange[DOWN] = 0;
		visitCell(UP, NOT_TARGET, true);
		visitCell(DOWN, NOT_TARGET, true);
	}
	else {
		aimRange[LEFT] = aimRange[RIGHT] = 0;
		visitCell(LEFT, NOT_TARGET, true);
		visitCell(RIGHT, NOT_TARGET, true);
	}
}

void SmartAlgo::determineAimDirection() {
	int direction = NONE;
	expandOverHits();
	while (direction == NONE) {
		if (!aimRange[DOWN] && !aimRange[UP] && !aimRange[LEFT] && !aimRange[RIGHT]) {
			seekAndDestroy = false;
			break;
		}
		direction = randomGen(0, 3);
		if (!aimRange[direction]) {
			direction = NONE;
			continue;
		}
		if (aimRange[direction] == NOT_TRIED) {
			aimRange[direction] = 0;
		}
		if (!tryToExpandAimRange(direction)) {
			direction = NONE;
		}
	}
	if (direction != NONE) {
		aimDirection = direction;
	}
}

void SmartAlgo::expandOverHits() {
	for (int direction = 0; direction < 4; direction++) {
		if (!aimRange[direction]) {
			continue;
		}
		calcCurrentCoords(direction);
		if (visitCell(direction, IS_SINK, false)) {
			aimRange[UP] = aimRange[DOWN] = aimRange[LEFT] = aimRange[RIGHT] = 0;
			return;
		}
		while (visitCell(direction, IS_HIT, false)) {
			aimRange[direction] = aimRange[direction]==NOT_TRIED ? 1 : ++aimRange[direction];
			blockIrrelevantDirections(direction);
			calcCurrentCoords(direction);
		}
	}
}

bool SmartAlgo::tryToExpandAimRange(int direction) {
	calcCurrentCoords(direction);
	if (visitCell(direction, TARGET, false)) {
		aimRange[direction]++;
		return true;
	}
	else {
		aimRange[direction] = 0;
		return false;
	}
}

void SmartAlgo::calcCurrentCoords(int direction) {
	int directionSize = aimRange[direction] == NOT_TRIED ? 0 : aimRange[direction];
	currentRow = direction == DOWN ? firstHitRow - directionSize : firstHitRow;
	currentRow = direction == UP ? currentRow + directionSize : currentRow;
	currentCol = direction == LEFT ? firstHitCol - directionSize : firstHitCol;
	currentCol = direction == RIGHT ? currentCol + directionSize : currentCol;
}

IBattleshipGameAlgo* GetAlgorithm() {
	return (IBattleshipGameAlgo*) new SmartAlgo();
}