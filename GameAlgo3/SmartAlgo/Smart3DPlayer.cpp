#include "Smart3DPlayer.h"

int Smart3DPlayer::strategyAVictories = 0;
int Smart3DPlayer::strategyBVictories = 0;

// finds and ranks all the available move on the board at the start of the game. 
void Smart3DPlayer::findLegalAttackMoves()
{
	double moveKeyScore;
	rankedLegalAttackMoves.clear();
	for (int d = 0; d < boardDepth; d++)
	{
		for (int i = 0; i < numOfRows; i++)
		{
			for (int j = 0; j < numOfCols; j++)
			{
				Coordinate move = { i, j, d };
				if (getBoardPiece(move) == UNDISCOVERED_AREA || getBoardPiece(move) == ENEMY_SELF_HIT_AREA)//goes over all the legal moves that can be chosen by the player for farther consideration (in case its a location that contains an enemy ship that was hit by the enemy itself, a location next to that location is gonna be examined).
				{
					moveKeyScore = rankMove(move);//gets the move score as calculated in rankMove
					setBoardKeyScore(move, moveKeyScore);//updates the calculated score for this location
					rankedLegalAttackMoves.insert({ moveKeyScore,move }); //adds the location to the map that conains all legal moves sorted by their score
				}
			}
		}
	}

}

// updates the board in the given location, updates its score, and the score of the surrounding locations.
void Smart3DPlayer::setBoardPiece(Coordinate loc, char piece, bool updateRanking) {
	gameBoard[loc.depth][loc.row][loc.col] = piece; //updates the board

	if (updateRanking)//in case the scores of locations on board need to be updated as a result of this update
	{
		list<Coordinate> updateLocations;
		updateLocations.push_back(loc);
		populateUpdatableLocationsList(updateLocations, loc); //calculates all the locations that are affected by the last board update (including the updates' location itself)
		for (auto &location : updateLocations)
		{
			double moveNewScore = rankMove(location);
			double moveOldScore = getBoardKeyScore(location);
			setBoardKeyScore(location, moveNewScore);
			rankedLegalAttackMoves.erase(moveOldScore); //deletes the move and its previous score from the available moves list
			if (moveNewScore != NO_ENEMY_SHIP_SCORE) //in case the move is still available it reenters it to the available moves list with its updated key(which is its score)
			{
				rankedLegalAttackMoves.erase(moveNewScore);
				rankedLegalAttackMoves.insert({ moveNewScore ,location });
			}
		}

	}
}

//finds the locations on board whose socre needs to be updated based on the update of a given location=(row, col, depth)
void Smart3DPlayer::populateUpdatableLocationsList(list<Coordinate>& updateLocations, Coordinate loc) const
{
	populateUpdatableLocationsListInDirection(updateLocations, loc, 1, 0, 0);
	populateUpdatableLocationsListInDirection(updateLocations, loc, 0, 1, 0);
	populateUpdatableLocationsListInDirection(updateLocations, loc, 0, 0, 1);
}

// a helper function used by populateUpdatableLocationsList in order to find the locations on board whose socre needs to be updated based on the update of a given location=(row,col), in a certain direction
void Smart3DPlayer::populateUpdatableLocationsListInDirection(list<Coordinate>& updateLocations, Coordinate loc, int vertical, int horizontal, int sideways) const
{
	int startLocRow = loc.row - (MAX_SHIP_SIZE - 1)*vertical;
	int startLocCol = loc.col - (MAX_SHIP_SIZE - 1)*horizontal;
	int startLocDepth = loc.depth - (MAX_SHIP_SIZE - 1)*sideways;

	int i = 0;
	while (i < MAX_SHIP_SIZE * 2 - 1)//goes over the proximity of the given move (in a certain direction from it) and inserts all the moves that are still available to updateLocations so their score may be updated later
	{
		if ((startLocRow != loc.row || startLocCol != loc.col || startLocDepth != loc.depth) && isAvailableMove({ startLocRow, startLocCol, startLocDepth }))
			updateLocations.push_back({ startLocRow, startLocCol ,startLocDepth });

		startLocRow += vertical;
		startLocCol += horizontal;
		startLocDepth += sideways;
		i++;
	}
}

Coordinate Smart3DPlayer::calc3DCubeCenter() const
{
	int rowSum = 0, colSum = 0, depthSum = 0;
	rowSum += 4 * numOfRows;
	colSum += 4 * numOfCols;
	depthSum += 4 * boardDepth;
	return { rowSum / 8,colSum / 8,depthSum / 8 };
}

