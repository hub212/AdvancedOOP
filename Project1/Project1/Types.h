#ifndef TYPES_H
#define TYPES_H

enum class Search
{
	Down,
	Right
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


class Vessel_ID
{
public:
	VesselType type;
	Players player;
	int score;

	Vessel_ID(VesselType type, Players player);
	Vessel_ID();
};

#endif
