#pragma once

#ifndef NAV_CONSTANTS_H
#define NAV_CONSTANTS_H

// Possible movement types. Defines the actions the bot needs to take to traverse this node
enum NavMovementFlag
{
	NAV_FLAG_DISABLED = 2147483647,		// Disabled
	NAV_FLAG_WALK = 1,		// Walk
	NAV_FLAG_CROUCH = 2,		// Crouch
	NAV_FLAG_JUMP = 4,		// Jump
	NAV_FLAG_LADDER = 8,		// Ladder
	NAV_FLAG_FALL = 16,		// Fall
	NAV_FLAG_PLATFORM = 32,		// Platform
	NAV_FLAG_ALL = -1		// All flags
};

// Area types. Defines the cost of movement through an area and which flag to use
enum NavArea
{
	NAV_AREA_UNWALKABLE = 60,		// Unwalkable
	NAV_AREA_WALK = 1,		// Walk
	NAV_AREA_CROUCH = 2,		// Crouch
	NAV_AREA_HAZARD = 3,		// Hazard
};

// Retrieve appropriate flag for area (See process() in the MeshProcess struct)
inline NavMovementFlag GetFlagForArea(NavArea Area)
{
	switch (Area)
	{
	case NAV_AREA_UNWALKABLE:
		return NAV_FLAG_DISABLED;
	case NAV_AREA_WALK:
		return NAV_FLAG_WALK;
	case NAV_AREA_CROUCH:
		return NAV_FLAG_CROUCH;
	case NAV_AREA_HAZARD:
		return NAV_FLAG_WALK;
	default:
		return NAV_FLAG_DISABLED;
	}
}

#endif // NAV_CONSTANTS_H