double Smart3DPlayer::mapCellToValue(Coordinate point) const
{
	Coordinate center = calc3DCubeCenter();
	point.row = point.row - center.row;
	point.col = point.col - center.col;
	point.depth = point.depth - center.depth;

	return  numOfRows*numOfCols*boardDepth - pow(point.row, 2) - pow(point.col, 2) - pow(point.depth, 2);
	//return rows*cols*depth - sqrt(max(max(abs(point.row),abs(point.col)),abs(point.depth)));
}

double Smart3DPlayer::calcFuncMinDiff() const
{
	unordered_set<double> distinctVals;
	double minDiff = DBL_MAX;
	for (int d = 0; d < boardDepth; d++) {
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				double res = mapCellToValue({ i,j,d });
				distinctVals.insert(res);
			}
		}
	}
	vector<double>orderedVals;
	orderedVals.insert(orderedVals.begin(), distinctVals.begin(), distinctVals.end());
	sort(orderedVals.begin(), orderedVals.end());
	for (size_t i = 0; i < orderedVals.size() - 1; i++)
	{
		double curDiff = abs(orderedVals[i + 1] - orderedVals[i]);
		if (curDiff < minDiff)
			minDiff = curDiff;
	}
	return minDiff;
}

//maps each location on the board to a distinct vaue such that the location in the center of the board get higher values then the edges of the board
void Smart3DPlayer::createCenteredCellToIndexMapping()
{
	unordered_set<double>indexes;
	map<double, Coordinate> rankedIndexes;
	double minDiff = calcFuncMinDiff();
	double adjustedMinVal = minDiff / (numOfRows*numOfCols*boardDepth * 2);

	for (int d = 0; d < boardDepth; d++){
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				double cellIndex = numOfRows*numOfCols*boardDepth - ((numOfRows*numOfCols)*d + numOfCols*i + j);
				double res = mapCellToValue({ i,j,d });
				int insertionRes = indexes.insert(res).second;
				if (insertionRes == 0)
				{
					indexes.insert(res + cellIndex*adjustedMinVal);
					rankedIndexes.insert({ res + cellIndex*adjustedMinVal,{ i,j,d } });
				}
				else
					rankedIndexes.insert({ res,{ i,j,d } });
			}
		}
	}
	int index = 0;
	for (auto p : rankedIndexes)
	{
		locationToIndexMap.insert({ p.second,index });
		index++;
	}
	/*for (int d = 0; d < boardDepth; d++) {
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				cout << locationToIndexMap[{i, j, d}] << " ";
			}
			cout << endl;
		}
	}*/
}

void Smart3DPlayer::createRandomCellToIndexMapping()
{
	vector<int> indexes;
	for (int d = 0; d < boardDepth; d++) {
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				int cellIndex = (numOfRows*numOfCols)*d + numOfCols*i + j;
				indexes.push_back(cellIndex);
			}
		}
	}
	static std::random_device rd;
	static auto engine = std::default_random_engine{rd()};
	std::shuffle(std::begin(indexes), std::end(indexes), engine);
	int index = 0;
	for (int d = 0; d < boardDepth; d++) {
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				locationToIndexMap.insert({ { i,j,d },indexes[index++] });
			}
		}

	}
	for (int d = 0; d < boardDepth; d++) {
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				cout << locationToIndexMap[{i, j, d}] << " ";
			}
			cout << endl;
		}
	}
}

void Smart3DPlayer::savePerviousGameInfo()
{
	int strategyAModefier = 0, strategyBModefier = 0;
	if (!isFirstGame)
	{
		if (strategyAVictories >= strategyBVictories)
			strategyAModefier = 1;
		else
			strategyBModefier =1;
		if (totalPlayersShips > 0)
		{
			strategyAVictories += strategyAModefier;
			strategyBVictories += strategyBModefier;
		}
		else
		{
			strategyAVictories -= strategyAModefier;
			strategyBVictories -= strategyBModefier;
		}
		//cout << "strategyAVictories after update=" << strategyAVictories << endl;
		//cout << "strategyBVictories after update=" << strategyBVictories << endl;
	}
	else
		isFirstGame = false;
}

