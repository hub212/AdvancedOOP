#pragma once
#include "IBattleshipGameAlgo.h"
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>
using namespace std;

bool operator<(const Coordinate& c1, const Coordinate& c2);
//#define DEBUG
//#define BUFFMoves

class Smart3DPlayer : public IBattleshipGameAlgo {
	class AttackMove {//helper class, to document a potential attack move and its direction in relation to the currently attacked ship
		Coordinate move;
		int horizontal;
		int vertical;
		int sideways;
		AttackMove(Coordinate _move, int _horizontal, int _vertical, int _sideways) : move(_move), horizontal(_horizontal), vertical(_vertical), sideways(_sideways){}
		friend class Smart3DPlayer;
	};
	enum class Direction
	{
		Horizontal,
		Vertical,
		Sideways
	};
	static int strategyAVictories; //A cell index selection stratagy that's based on prioritizing the center over the edges
	static int strategyBVictories;//A cell index selection strategy linearly starts allocating indexes from one edge of the board to the other, thus prioritizing on edge over the center and over the other edge
	static const int B_LENGTH = 1;
	static const int P_LENGTH = 2;
	static const int M_LENGTH = 3;
	static const int D_LENGTH = 4;
	const Coordinate InvalidAttack{ -1,-1,-1 };
	vector<std::vector<std::vector<char>>> gameBoard; //to change it to unique_ptr
	int numOfRows = 0;
	int numOfCols = 0;
	int boardDepth = 0;
	static const int UP = -1;
	static const int DOWN = 1;
	static const int RIGHT = 1;
	static const int LEFT = -1;
	static const int SIDEWAYS_NEAR = -1;
	static const int SIDEWAYS_FAR = 1;
	static const int ENEMY_SHIP_SCORE = 10000;
	static const int UNLIKELY_SHIP_LOCATION_MITIGATION = 1000;
	static const int NO_ENEMY_SHIP_SCORE = -1;
	static const int CONFIRMED_ENEMY_PROXIMITY_FACTOR = 25;
	const int BASE_SCORE[4][4] = { { 1, 2, 3, 4 }, { 2, 4, 6, 7 }, { 3, 6, 8, 9 }, { 4, 7, 9, 10 } }; //likelihood of enemy ship in a given square based on its surrounding (how many undiscovered locations are there in its proximity)
	double EPSILON = 0; //unique value for each location on the board, to make sure no two different locations have the same score. 
	static const char DISCOVERED_AREA = 'X';
	static const char UNDISCOVERED_AREA = ' ';
	static const char ENEMY_SELF_HIT_AREA = 'S';
	static const int MAX_SHIP_SIZE = 4;
	int playerID; //the id given by the GameManager - 0 or 1
	map<double, Coordinate> rankedLegalAttackMoves; //all the available moves sorted by their score 
	map<Coordinate, double> reverseLegalAttackMoves; //holds the moves as as they key, and the score of those moves as the value (the reverse of rankedLegalAttackMoves)
	list<Coordinate> attackedShip;//holds the destroyed locations of the currently attacked enemy ship. front = left or up or near part of the ship. back = right or down or far part of the ship.
	bool isAttacking = false;//indicates whether an enemy ship was already discovered but not yet sunk
	//bool isVerticalShip = false; //indicates whether the attacked ship diraction is vertical
	Direction shipDirection= Direction::Horizontal;
	bool isDirectionDetermined = false;////indicates whether the attacked ship diraction diraction was determined
	int horizontalModefier = 0;// indicates the diraction of the current attack on the enemy ship - meant to figure out its diraction
	int verticalModefier = 0;// indicates the diraction of the current attack on the enemy ship -  meant to figure out its diraction
	int sidewaysModefier = 0;// indicates the diraction of the current attack on the enemy ship -  meant to figure out its diraction
	vector<int> shipTypes{ 0,0,0,0 };
	int smallestEnemyShipIndex = 0;
	map<Coordinate, int> locationToIndexMap;
	int totalEnemyShips = 0;//holds the total number of enemy ships its assumed it has at the start of the game

