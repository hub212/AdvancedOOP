#include "Types.h"

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

Player::Player(std::string name, HINSTANCE hdll, GetAlgoType getAlgo) : name(name), hdll(hdll), getAlgo(getAlgo) {};

bool Player::operator== (const Player& player1) {
	return name == player1.name && hdll == player1.hdll;
}