// ranks a given location on board using various logic
double Smart3DPlayer::rankMove(Coordinate move) const
{
	char piece = getBoardPiece(move);
	double cellIndex;
	
	if(strategyAVictories>=strategyBVictories)
		 cellIndex = getLocationIndex(move);
	else
	{
		//cellIndex = (numOfRows*numOfCols)*move.depth + numOfCols*move.row + move.col;
		cellIndex = numOfRows*numOfCols*boardDepth - ((numOfRows*numOfCols)*move.depth + numOfCols*move.row + move.col);
	}
	double moveScore = EPSILON*cellIndex; //this initial initialization of moveScore, makes sure that all moves will have unique values. 
	double baseScore;
	int enemyProximityCount, maxShipSize;

	switch (piece)//calculates the score of this move according to the information that's known about it and its proximity
	{
		case ENEMY_SELF_HIT_AREA: //a location that contains an enemy ship that was attacked by the enemy
			baseScore = calcBaseScore(move);
			enemyProximityCount = calcEnemyProximity(move);
			moveScore += ENEMY_SHIP_SCORE + baseScore + enemyProximityCount*CONFIRMED_ENEMY_PROXIMITY_FACTOR;
			break;
		case UNDISCOVERED_AREA: //a location that wasn't attacked yet
			baseScore = calcBaseScore(move);
			enemyProximityCount = calcEnemyProximity(move);
			moveScore += baseScore + enemyProximityCount*CONFIRMED_ENEMY_PROXIMITY_FACTOR;
			maxShipSize = calcMaxPossibleShipSizeInLocation(move);//drastically lowers the score of locations on the board that can't hold the ships that persumably the enemy has left
			if (maxShipSize < smallestEnemyShipIndex + 1)
				moveScore = calcMitigatedScoreForUnlikelyLocation(moveScore);
			else
			{
#ifdef BUFFMoves
				moveScore = factorScoreForLikelyLocation(moveScore, maxShipSize);
#endif
			}
			break;
		default: //a location that was already attacked by me or by the enemy and no longer intrests me (because there is no live enemy ship next to it)
			moveScore = NO_ENEMY_SHIP_SCORE;
			break;
	}
	
	return moveScore;
}

// calculates the ammount of self hits made by the enemy near a given location.
int Smart3DPlayer::calcEnemyProximity(Coordinate loc) const
{
	int closeEnemies = 0;

	closeEnemies = isEnemySelfHitArea({ loc.row + UP, loc.col , loc.depth}) ? closeEnemies + 1 : closeEnemies;
	closeEnemies = isEnemySelfHitArea({loc.row + DOWN, loc.col, loc.depth }) ? closeEnemies + 1 : closeEnemies;
	closeEnemies = isEnemySelfHitArea({loc.row, loc.col + RIGHT, loc.depth }) ? closeEnemies + 1 : closeEnemies;
	closeEnemies = isEnemySelfHitArea({loc.row, loc.col + LEFT, loc.depth }) ? closeEnemies + 1 : closeEnemies;
	closeEnemies = isEnemySelfHitArea({ loc.row, loc.col, loc.depth + SIDEWAYS_FAR }) ? closeEnemies + 1 : closeEnemies;
	closeEnemies = isEnemySelfHitArea({ loc.row, loc.col, loc.depth + SIDEWAYS_NEAR }) ? closeEnemies + 1 : closeEnemies;

	return closeEnemies;
}

// returns the base score of a given location on board using BASE_SCORE values
double Smart3DPlayer::calcBaseScore(Coordinate loc) const
{
	int direction1Val = 0, direction2Val = 0;
	int score = 0;

	direction1Val = calcAvailableMovesInDirection(loc, UP, 0, 0);
	direction2Val = calcAvailableMovesInDirection(loc, DOWN, 0, 0);
	score = BASE_SCORE[direction1Val][direction2Val]; //gives a score to a given location based on the number of consequtive available locations in vertical direction

	direction1Val = calcAvailableMovesInDirection(loc, 0, RIGHT, 0);
	direction2Val = calcAvailableMovesInDirection(loc, 0, LEFT, 0);
	score += BASE_SCORE[direction1Val][direction2Val]; //add to the previous score for a given location location, a score that's based on the number of consequtive available locations in horizontal direction

	direction1Val = calcAvailableMovesInDirection(loc, 0, 0, SIDEWAYS_FAR);
	direction2Val = calcAvailableMovesInDirection(loc, 0, 0, SIDEWAYS_NEAR);
	score += BASE_SCORE[direction1Val][direction2Val]; //add to the previous score for a given location location, a score that's based on the number of consequtive available locations in horizontal direction

	return score;
}

