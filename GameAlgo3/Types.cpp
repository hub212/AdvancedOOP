#include "Types.h"
#include <iostream>


////--------------------------
////		Vessels
////--------------------------

Vessel_ID::Vessel_ID(VesselType type, Players player) : type(type), player(player)
{
	switch (type)
	{
	case VesselType::Boat:
		score = (int)Scores::BScore;
		break;

	case VesselType::Missiles:
		score = (int)Scores::MScore;
		break;

	case VesselType::Sub:
		score = (int)Scores::SScore;
		break;

	case VesselType::War:
		score = (int)Scores::WScore;
		break;
	}
};

Vessel_ID::Vessel_ID() {};

/////---------------------------
/////		Player
/////---------------------------

Player::Player(std::string name, HINSTANCE hdll, GetAlgoType getAlgo) : playerNumber(playerNumber), name(name), hdll(hdll), getAlgo(getAlgo) {}
Player::Player(int playerNumber, Player player) {}


ostream& operator<<(std::ostream & Str, const Player& player)
{
	Str << player.name;
	return Str;
}


bool Player::operator== (const Player& player1) {
	return name == player1.name && hdll == player1.hdll;
}
