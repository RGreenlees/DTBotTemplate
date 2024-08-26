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
	NAV_AREA_OBSTRUCTED = 3,		// Obstructed
	NAV_AREA_HAZARD = 4,		// Hazard
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
	case NAV_AREA_OBSTRUCTED:
		return NAV_FLAG_JUMP;
	case NAV_AREA_HAZARD:
		return NAV_FLAG_WALK;
	default:
		return NAV_FLAG_DISABLED;
	}
}

// Get appropriate debug colour for the area. Returns RGB as 3 unsigned chars encoded into a single unsigned int
inline void GetDebugColorForArea(NavArea Area, unsigned char& R, unsigned char& G, unsigned char& B)
{
	switch (Area)
	{
	case NAV_AREA_UNWALKABLE:
		R = 10;
		G = 10;
		B = 10;
		break;
	case NAV_AREA_WALK:
		R = 0;
		G = 192;
		B = 255;
		break;
	case NAV_AREA_CROUCH:
		R = 9;
		G = 130;
		B = 150;
		break;
	case NAV_AREA_OBSTRUCTED:
		R = 255;
		G = 64;
		B = 64;
		break;
	case NAV_AREA_HAZARD:
		R = 192;
		G = 32;
		B = 32;
		break;
	default:
		R = 255;
		G = 255;
		B = 255;
		break;
	}
}

// Get appropriate debug colour for the movement flag. Returns RGB as 3 unsigned chars encoded into a single unsigned int
inline void GetDebugColorForFlag(NavMovementFlag Flag, unsigned char& R, unsigned char& G, unsigned char& B)
{
	switch (Flag)
	{
	case NAV_FLAG_DISABLED:
		R = 8;
		G = 8;
		B = 8;
		break;
	case NAV_FLAG_WALK:
		R = 0;
		G = 192;
		B = 255;
		break;
	case NAV_FLAG_CROUCH:
		R = 9;
		G = 130;
		B = 150;
		break;
	case NAV_FLAG_JUMP:
		R = 200;
		G = 200;
		B = 0;
		break;
	case NAV_FLAG_LADDER:
		R = 64;
		G = 64;
		B = 255;
		break;
	case NAV_FLAG_FALL:
		R = 255;
		G = 64;
		B = 64;
		break;
	case NAV_FLAG_PLATFORM:
		R = 255;
		G = 32;
		B = 255;
		break;
	default:
		R = 255;
		G = 255;
		B = 255;
		break;
	}
}

// Return name of a flag for debugging purposes
inline void GetFlagName(NavMovementFlag Flag, char* outName)
{
	if (!outName) { return; }

	switch (Flag)
	{
	case NAV_FLAG_DISABLED:
		sprintf(outName, "Disabled");
		break;
	case NAV_FLAG_WALK:
		sprintf(outName, "Walk");
		break;
	case NAV_FLAG_CROUCH:
		sprintf(outName, "Crouch");
		break;
	case NAV_FLAG_JUMP:
		sprintf(outName, "Jump");
		break;
	case NAV_FLAG_LADDER:
		sprintf(outName, "Ladder");
		break;
	case NAV_FLAG_FALL:
		sprintf(outName, "Fall");
		break;
	case NAV_FLAG_PLATFORM:
		sprintf(outName, "Platform");
		break;
	default:
		sprintf(outName, "Undefined");
		break;
	}
}

// Return name of a flag for debugging purposes
inline void GetAreaName(NavArea Area, char* outName)
{
	if (!outName) { return; }

	switch (Area)
	{
	case NAV_AREA_UNWALKABLE:
		sprintf(outName, "Unwalkable");
		break;
	case NAV_AREA_WALK:
		sprintf(outName, "Walk");
		break;
	case NAV_AREA_CROUCH:
		sprintf(outName, "Crouch");
		break;
	case NAV_AREA_OBSTRUCTED:
		sprintf(outName, "Obstructed");
		break;
	case NAV_AREA_HAZARD:
		sprintf(outName, "Hazard");
		break;
	default:
		sprintf(outName, "Undefined");
		break;
	}
}

#endif // NAV_CONSTANTS_H