// a helper function for calcBaseScore. Calculates the number of consequtive available locations on board (distance of up to SHIP_MAX_SIZE -1 from a given location)
int Smart3DPlayer::calcAvailableMovesInDirection(Coordinate loc, int vertical, int horizontal, int sideways) const
{
	int moveCounter = 0;

	while (moveCounter < MAX_SHIP_SIZE - 1)
	{
		if (!isAvailableMove({ loc.row + vertical, loc.col + horizontal, loc.depth + sideways }))
			return moveCounter;
		loc.row += vertical;
		loc.col += horizontal;
		loc.depth += sideways;
		moveCounter++;
	}
	return moveCounter;
}

void Smart3DPlayer::createCellToIndexMapping()
{
	//createCenteredCellToIndexMapping();//maps each location on the board to a distinct vaue
	createRandomCellToIndexMapping();
}

//creates the board and then initializes it according to the board passed by the GameManager, so all the locations that can't hold an enemy ship according to the rules, are marked as such
void Smart3DPlayer::setBoard(const BoardData& board)
{
	savePerviousGameInfo();
	resetGameState();
	createGameBoard(board);//initializes the board

	createCellToIndexMapping();
	EPSILON = 1.0 / (numOfRows*numOfCols*boardDepth * 2);// set a value to EPSILON based on the board size. Such that (EPSILON*numOfRows*numOfCols*boardDepth) < 1
	
	for (int d = 0; d < boardDepth; d++)
	{
		for (int i = 0; i < numOfRows; i++)//sets default values to all locations on board
		{
			for (int j = 0; j < numOfCols; j++)
			{
				setBoardPiece({ i, j, d }, 'f', false);
				setBoardKeyScore({ i, j, d }, -1);
			}
		}
	}
	for (int d = 0; d < boardDepth; d++)
	{
		for (int i = 0; i < numOfRows; i++)//marks all the locations that can't hold an enemy ship according to the rules
		{
			for (int j = 0; j < numOfCols; j++)
			{
				if (board.charAt(Coordinate(i,j,d)) == UNDISCOVERED_AREA && getBoardPiece(Coordinate(i, j, d)) != DISCOVERED_AREA)
					setBoardPiece({ i, j, d }, UNDISCOVERED_AREA, false);
				else if (board.charAt(Coordinate(i, j, d)) != UNDISCOVERED_AREA)
				{
					setBoardPiece({ i, j, d }, DISCOVERED_AREA, false);
					markSurrounding({ i, j, d }, false);
				}
			}
		}
	}
	findLegalAttackMoves(); //ranks all the available moves on board
}

double Smart3DPlayer::calcMitigatedScoreForUnlikelyLocation(double curScore) const
{
	return curScore / UNLIKELY_SHIP_LOCATION_MITIGATION;
}

double Smart3DPlayer::factorScoreForLikelyLocation(double curScore, int maxShipSize) const
{
	int curAvailableShipsUpToSize = calcNumOfShipsUpToGivenSize(maxShipSize);
	int totalShipsLeft = calcNumOfShipsUpToGivenSize(MAX_SHIP_SIZE);
	double factor = 1.0 / totalShipsLeft;
	double factoredScore = factor*curAvailableShipsUpToSize*curScore + 1;
	return factoredScore;
}

int Smart3DPlayer::calcNumOfShipsUpToGivenSize(int size) const
{
	int shipCounter = 0;
	for (int i = 0; i < MAX_SHIP_SIZE; i++)
	{
		if (size >= i + 1)
			shipCounter += shipTypes[i];
		else
			return shipCounter;
	}
	return shipCounter;
}

//calculates the maximum ship size that can fit in a given location
int Smart3DPlayer::calcMaxPossibleShipSizeInLocation(Coordinate loc) const
{
	int direction1Val = 0, direction2Val = 0;
	int maxSize = 1, temp=1;

	direction1Val = calcAvailableMovesInDirection(loc, UP, 0, 0);
	direction2Val = calcAvailableMovesInDirection(loc, DOWN, 0, 0);
	maxSize += direction1Val + direction2Val;

	direction1Val = calcAvailableMovesInDirection(loc, 0, RIGHT, 0);
	direction2Val = calcAvailableMovesInDirection(loc, 0, LEFT, 0);
	temp += direction1Val + direction2Val;
	maxSize = temp > maxSize ? temp : maxSize;
	
	temp = 1;
	direction1Val = calcAvailableMovesInDirection(loc, 0, 0, SIDEWAYS_FAR);
	direction2Val = calcAvailableMovesInDirection(loc, 0, 0, SIDEWAYS_NEAR);
	temp += direction1Val + direction2Val;
	maxSize = temp > maxSize ? temp : maxSize;

	return maxSize;
}

