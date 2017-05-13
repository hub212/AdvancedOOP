#pragma once
#include "CommonAlgo.h"
#include <random>

class SmartAlgo :
	public CommonAlgo
{


	bool seekAndDestroy = false;
	bool horizontalAxis = false;
	bool verticalAxis = false;
	bool axisFound = false;
	bool attackSucceeded = false;
	bool shipSinked = false;
	int firstHitRow = -1;
	int firstHitCol = -1;
	int aimDirection = -1;
	int randVar = -1;
	int *aimRange = nullptr;
	int** randomSpots = nullptr;
	std::vector<std::vector<int>> targetBank;
	int randomSpotIndex = 0;

	int randomGen(int min, int max);
	void initRandomTargets();
	void pickRandTarget();
	void calcStateAfterAttack();
	void tryToExpandAimRange(int direction);
	void setNextAttack();
	void determineAimDirection();
	void helpDetermineAimDirection(int direction1, int direction2);
	void blockIrrelevantDirections();

public:
	SmartAlgo();
	bool init(const std::string& path);
	void notifyOnAttackResult(int player, int row, int col, AttackResult result);
	~SmartAlgo();
};

