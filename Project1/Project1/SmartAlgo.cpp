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
	setNextAttack();
	return true;
}

void SmartAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result) {
	if (this->player_num != player) {
		int tmpRow = currentRow;
		int tmpCol = currentCol;
		currentRow = row-1;
		currentCol = col-1;
		removeFromRandomTargets(HERE);
		if (result == AttackResult::Sink && possible_targets[currentRow][currentCol] != NOT_TARGET) {
			//the other player sinked his own ship. the cells around it are empty.
			markAdjacentCells();
		}
		if (result == AttackResult::Hit && possible_targets[currentRow][currentCol] != NOT_TARGET) {
			//the other player hit his own ship but didn't sink it. add it to the target bank.
			targetBank.resize(targetBank.size()+1, std::vector<int>(2, 0));
			targetBank[targetBank.size() - 1][0] = currentRow;
			targetBank[targetBank.size() - 1][1] = currentCol;
		}
		currentRow = tmpRow;
		currentCol = tmpCol;
		return;
	}
	if (result == AttackResult::Miss) {
		if (!seekAndDestroy && targetBank.size()>0) {
			//we are not busy chasing after a ship, so load a target from the target bank.
			attackSucceeded = true;
			currentRow = targetBank[targetBank.size()][0]; //the next attacks will be centered around these coordinates
			currentCol = targetBank[targetBank.size()][1]; //the next attacks will be centered around these coordinates
			targetBank.resize(targetBank.size()-1);
		}
		else {
			shipSinked = false;
			attackSucceeded = false;
		}
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
	if (aimRange[UP] == 0 && aimRange[DOWN] == 0 && aimRange[RIGHT] == 0 && aimRange[LEFT] == 0) {
		seekAndDestroy = false;
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
	bool chooseHorizontalAxis = (axisFound && horizontalAxis) || (!axisFound && randomGen(0, 1));
	if (chooseHorizontalAxis) {
		helpDetermineAimDirection(LEFT, RIGHT);
	}
	else {
		helpDetermineAimDirection(UP, DOWN);
	}
}

void SmartAlgo::helpDetermineAimDirection(int direction1, int direction2) {
	if (aimRange[direction1] && aimRange[direction2]) {
		randVar = randomGen(0, 1);
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


IBattleshipGameAlgo* GetAlgorithm() {
	return (IBattleshipGameAlgo*) new SmartAlgo();
}