//resets all parameters in case this instance of Smart3DPlayer was used for previous games
void Smart3DPlayer::resetGameState()
{
	reverseLegalAttackMoves.clear();
	resetAttackParameters();
	rankedLegalAttackMoves.clear();
	smallestEnemyShipIndex = 0;
	shipTypes = { 0,0,0,0 };
	locationToIndexMap.clear();
	totalPlayersShips = 0;
}

// prints the gameboard
void Smart3DPlayer::printBoard() const {
	for (int d = 0; d < boardDepth; d++)
	{
		cout << "depth: " << d << endl;
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				cout << gameBoard[d][i][j];
			}
			cout << endl;
		}
	}
}

// prints the score board - the score given to each location on the board
void Smart3DPlayer::printScoreBoard() const {
	for (int d = 0; d < boardDepth; d++)
	{
		cout << "depth: " << d << endl;
		for (int i = 0; i < numOfRows; i++) {
			for (int j = 0; j < numOfCols; j++) {
				cout << getBoardKeyScore({ i, j, d }) << " ";
			}
			cout << endl;
		}
	}
}

// marks locations on the board adjacent to the given location (i,j,d) as locations that can't hold enemy ship (based on the game rules). The updateRanking variable indicates whether to reclaculate the score of the updated locations
void Smart3DPlayer::markSurrounding(Coordinate loc, bool updateRanking) {
	setBoardPiece(loc, DISCOVERED_AREA, updateRanking);
	if (isValidRow(loc.row - 1 ))
		setBoardPiece({ loc.row - 1, loc.col, loc.depth }, DISCOVERED_AREA, updateRanking);
	if (isValidRow(loc.row + 1))
		setBoardPiece({ loc.row + 1, loc.col, loc.depth }, DISCOVERED_AREA, updateRanking);
	if (isValidCol(loc.col - 1))
		setBoardPiece({ loc.row, loc.col - 1, loc.depth }, DISCOVERED_AREA, updateRanking);
	if (isValidCol(loc.col + 1))
		setBoardPiece({ loc.row, loc.col + 1, loc.depth }, DISCOVERED_AREA, updateRanking);
	if (isValidDepth(loc.depth - 1))
		setBoardPiece({ loc.row, loc.col, loc.depth - 1 }, DISCOVERED_AREA, updateRanking);
	if (isValidDepth(loc.depth + 1))
		setBoardPiece({ loc.row, loc.col, loc.depth + 1 }, DISCOVERED_AREA, updateRanking);
}

Coordinate Smart3DPlayer::attack()
{
	Coordinate candidateAttackMove{0,0,0}; 
	horizontalModefier = 0;
	verticalModefier = 0;
	sidewaysModefier = 0;

	if (!isAttacking)//in case the algo is in search mode after an enemy ship
		candidateAttackMove = findAttackMoveByScore();
	else//in case an enemy ship was already detected
		candidateAttackMove = findAttackMoveByDirectionAndScore();
	if (!isEnemySelfHitArea(candidateAttackMove)) //if the chosen move IS NOT a location that was previously attacked by the enemy and help its own ship
		return { candidateAttackMove.row + 1, candidateAttackMove.col + 1 , candidateAttackMove.depth + 1};
	else
		isAttacking = true;

	do {
		registerHit(candidateAttackMove, verticalModefier, horizontalModefier, sidewaysModefier, attackedShip); //marks the location as already attacked and documents it in attackedShip
		if (!isDirectionDetermined && attackedShip.size() > 1)
			determineAndSetAttackDirection(); //decides the direction of the enemy ship
		candidateAttackMove = findAttackMoveByDirectionAndScore();
	} while (isEnemySelfHitArea(candidateAttackMove)); //searches until a viable attack location is found

	return{ candidateAttackMove.row + 1, candidateAttackMove.col + 1 , candidateAttackMove.depth + 1 }; //Added 1 to each coordinate adjust for GameManager expected values
}

