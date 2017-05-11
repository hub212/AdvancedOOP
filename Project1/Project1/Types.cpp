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