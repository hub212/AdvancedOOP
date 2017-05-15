#include "SmartAlgo.h"


int SmartAlgo::randomGen(int min, int max) {
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

	int random_integer = uni(rng);
	return random_integer;
}

bool SmartAlgo::init(const std::string& path) {



	aimRange = new int[4]{};
	initRandomTargets();
	return true;
}

void SmartAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result) {
	if (this->player_num != player) {
		int tmpRow = currentRow;
		int tmpCol = currentCol;
		currentRow = row - 1;
		currentCol = col - 1;
		int cell = possible_targets[currentRow][currentCol];
		if (result == AttackResult::Sink && cell != NOT_TARGET) {
			//the other player sinked his own ship. the cells around it are empty.
			markAdjacentCells();
		}
		if (result == AttackResult::Hit && possible_targets[currentRow][currentCol] != NOT_TARGET) {
			//the other player hit his own ship but didn't sink it. add it to the target bank.
			targetBank.resize(targetBank.size() + 1, std::vector<int>(2, 0));
			targetBank[targetBank.size() - 1][0] = currentRow;
			targetBank[targetBank.size() - 1][1] = currentCol;
		}
		removeFromRandomTargets(HERE, true);
		currentRow = tmpRow;
		currentCol = tmpCol;
		return;
	}
	removeFromRandomTargets(HERE, true);
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
	//calcAttack();
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
	} while (possible_targets[currentRow][currentCol] == NOT_TARGET && iters <= rows*cols);
}

void SmartAlgo::calcAttack() {
	if (attackSucceeded && !seekAndDestroy) {
		seekAndDestroy = true; //we hit a ship and didn't sink it - continue to target it
		firstHitRow = currentRow; //the next attacks will be centered around these coordinates
		firstHitCol = currentCol; //the next attacks will be centered around these coordinates
		aimRange[UP] = aimRange[DOWN] = aimRange[RIGHT] = aimRange[LEFT] = NOT_TRIED;
	}
	else if (attackSucceeded && seekAndDestroy) {
		blockIrrelevantDirections();
	}
	else if (!attackSucceeded && seekAndDestroy) {
		aimRange[aimDirection] = 0;
	}
	determineAimDirection();
	calcCurrentCoords(aimDirection);
}

void SmartAlgo::blockIrrelevantDirections() {
	if (aimDirection == RIGHT || aimDirection == LEFT) {
		aimRange[UP] = aimRange[DOWN] = 0;
		removeFromRandomTargets(UP, true);
		removeFromRandomTargets(DOWN, true);
	}
	else {
		aimRange[LEFT] = aimRange[RIGHT] = 0;
		removeFromRandomTargets(LEFT, true);
		removeFromRandomTargets(RIGHT, true);
	}
}

void SmartAlgo::determineAimDirection() {
	aimDirection = NONE;
	while (aimDirection == NONE) {
		if (!aimRange[DOWN] && !aimRange[UP] && !aimRange[LEFT] && !aimRange[RIGHT]) {
			seekAndDestroy = false;
			break;
		}
		aimDirection = randomGen(0, 3);
		if (!aimRange[aimDirection]) {
			aimDirection = NONE;
			continue;
		}
		if (aimRange[aimDirection] == NOT_TRIED) {
			aimRange[aimDirection] = 0;
		}
		if (!tryToExpandAimRange(aimDirection)) {
			aimDirection = NONE;
		}
	}
}

bool SmartAlgo::tryToExpandAimRange(int direction) {
	calcCurrentCoords(direction);
	if (removeFromRandomTargets(direction, false)) {
		aimRange[direction]++;
		return true;
	}
	else {
		aimRange[direction] = 0;
		return false;
	}
}

void SmartAlgo::calcCurrentCoords(int direction) {
	currentRow = direction == DOWN ? firstHitRow - aimRange[DOWN] : firstHitRow;
	currentRow = direction == UP ? currentRow + aimRange[UP] : currentRow;
	currentCol = direction == LEFT ? firstHitCol - aimRange[LEFT] : firstHitCol;
	currentCol = direction == RIGHT ? currentCol + aimRange[RIGHT] : currentCol;
}

IBattleshipGameAlgo* GetAlgorithm() {
	return (IBattleshipGameAlgo*) new SmartAlgo();
}