// returns the best available move to make another hit on an already attacked enemy ship, based on direction of the attacked ship and the score.
Coordinate Smart3DPlayer::findAttackMoveByDirectionAndScore()
{
	map<double, AttackMove> candidateAttackMoves;

	if (isDirectionDetermined)//means that there were at least 2 hits and the ship wasn't sunk yet
	{
		switch (shipDirection)
		{
		case Direction::Horizontal:
			findCandidateAttackMovesInDirection(candidateAttackMoves, 0, 1, 0);
			break;
		case Direction::Vertical:
			findCandidateAttackMovesInDirection(candidateAttackMoves, 1, 0, 0);
			break;
		case Direction::Sideways:
			findCandidateAttackMovesInDirection(candidateAttackMoves, 0, 0, 1);
			break;
		}
	}
	else//means that there was only 1 hit and the ship wasn't sunk yet
	{
		findCandidateAttackMovesInDirection(candidateAttackMoves, 0, 1, 0);
		findCandidateAttackMovesInDirection(candidateAttackMoves, 1, 0, 0);
		findCandidateAttackMovesInDirection(candidateAttackMoves, 0, 0, 1);
	}
	horizontalModefier = candidateAttackMoves.rbegin()->second.horizontal;
	verticalModefier = candidateAttackMoves.rbegin()->second.vertical;
	sidewaysModefier = candidateAttackMoves.rbegin()->second.sideways;
	return candidateAttackMoves.rbegin()->second.move;
}

// a helper function for findAttackMoveByDirectionAndScore. 
void Smart3DPlayer::findCandidateAttackMovesInDirection(map<double, AttackMove>& candidateAttackMoves, int vertical, int horizontal, int sideways) const
{
	Coordinate move = attackedShip.back();

	if (isAvailableMove({ move.row + vertical, move.col + horizontal, move.depth + sideways }))
	{
		move = { move.row + vertical, move.col + horizontal, move.depth + sideways };
		candidateAttackMoves.insert({ getBoardKeyScore(move), AttackMove(move, horizontal, vertical, sideways) });
	}
	move = attackedShip.front();
	if (isAvailableMove({ move.row - vertical, move.col - horizontal, move.depth - sideways }))
	{
		move = { move.row - vertical, move.col - horizontal , move.depth - sideways };
		candidateAttackMoves.insert({ getBoardKeyScore(move), AttackMove(move, -horizontal, -vertical, -sideways) });
	}
}

// returns the best available move that will lead to the discovery of an enemy ship (based on a score given to each move)
Coordinate Smart3DPlayer::findAttackMoveByScore() const
{
	if (rankedLegalAttackMoves.size()>0)
		return rankedLegalAttackMoves.rbegin()->second;
	return InvalidAttack;
}

void Smart3DPlayer::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	bool isAlreadyDiscovered = false;
	int row = move.row -1, col = move.col -1, depth = move.depth -1; //Adjust GameManager values

	if (!isLegalPosition({ row, col, depth }))
		return;
	if (getBoardPiece({ row, col, depth }) != DISCOVERED_AREA)
		setBoardPiece({ row, col, depth }, DISCOVERED_AREA);
	else
		isAlreadyDiscovered = true;

	switch (result)
	{
		case AttackResult::Miss:
			break;
		case AttackResult::Sink:
			handleSink({ row, col, depth }, player, isAlreadyDiscovered);
			break;
		case AttackResult::Hit:
			handleHit({ row, col, depth }, player, isAlreadyDiscovered);
			break;
	}
#ifdef DEBUG 
	if (player == playerID)
	{
		cout << "player " << playerID << endl;
		printBoard();
		printScoreBoard();
	}
#endif // DEBUG 

}

void Smart3DPlayer::setPlayer(int player)
{
	playerID = player;
}

// after a successful sink of an enemy ship (by me or the enemy), marks all the relevent surrounding of the ship so the algorithem will learn from 
// this too about locations that can't hold an enemy ship. Also resets some parameters in case i was also attacking that ship
void Smart3DPlayer::handleSink(Coordinate loc, int player, bool isAlreadyDiscovered)
{
	if (player == playerID)
	{
		registerHit(loc, verticalModefier, horizontalModefier, sidewaysModefier, attackedShip);
		handleSinkHelper(true, attackedShip);
	}
	else if (!isAlreadyDiscovered)//if isAlreadyDiscovered==true then it's my ship
	{
		list<Coordinate> curSunkShip;
		bool isCurrentlyAttackedShip = findEnemySunkShip(loc, curSunkShip);
		if (isCurrentlyAttackedShip)
			handleSinkHelper(true, attackedShip);
		else
			handleSinkHelper(false, curSunkShip);
	}
	else
		totalPlayersShips--;//documents that players' ship was sunk
}

