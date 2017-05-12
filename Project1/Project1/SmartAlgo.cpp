#include "SmartAlgo.h"

bool SmartAlgo::init(const std::string& path) {



	aimRange = new int[4]{};
	initRandomTargets();
	setNextAttack();
	return true;
}

void SmartAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result) {
	if (this->player_num != player) {
		return;
	}
	if (result == AttackResult::Miss) {
		shipSinked = false;
		attackSucceeded = false;
	}
	else if (result == AttackResult::Hit) {
		shipSinked = false;
		attackSucceeded = true;
	}
	else if (result == AttackResult::Sink) {
		shipSinked = true;
		attackSucceeded = true;
	}
	calcStateAfterAttack();
}

SmartAlgo::SmartAlgo()
{
}


SmartAlgo::~SmartAlgo()
{
	if (randomSpots != nullptr) {
		for (int i = 0; i < rows*cols; i++) {
			delete[] possible_targets[i];
		}
		delete[] possible_targets;
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
	randomSpots = new int*[rows*cols];
	for (row = 0; row<rows; row++) {
		for (col = 0; col<cols; col++) {
			cnt++;
			randomSpots[cnt] = new int[2];
			randomSpots[cnt][0] = row;
			randomSpots[cnt][1] = col;
		}
	}
	int tmpPair[2] = {};
	for (int shuffleIndex = rows*cols - 1; shuffleIndex > 0; shuffleIndex--) {
		randVar = rand() % (shuffleIndex + 1);
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
		randomSpotIndex = randomSpotIndex<rows*cols - 1 ? randomSpotIndex++ : 0;
	} while (possible_targets[currentRow][currentCol] == NOT_TARGET && iters <= rows*cols);
}

void SmartAlgo::calcStateAfterAttack() {
	if (shipSinked) {
		seekAndDestroy = false;
		markAdjacentCells();
	}
	else if (attackSucceeded && !seekAndDestroy) {
		seekAndDestroy = true; //we hit a ship and didn't sink it - continue to target it until it's sinked
		axisFound = false; //we still don't know whether the current ship is vertical or horizontal
		firstHitRow = currentRow; //the next attacks will be centered around these coordinates
		firstHitCol = currentCol; //the next attacks will be centered around these coordinates
		aimRange[UP] = aimRange[DOWN] = aimRange[RIGHT] = aimRange[LEFT] = 0;
		tryToExpandAimRange(UP); //check whether the top neighbor of the attacked cell is a potential target
		tryToExpandAimRange(DOWN); //check whether the bottom neighbor of the attacked cell is a potential target
		tryToExpandAimRange(RIGHT); //check whether the right neighbor of the attacked cell is a potential target
		tryToExpandAimRange(LEFT); //check whether the left neighbor of the attacked cell is a potential target
	}
	else if (attackSucceeded && seekAndDestroy) {
		if (!axisFound) { //this is the second hit on this ship, so we can deduce the ship's axis (vertical or horizontal)
			blockIrrelevantDirections();
		}
		tryToExpandAimRange(aimDirection);
		markAdjacentCells();
	}
	else if (!attackSucceeded && seekAndDestroy) {
		aimRange[aimDirection] = 0;
	}
	setNextAttack();
}

void SmartAlgo::tryToExpandAimRange(int direction) {
	if (removeFromRandomTargets(direction)) {
		aimRange[direction]++;
	}
	else {
		aimRange[direction] = 0;
	}
}

void SmartAlgo::setNextAttack() {
	if (seekAndDestroy) {
		determineAimDirection();
		currentRow = aimDirection == DOWN ? firstHitRow - aimRange[DOWN] : firstHitRow;
		currentRow = aimDirection == UP ? currentRow + aimRange[UP] : currentRow;
		currentCol = aimDirection == LEFT ? firstHitCol - aimRange[LEFT] : firstHitCol;
		currentCol = aimDirection == RIGHT ? currentCol + aimRange[RIGHT] : currentCol;
	}
	else {
		pickRandTarget();
	}
	removeFromRandomTargets(HERE);
}

void SmartAlgo::determineAimDirection() {
	if (!axisFound) {
		verticalAxis = aimRange[UP] || aimRange[DOWN];
		horizontalAxis = aimRange[RIGHT] || aimRange[LEFT];
		axisFound = !verticalAxis || !horizontalAxis;
	}
	if (!axisFound) {
		horizontalAxisScore = aimRange[RIGHT] + aimRange[LEFT] - (aimRange[UP] + aimRange[DOWN]); //all elements in "aimRange" are either 0 or 1 here
	}
	bool chooseHorizontalAxis = (axisFound && horizontalAxis) || (!axisFound && (horizontalAxisScore > 0 || (horizontalAxisScore == 0 && (rand() % 2))));
	if (chooseHorizontalAxis) {
		helpDetermineAimDirection(LEFT, RIGHT);
	}
	else {
		helpDetermineAimDirection(UP, DOWN);
	}
}

void SmartAlgo::helpDetermineAimDirection(int direction1, int direction2) {
	if (aimRange[direction1] && aimRange[direction2]) {
		randVar = rand() % 2;
		aimDirection = randVar == 0 ? direction1 : direction2;
	}
	else {
		aimDirection = aimRange[direction1] ? direction1 : direction2;
	}
}

void SmartAlgo::blockIrrelevantDirections() {
	if (aimDirection == RIGHT || aimDirection == LEFT) {
		aimRange[UP] = aimRange[DOWN] = 0;
	}
	else {
		aimRange[LEFT] = aimRange[RIGHT] = 0;
	}
}

