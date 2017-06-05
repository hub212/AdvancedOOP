#ifndef TYPES_H
#define TYPES_H
#include "IBattleshipGameAlgo.h"
#include <string>
#include <Windows.h>

#define NUM_ROWS 10
#define NUM_COLS 10
#define MY_MAX_PATH 1024


class Player 
{
public:
	std::string name = "";
	HINSTANCE hdll;
	GetAlgoType getAlgo;

	Player(std::string name, HINSTANCE hdll, GetAlgoType getAlgo);

};

enum class Players {
	PlayerA = 0,
	PlayerB = 1
};

enum class VesselType
{
	Boat, Missiles, Sub, War
};

enum class Scores
{
	BScore = 2,
	MScore = 3,
	SScore = 7,
	WScore = 8
};

enum class Length
{
	BLength = 1,
	MLength = 2,
	SLength = 3,
	WLength = 4
};


// this class type is being used for getting back the attack results
class Vessel_ID
{
public:
	VesselType type;
	Players player;
	int score;

	Vessel_ID(VesselType type, Players player);
	Vessel_ID();
};

typedef IBattleshipGameAlgo *(*GetAlgoType)();

#endif