	int calcEnemyProximity(Coordinate loc) const; // calculates the ammount of self hits made by the enemy near a given location. 
	void findLegalAttackMoves();// finds and ranks all the available move on the board at the start of the game. 
	void resetAttackParameters();// resets variables after a successful sink of an enemy ship
	void registerHit(Coordinate hit, int verModefier, int horModefier, int sideModefier, list<Coordinate>& curShip);// inserts a newly discovered location of the currently attacked enemy ship into attackedShip
	void markSurrounding(Coordinate loc, bool updateRanking = true); // marks locations on the board adjacent to the given location (i,j,d) as locations that can't hold enemy ship (based on the game rules). The updateRanking variable indicates whether to reclaculate the score of the updated locations
	Coordinate findAttackMoveByScore() const;// returns the best available move that will lead to the discovery of an enemy ship (based on a score given to each move)
	bool findEnemySunkShip(Coordinate loc, list<Coordinate>& curSunkShip); // handles a situation when an enemy sinks its own ship, and discoveres all the locations on the board containing the sunk ship. 
	void handleSink(Coordinate loc, int player, bool isAlreadyDiscovered);// after a successful sink of an enemy ship (by me or the enemy), marks all the relevent surrounding of the ship so the algorithem will learn from this too about locations that can't hold an enemy ship. Also resets some parameters in case i was also attacking that ship
	void handleSinkHelper(bool isCurrentlyAttackedShip, list<Coordinate>& desiredSunkShip);// a helper function for handleSink (helps to handle a situatioin when a ship is sunk)
	bool findEnemyAttackedLocationInGivenDirection(Coordinate loc, int vertical, int horizontal, int sideways, list<Coordinate>& curSunkShip);//a helper function of findEnemySunkShip. Finds parts of an enemy ship in a given direction (UP, DOWN, LEFT, RIGHT, SideWays)
	void printBoard() const; // prints the gameboard
	bool isPartOfCurrentlyAttackedShip(Coordinate ship1, Coordinate ship2) const;// tells whether ship1 and ship2 locations belong to the dame ship
	void determineAndSetAttackDirection(); // finds the direction of the currently attacked ship based on the second hit on that ship
	int calcMaxPossibleShipSizeInLocation(Coordinate loc) const; //calculates the maximum ship size that can fit in a given location
	int calcAvailableMovesInDirection(Coordinate loc, int vertical, int horizontal, int sideways) const;// a helper function for calcBaseScore. Calculates the number of consequtive available locations on board (distance of up to SHIP_MAX_SIZE -1 from a given location)
	double rankMove(Coordinate move) const;// ranks a given location on board using various logic
	double calcBaseScore(Coordinate loc) const;// returns the base score of a given location on board using BASE_SCORE values
	void setBoardPiece(Coordinate loc, char piece, bool updateRanking = true);// updates the board in the given location, updates its score, and the score of the surrounding locations.
	void printScoreBoard() const; // prints the score board - the score given to each location on the board
	void populateUpdatableLocationsList(list<Coordinate>& updateLocations, Coordinate loc) const; //finds the locations on board whose socre needs to be updated based on the update of a given location=(row, col, depth)
	void populateUpdatableLocationsListInDirection(list<Coordinate>& updateLocations, Coordinate loc, int vertical, int horizontal, int sideways) const;// a helper function used by populateUpdatableLocationsList in order to find the locations on board whose socre needs to be updated based on the update of a given location=(row ,col, depth), in a certain direction
	Coordinate findAttackMoveByDirectionAndScore();// returns the best available move to make another hit on an already attacked enemy ship, based on direction of the attacked ship and the score.
	void findCandidateAttackMovesInDirection(map<double, AttackMove>& candidateAttackMoves, int vertical, int horizontal, int sideways) const;// a helper function for findAttackMoveByDirectionAndScore. 
	void handleHit(Coordinate loc, int player, bool isAlreadyDiscovered); // handles a successful hit - be it a self hit by the enemy or a hit on an enemy ship by the Smart3DPlayer
	void createGameBoard(const BoardData& board);
	void resetGameState();//resets all parameters in case this instance of Smart3DPlayer was used for previous games
	bool isPartOfCurrentlyAttackedShipInDirection(Coordinate ship1, Coordinate ship2, int vertical, int horizontal, int sideways) const; //checks whether two ships have a common coordinate in a given direction
	void updateShipCount(char cur);
	int calcNumOfShipsUpToGivenSize(int size) const;
	double calcMitigatedScoreForUnlikelyLocation(double curScore) const;
	double factorScoreForLikelyLocation(double curScore, int maxShipSize) const;
	Coordinate calc3DCubeCenter() const;
	double mapCellToValue(Coordinate point) const;
	double calcFuncMinDiff() const;
	void createCellToIndexMapping();


	int getLocationIndex(Coordinate move) const{
		return locationToIndexMap.at(move);
	}
	char getBoardPiece(Coordinate c) const {
		return gameBoard[c.depth][c.row][c.col];
	}
	double getBoardKeyScore(Coordinate loc) const {
		return reverseLegalAttackMoves.at(loc);
	}
	void setBoardKeyScore(Coordinate loc, double keyScore) {
		reverseLegalAttackMoves[loc] = keyScore;
	}
	//check if row is a valid row number in the board
	bool isValidRow(int row) const
	{ 
		return (row >= 0 && row < numOfRows); 
	}
	//check if col is a valid col number in the board
	bool isValidCol(int col) const
	{
		return (col >= 0 && col < numOfCols);
	}
	// check if depth is a valid col number in the board
	bool isValidDepth(int depth) const
	{
		return (depth >= 0 && depth < boardDepth);
	}
	//checks whether Coordinate pos is a legal location on the board
	bool isLegalPosition(Coordinate pos) const
	{
		return isValidRow(pos.row) && isValidCol(pos.col) && isValidDepth(pos.depth);
	}
	//checks whether Coordinate hit is a legal location on the board that was attacked by the enemy and help its own ship
	bool isEnemySelfHitArea(Coordinate hit) const {
		return isLegalPosition(hit) && getBoardPiece(hit) == ENEMY_SELF_HIT_AREA;
	}
	//checks whether Coordinate move is a legal location on the board that has a location that's relevent for attacking an enemy ship
	bool isAvailableMove(Coordinate move) const {
		return isLegalPosition(move) && (getBoardPiece(move) == UNDISCOVERED_AREA || getBoardPiece(move) == ENEMY_SELF_HIT_AREA);
	}

public:
	void setBoard(const BoardData& board) override;
	Coordinate attack() override;
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;
	void setPlayer(int player) override;
	Smart3DPlayer& operator=(Smart3DPlayer&) = delete;
	Smart3DPlayer(Smart3DPlayer& other) = delete;
	Smart3DPlayer() = default;
	~Smart3DPlayer() = default;
};