// handles a successful hit - be it a self hit by the enemy or a hit on an enemy ship by the Smart3DPlayer
void Smart3DPlayer::handleHit(Coordinate loc, int player, bool isAlreadyDiscovered)
{
	if (playerID == player)
	{
		registerHit(loc, verticalModefier, horizontalModefier, sidewaysModefier, attackedShip);
		if (!isAttacking)//means it's the first hit
			isAttacking = true;
		else
		{//means it's at least the second hit
			if (!isDirectionDetermined)
				determineAndSetAttackDirection();
		}
	}
	else
	{//to learn from opponent's self hit
		if (!isAlreadyDiscovered)
			setBoardPiece(loc, ENEMY_SELF_HIT_AREA);
	}
}

//update numOfRows, numOfCols, boardDepth and allocate the board. Also counts the players' ships by type
void Smart3DPlayer::createGameBoard(const BoardData & board)
{
	gameBoard.clear();
	numOfRows = board.rows();
	numOfCols = board.cols();
	boardDepth = board.depth();

	gameBoard.resize(boardDepth);
	for (int d = 0; d < boardDepth; d++)
	{
		gameBoard[d].resize(numOfRows);
		for (int i = 0; i < numOfRows; i++) {
			gameBoard[d][i].resize(numOfCols);
			for (int j = 0; j < numOfCols; j++) {
				char curchar = board.charAt(Coordinate(i, j, d));
				gameBoard[d][i][j] = curchar;
				updateShipCount(curchar);
			}
		}
	}
	for (int i=0;i<MAX_SHIP_SIZE;i++) //converts from number of ship parts of each type, to how many ships are there from that type.
	{
		shipTypes[i] = shipTypes[i] / (i + 1);
		if (shipTypes[smallestEnemyShipIndex] == 0)
			smallestEnemyShipIndex++;
	}
	totalPlayersShips = calcNumOfShipsUpToGivenSize(MAX_SHIP_SIZE);//calc how many ships its persumed the enemy has (based on the amount of ships this player has)
}

void Smart3DPlayer::updateShipCount(char cur)
{
	switch (cur)
	{
	case 'B':
	case 'b':
		shipTypes[B_LENGTH - 1]++;
		break;
	case  'P':
	case  'p':
		shipTypes[P_LENGTH - 1]++;
		break;
	case 'M':
	case 'm':
		shipTypes[M_LENGTH - 1]++;
		break;
	case 'D':
	case 'd':
		shipTypes[D_LENGTH - 1]++;
		break;
	default:
		break;
	}
}

// a helper function for handleSink (helps to handle a situatioin when a ship is sunk)
void Smart3DPlayer::handleSinkHelper(bool isCurrentlyAttackedShip, list<Coordinate>& desiredSunkShip)
{
	for (Coordinate loc : desiredSunkShip)//marks all the locations of the sunk ship and their surrounding as locations that can't hold an enemy ship, so they aren't attacked again
	{
		markSurrounding(loc);
	}
	shipTypes[desiredSunkShip.size() - 1]--; //update the ammount of ships of that size that the enemy has (according to the assumption that both the players have the same ships)
	while (smallestEnemyShipIndex < shipTypes.size() && shipTypes[smallestEnemyShipIndex] == 0)//find the smallest ship that the enemy still has
	{
		smallestEnemyShipIndex++;
	}
	findLegalAttackMoves();
	if (isCurrentlyAttackedShip)
		resetAttackParameters();
	else
		desiredSunkShip.clear();
}

// resets variables after a successful sink of an enemy ship
void Smart3DPlayer::resetAttackParameters()
{
	isAttacking = false;
	isDirectionDetermined = false;
	shipDirection = Direction::Horizontal;
	attackedShip.clear();
}

