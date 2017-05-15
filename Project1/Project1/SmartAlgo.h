#pragma once
#include "CommonAlgo.h"
#include <random>
#define NOT_TRIED -1
#define NONE -2

class SmartAlgo :
	public CommonAlgo
{

	bool seekAndDestroy = false;
	bool attackSucceeded = false;
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
	void calcAttack();
	bool tryToExpandAimRange(int direction);
	void determineAimDirection();
	void blockIrrelevantDirections();
	void calcCurrentCoords(int direction);

public:
	SmartAlgo();

	bool init(const std::string& path);
	void notifyOnAttackResult(int player, int row, int col, AttackResult result);
	std::pair<int, int> attack();
	~SmartAlgo();
};

