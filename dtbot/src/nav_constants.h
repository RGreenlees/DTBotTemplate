#pragma once

#ifndef NAV_CONSTANTS_H
#define NAV_CONSTANTS_H

#define NAV_MESH_DEFAULT 0

#define NUM_NAV_MESHES 1

#include <vector>

// Possible movement types. Defines the actions the bot needs to take to traverse this node
enum NavMovementFlag
{
	NAV_FLAG_DISABLED = 1 << 31,		// Disabled
	NAV_FLAG_WALK = 1 << 0,		// Walk
	NAV_FLAG_CROUCH = 1 << 1,		// Crouch
	NAV_FLAG_JUMP = 1 << 2,		// Jump
	NAV_FLAG_LADDER = 1 << 3,		// Ladder
	NAV_FLAG_FALL = 1 << 4,		// Fall
	NAV_FLAG_PLATFORM = 1 << 5,		// Platform
	NAV_FLAG_ALL = -1		// All flags
};

// Area types. Defines the cost of movement through an area and which flag to use
enum NavArea
{
	NAV_AREA_NULL = 0,		// Null area, cuts a hole in the mesh
	NAV_AREA_UNWALKABLE = 60,		// Unwalkable
	NAV_AREA_WALK = 1,		// Walk
	NAV_AREA_CROUCH = 2,		// Crouch
	NAV_AREA_OBSTRUCTED = 3,		// Obstructed
	NAV_AREA_HAZARD = 4,		// Hazard
};

// Profile indices. Use these when retrieving base agent profile information
enum NavProfileIndex
{
	NAV_PROFILE_PLAYER = 0,		// Base Player
	NAV_PROFILE_DEFAULT = 1,		// Default profile which has all capabilities except disabled flags, and 1.0 area costs for everything 
};

// Profile indices. Use these when retrieving base agent profile information
enum NavMeshIndex
{
	NAV_MESH_PLAYER = 0,		// Nav Mesh
};

// Agent profile definition. Holds all information an agent needs when querying the nav mesh
typedef struct _NAV_AGENT_PROFILE
{
	unsigned int NavMeshIndex = 0;
	class dtQueryFilter Filters;
	bool bFlyingProfile = false;
} NavAgentProfile;

// Declared in DTNavigation.cpp
// List of base agent profiles
extern std::vector<NavAgentProfile> BaseAgentProfiles;

// Retrieve appropriate flag for area (See process() in the MeshProcess struct)
inline NavMovementFlag GetFlagForArea(NavArea Area)
{
	switch (Area)
	{
	case NAV_AREA_UNWALKABLE:
		return NAV_FLAG_DISABLED;
	case NAV_AREA_WALK:
		return NAV_FLAG_CROUCH;
	case NAV_AREA_CROUCH:
		return NAV_FLAG_JUMP;
	case NAV_AREA_OBSTRUCTED:
		return NAV_FLAG_LADDER;
	case NAV_AREA_HAZARD:
		return NAV_FLAG_CROUCH;
	default:
		return NAV_FLAG_DISABLED;
	}
}

// Get appropriate debug colour for the area. Returns RGB as 3 unsigned chars encoded into a single unsigned int
inline void GetDebugColorForArea(NavArea Area, unsigned char& R, unsigned char& G, unsigned char& B)
{
	switch (Area)
	{
	case NAV_AREA_NULL:
		R = 128;
		G = 128;
		B = 128;
		break;
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

// Populate the base nav profiles. Should be called once after loading the navigation data
inline void PopulateBaseAgentProfiles()
{
	BaseAgentProfiles.clear();

	NavAgentProfile NewProfile;
	NewProfile.NavMeshIndex = 0;
	NewProfile.Filters.setIncludeFlags(63);
	NewProfile.Filters.setExcludeFlags(NAV_FLAG_DISABLED);
	NewProfile.Filters.setAreaCost(0, 0.0);
	NewProfile.Filters.setAreaCost(1, 1.0);
	NewProfile.Filters.setAreaCost(2, 2.0);
	NewProfile.Filters.setAreaCost(3, 4.0);
	NewProfile.Filters.setAreaCost(4, 10.0);
	BaseAgentProfiles.push_back(NewProfile);

	NavAgentProfile DefaultProfile;
	DefaultProfile.NavMeshIndex = 0;
	DefaultProfile.Filters.setIncludeFlags(0x7fffffff);
	DefaultProfile.Filters.setExcludeFlags(NAV_FLAG_DISABLED);
	DefaultProfile.Filters.setAreaCost(0, 1.0);
	DefaultProfile.Filters.setAreaCost(1, 1.0);
	DefaultProfile.Filters.setAreaCost(2, 1.0);
	DefaultProfile.Filters.setAreaCost(3, 1.0);
	DefaultProfile.Filters.setAreaCost(4, 1.0);
	BaseAgentProfiles.push_back(DefaultProfile);

}

// Return the appropriate base nav profile information
inline const NavAgentProfile GetBaseAgentProfile(const NavProfileIndex Index)
{
	return BaseAgentProfiles[Index];
}

#endif // NAV_CONSTANTS_H