// handles a situation when an enemy sinks its own ship, and discoveres all the locations on the board containing the sunk ship. 
bool Smart3DPlayer::findEnemySunkShip(Coordinate loc, list<Coordinate>& curSunkShip)
{
	bool isDirection1 = false, isDirection2=false;
	registerHit(loc, 0, 0, 0, curSunkShip);

	isDirection1 = findEnemyAttackedLocationInGivenDirection(loc, 1, 0, 0, curSunkShip); //searches for the ship in a vertical position - DOWN
	isDirection2 = findEnemyAttackedLocationInGivenDirection(loc, -1, 0, 0, curSunkShip); //searches for the ship in a vertical position - UP
	if (!(isDirection1 || isDirection2)) //if the ship was't vertical then look for the ship in a horizontal position
	{
		isDirection1 = findEnemyAttackedLocationInGivenDirection(loc, 0, 1, 0, curSunkShip); //searches for the ship in a horizontal position
		isDirection2 = findEnemyAttackedLocationInGivenDirection(loc, 0, -1, 0, curSunkShip); //searches for the ship in a horizontal position
		if (!(isDirection1 || isDirection2)) //if the ship was't vertical nor horizontal, then look for the ship in a sideways position
		{
			isDirection1 = findEnemyAttackedLocationInGivenDirection(loc, 0, 0, 1, curSunkShip); //searches for the ship in a sideways position
			isDirection2 = findEnemyAttackedLocationInGivenDirection(loc, 0, 0, -1, curSunkShip); //searches for the ship in a sideways position
		}
	}
	if (isAttacking) //if there is a chance that the currently sunk ship is the ship i was also attacking
	{
		bool isPart = isPartOfCurrentlyAttackedShip(attackedShip.front(), curSunkShip.back());
		if (isPart)
		{
			attackedShip.insert(attackedShip.begin(), curSunkShip.begin(), curSunkShip.end());
		}
		else if (isPartOfCurrentlyAttackedShip(attackedShip.back(), curSunkShip.front()))
		{
			isPart = true;
			attackedShip.insert(attackedShip.end(), curSunkShip.begin(), curSunkShip.end());
		}
		return isPart;
	}

	return false;
}

//a helper function of findEnemySunkShip. Finds parts of an enemy ship in a given direction (UP, DOWN, LEFT, RIGHT, Sideways)
bool Smart3DPlayer::findEnemyAttackedLocationInGivenDirection(Coordinate loc, int vertical, int horizontal,int sideways, list<Coordinate>& curSunkShip)
{
	bool wasFound = false;

	while (isEnemySelfHitArea({ loc.row + vertical, loc.col + horizontal, loc.depth + sideways }))
	{
		loc.row += vertical;
		loc.col += horizontal;
		loc.depth += sideways;
		registerHit(loc, vertical, horizontal, sideways, curSunkShip);
		wasFound = true;
	}

	return wasFound;
}

// checks whether ship1 and ship2 locations belong to the dame ship
bool Smart3DPlayer::isPartOfCurrentlyAttackedShip(Coordinate ship1, Coordinate ship2) const
{
	if (isPartOfCurrentlyAttackedShipInDirection(ship1,ship2, 1,0,0) || isPartOfCurrentlyAttackedShipInDirection(ship1, ship2, 0, 1, 0) || isPartOfCurrentlyAttackedShipInDirection(ship1, ship2, 0, 0, 1))
		return true;
	return false;
}

//checks whether two ships have a common coordinate in a given direction
bool Smart3DPlayer::isPartOfCurrentlyAttackedShipInDirection(Coordinate ship1, Coordinate ship2, int vertical, int horizontal, int sideways) const
{
	int row = ship2.row;
	int col = ship2.col;
	int depth = ship2.depth;
	return (ship1.row + vertical == row && ship1.col + horizontal == col && ship1.depth + sideways == depth) || (ship1.row - vertical == row && ship1.col - horizontal == col && ship1.depth - sideways == depth);
}

// finds the direction of the currently attacked ship based on the second hit on that ship
void Smart3DPlayer::determineAndSetAttackDirection() {
	if (horizontalModefier == RIGHT || horizontalModefier == LEFT)
		shipDirection = Direction::Horizontal;
	else if (verticalModefier == UP || verticalModefier == DOWN)
		shipDirection = Direction::Vertical;
	else
		shipDirection = Direction::Sideways;

	isDirectionDetermined = true;
}

// inserts a newly discovered location of the currently attacked enemy ship into attackedShip
void Smart3DPlayer::registerHit(Coordinate hit, int verModefier, int horModefier, int sideModefier, list<Coordinate>& curShip) {
	if (horModefier == RIGHT || verModefier == DOWN || sideModefier == SIDEWAYS_FAR) //if this location is to the right of the ship or bellow the ship or to the far side of the ship
	{
		curShip.push_back(hit);
	}
	else //otherwise
	{
		curShip.push_front(hit);
	}
}

// required to use a map with Coordinate 
bool operator<(const Coordinate& c1, const Coordinate& c2) {
	if (c1.col == c2.col) {
		if (c1.row == c2.row) {
			return c1.depth < c2.depth;
		}
		return c1.row < c2.row;
	}
	return c1.col < c2.col;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *smartPlayer = new Smart3DPlayer;
	return smartPlayer;
}
