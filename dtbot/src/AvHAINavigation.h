//
// EvoBot - Neoptolemus' Natural Selection bot, based on Botman's HPB bot template
//
// bot_navigation.h
// 
// Handles all bot path finding and movement
//

#pragma once
#ifndef AVH_AI_NAVIGATION_H
#define AVH_AI_NAVIGATION_H

#include <extdll.h>

#include <string>

#include "DetourStatus.h"
#include "DetourNavMeshQuery.h"
#include "DetourTileCache.h"
#include "AvHAIPlayer.h"
#include "nav_constants.h"

constexpr auto MAX_PATH_POLY = 512; // Max nav mesh polys that can be traversed in a path. This should be sufficient for any sized map.

typedef struct _DYNAMIC_MAP_PROTOTYPE
{
	int EdictIndex = -1;
	edict_t* Edict = nullptr;
	std::string ObjectName = "";
	std::string Master = "";
	float Lip = 0.0f;
	float Height = 0.0f;
	float Wait = 0.0f; // How long the object will remain triggered before resetting (-1 means never)
	float Delay = 0.0f; // How long the object, once triggered, will wait before doing its thing
	float Health = 0.0f; // Does this object need to be damaged to activate?
	std::string GlobalState = "";
	int TriggerState = 0;
	int TriggerMode = 1;
	std::vector<std::string> Targets; // All targets this object triggers when activated
} DynamicMapPrototype;

typedef struct _DYNAMIC_OBJECT_STOPPOINT
{
	edict_t* CornerEdict = nullptr;
	Vector StopLocation = ZERO_VECTOR;
	bool bWaitForRetrigger = true;
	float WaitTime = 0.0f;
} DynamicMapObjectStop;

typedef struct _DYNAMIC_MAP_OBJECT
{
	int EdictIndex = -1;
	edict_t* Edict = nullptr;
	const char* ObjectName = nullptr;
	DynamicMapObjectType Type = MAPOBJECT_STATIC;
	std::vector<NavTempObstacle> TempObstacles; // Dynamic obstacle ref. Used to add/remove the obstacle as the door is opened/closed
	std::vector<edict_t*> Triggers; // Reference to the trigger edicts (e.g. func_trigger, func_button etc.)
	std::vector<NavOffMeshConnection*> AffectedConnections; // Which off-mesh connections are affected by this object (i.e. blocked/unblocked)
	std::vector<DynamicMapObjectStop> StopPoints; // Where the object stops when triggered. Doors will always have two stop points (open and shut positions), trains could have many
	int NextStopIndex = 0;
	std::vector<edict_t*> Targets;
	DynamicMapObjectState State = OBJECTSTATE_IDLE; // What is the object currently doing
	edict_t* Master = nullptr; // The entity which has locked this object and must be triggered first
	std::string GlobalState = ""; // The global state this object relies upon to be active
	float Wait = 0.0f; // Once finished its trigger action (e.g. opening), how long it waits before resetting
	float Delay = 0.0f; // After being triggered, how long this object will wait before doing its thing
	float LastActivatedTime = 0.0f;
	bool bToggleActive = false; // Can this be toggled active/inactive?
	bool bIsActive = true;
	int NumTimesActivated = 0; // How many times this object has been triggered
} DynamicMapObject;

// Door reference. Not used, but is a future feature to allow bots to track if a door is open or not, and how to open it etc.
typedef struct _NAV_HITRESULT
{
	float flFraction = 0.0f;
	bool bStartOffMesh = false;
	Vector TraceEndPoint = ZERO_VECTOR;
} nav_hitresult;

// Links together a tile cache, nav query and the nav mesh into one handy structure for all your querying needs
typedef struct _NAV_MESH
{
	class dtTileCache* tileCache = nullptr;
	class dtNavMeshQuery* navQuery = nullptr;
	class dtNavMesh* navMesh = nullptr;
	std::vector<NavOffMeshConnection> MeshConnections;
	std::vector<NavHint> MeshHints;
	std::vector<NavTempObstacle> TempObstacles;
} nav_mesh;


static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET', used to confirm the nav mesh we're loading is compatible;
static const int NAVMESHSET_VERSION = 1;

static const int TILECACHESET_MAGIC = 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'TSET', used to confirm the tile cache we're loading is compatible;
static const int TILECACHESET_VERSION = 4;

static const float pExtents[3] = { 400.0f, 50.0f, 400.0f }; // Default extents (in GoldSrc units) to find the nearest spot on the nav mesh
static const float pReachableExtents[3] = { max_ai_use_reach, max_ai_use_reach, max_ai_use_reach }; // Extents (in GoldSrc units) to determine if something is on the nav mesh

static const int DT_AREA_NULL = 0; // Represents a null area on the nav mesh. Not traversable and considered not on the nav mesh
static const int DT_AREA_BLOCKED = 3; // Area occupied by an obstruction (e.g. building). Not traversable, but considered to be on the nav mesh

static const int DOOR_USE_ONLY = 256; // Flag used by GoldSrc to determine if a door entity can only be used to open (i.e. can't be triggered)
static const int DOOR_START_OPEN = 1;

static const float CHECK_STUCK_INTERVAL = 0.1f; // How frequently should the bot check if it's stuck?

// Returns true if a valid nav mesh has been loaded into memory
bool NavmeshLoaded();
// Unloads all data, including loaded nav meshes, nav profiles, all the map data such as buildable structure maps and hive locations.
void UnloadNavigationData();
// Unloads only the nav meshes, but not map data such as doors, hives and locations
void UnloadNavMeshes();
// Searches for the corresponding .nav file for the input map name, and loads/initiatialises the nav meshes and nav profiles.
bool loadNavigationData(const char* mapname);
// Loads the nav mesh only. Map data such as hive locations, doors etc are not loaded
bool LoadNavMesh(const char* mapname);
// Unloads the nav meshes (UnloadNavMeshes()) and then reloads them (LoadNavMesh). Map data such as doors, hives, locations are not touched.
void ReloadNavMeshes();

AvHAINavMeshStatus NAV_GetNavMeshStatus();

void SetBaseNavProfile(AvHAIPlayer* pBot);
void UpdateBotMoveProfile(AvHAIPlayer* pBot, BotMoveStyle MoveStyle);


bool NAV_GenerateNewBasePath(AvHAIPlayer* pBot, const Vector NewDestination, const BotMoveStyle MoveStyle, const float MaxAcceptableDist);
bool NAV_MergeAndUpdatePath(AvHAIPlayer* pBot, std::vector<bot_path_node>& NewPath);

// Finds any random point on the navmesh that is relevant for the bot. Returns ZERO_VECTOR if none found
Vector UTIL_GetRandomPointOnNavmesh(const AvHAIPlayer* pBot);

/*	Finds any random point on the navmesh that is relevant for the bot within a given radius of the origin point,
	taking reachability into account(will not return impossible to reach location).

	Returns ZERO_VECTOR if none found
*/
Vector UTIL_GetRandomPointOnNavmeshInRadius(const NavAgentProfile& NavProfile, const Vector origin, const float MaxRadius);

/*	Finds any random point on the navmesh that is relevant for the bot within a given radius of the origin point,
	ignores reachability (could return a location that isn't actually reachable for the bot).

	Returns ZERO_VECTOR if none found
*/
Vector UTIL_GetRandomPointOnNavmeshInRadiusIgnoreReachability(const NavAgentProfile& NavProfile, const Vector origin, const float MaxRadius);

/*	Finds any random point on the navmesh of the area type (e.g. crouch area) that is relevant for the bot within a given radius of the origin point,
	taking reachability into account(will not return impossible to reach location).

	Returns ZERO_VECTOR if none found
*/
Vector UTIL_GetRandomPointOnNavmeshInRadiusOfAreaType(NavMovementFlag Flag, const Vector origin, const float MaxRadius);

/*	Finds any random point on the navmesh of the area type (e.g. crouch area) that is relevant for the bot within the min and max radius of the origin point,
	taking reachability into account(will not return impossible to reach location).

	Returns ZERO_VECTOR if none found
*/
Vector UTIL_GetRandomPointOnNavmeshInDonut(const NavAgentProfile& NavProfile, const Vector origin, const float MinRadius, const float MaxRadius);

/*	Finds any random point on the navmesh of the area type (e.g. crouch area) that is relevant for the bot within the min and max radius of the origin point,
	ignores reachability (could return a location that isn't actually reachable for the bot).

	Returns ZERO_VECTOR if none found
*/
Vector UTIL_GetRandomPointOnNavmeshInDonutIgnoreReachability(const NavAgentProfile& NavProfile, const Vector origin, const float MinRadius, const float MaxRadius);

// Roughly estimates the movement cost to move between FromLocation and ToLocation. Uses simple formula of distance between points x cost modifier for that movement
float UTIL_GetPathCostBetweenLocations(const NavAgentProfile &NavProfile, const Vector FromLocation, const Vector ToLocation);

// Returns true is the bot is grounded, on the nav mesh, and close enough to the Destination to be considered at that point
bool BotIsAtLocation(const AvHAIPlayer* pBot, const Vector Destination);

// Sets the bot's desired movement direction and performs jumps/crouch/etc. to traverse the current path point
void NewMove(AvHAIPlayer* pBot);
// Returns true if the bot has completed the current movement along their path
bool HasBotReachedPathPoint(const AvHAIPlayer* pBot);
bool HasBotCompletedLadderMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool HasBotCompletedWalkMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool HasBotCompletedFallMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool HasBotCompletedJumpMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool HasBotCompletedLiftMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool HasBotCompletedObstacleMove(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);

// Returns true if the bot is considered to have strayed off the path (e.g. missed a jump and fallen)
bool IsBotOffPath(const AvHAIPlayer* pBot);
bool IsBotOffLadderNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool IsBotOffWalkNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool IsBotOffFallNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool IsBotOffJumpNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool IsBotOffLiftNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);
bool IsBotOffObstacleNode(const AvHAIPlayer* pBot, Vector MoveStart, Vector MoveEnd, Vector NextMoveDestination, NavMovementFlag NextMoveFlag);

// Called by NewMove, determines the movement direction and inputs required to walk/crouch between start and end points
void GroundMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint);
// Called by NewMove, determines the movement direction and inputs required to jump between start and end points
void JumpMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint);
// Called by NewMove, determines movement direction and jump inputs to hop over obstructions (structures)
void BlockedMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint);
// Called by NewMove, determines the movement direction and inputs required to drop down from start to end points
void FallMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint);
// Called by NewMove, determines the movement direction and inputs required to climb a ladder to reach endpoint
void LadderMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint, float RequiredClimbHeight, unsigned char NextArea);

// Called by NewMove, determines the movement direction and inputs required to use a moving platform to reach an end point
void PlatformMove(AvHAIPlayer* pBot, const Vector StartPoint, const Vector EndPoint);

bool UTIL_TriggerHasBeenRecentlyActivated(edict_t* TriggerEntity);

// Will check for any func_breakable which might be in the way (e.g. window, vent) and make the bot aim and attack it to break it. Marines will switch to knife to break it.
void CheckAndHandleBreakableObstruction(AvHAIPlayer* pBot, const Vector MoveFrom, const Vector MoveTo, unsigned int MovementFlags);

void CheckAndHandleDoorObstruction(AvHAIPlayer* pBot);

DynamicMapObject* UTIL_GetNearestObjectTrigger(const Vector Location, DynamicMapObject* Object, edict_t* IgnoreTrigger, bool bCheckBlockedByDoor);
bool UTIL_IsPathBlockedByObject(const Vector StartLoc, const Vector EndLoc, DynamicMapObject* SearchObject);

DynamicMapObject* UTIL_GetObjectBlockingPathPoint(bot_path_node* PathNode, DynamicMapObject* SearchObject);
DynamicMapObject* UTIL_GetObjectBlockingPathPoint(const Vector FromLocation, const Vector ToLocation, const unsigned int MovementFlag, DynamicMapObject* SearchObject);
edict_t* UTIL_GetBreakableBlockingPathPoint(bot_path_node* PathNode, edict_t* SearchBreakable);
edict_t* UTIL_GetBreakableBlockingPathPoint(const Vector FromLocation, const Vector ToLocation, const unsigned int MovementFlag, edict_t* SearchBreakable);


Vector UTIL_GetButtonFloorLocation(const Vector UserLocation, edict_t* ButtonEdict);

// Clears all tracking of a bot's stuck status
void ClearBotStuck(AvHAIPlayer* pBot);
// Clears all bot movement data, including the current path, their stuck status. Effectively stops all movement the bot is performing.
void ClearBotMovement(AvHAIPlayer* pBot);

// Called every bot frame (default is 60fps). Ensures the tile cache is updated after obstacles are placed
bool UTIL_UpdateTileCache();

void AIDEBUG_DrawOffMeshConnections(unsigned int NavMeshIndex, float DrawTime = 0.0f);
void AIDEBUG_DrawTemporaryObstacles(unsigned int NavMeshIndex, float DrawTime = 0.0f);

Vector UTIL_GetNearestPointOnNavWall(AvHAIPlayer* pBot, const float MaxRadius);
Vector UTIL_GetNearestPointOnNavWall(const NavAgentProfile& NavProfile, const Vector Location, const float MaxRadius);

/*	Places a temporary obstacle of the given height and radius on the specified mesh. Will mark that part of the nav mesh as the given area.
	An example use case is to place an obstacle of area type SAMPLE_POLYAREA_OBSTRUCTION to mark temporary obstacles.
	Using DT_AREA_NULL will effectively cut a hole in the nav mesh, meaning it's no longer considered a valid mesh position.
*/
NavTempObstacle* NAV_AddTemporaryObstacleToNavmesh(unsigned int NavMeshIndex, Vector Position, float Radius, float Height, unsigned char Area);

/*	Removes the temporary obstacle from the mesh. The area will return to its default type (either walk or crouch).
	Removing a DT_AREA_NULL obstacle will "fill in" the hole again, making it traversable and considered a valid mesh position.
*/
bool NAV_RemoveTemporaryObstacleFromNavmesh(NavTempObstacle& ObstacleToRemove);


/* Adds a new off-mesh connection to the specified nav mesh at runtime. Bots using this nav mesh will immediately start using this connection if they're allowed to */
NavOffMeshConnection* NAV_AddOffMeshConnectionToNavmesh(unsigned int NavMeshIndex, Vector StartLoc, Vector EndLoc, unsigned char area, unsigned int flags, bool bBiDirectional);

NavHint* NAV_AddHintToNavmesh(unsigned int NavMeshIndex, Vector Location, unsigned int HintFlags);

/* Works the same as NAV_AddOffMeshConnectionToNavmesh but adds it to all nav meshes */
bool NAV_AddOffMeshConnectionToAllNavmeshes(Vector StartLoc, Vector EndLoc, unsigned char area, unsigned int flags, bool bBiDirectional);

/* Removes the off-mesh connection from all nav meshes which contain it */
bool NAV_RemoveOffMeshConnection(NavOffMeshConnection& RemoveConnectionDef);


/*
	Safely aborts the current movement the bot is performing. Returns true if the bot has successfully aborted, and is ready to calculate a new path.

	The purpose of this is to avoid sudden path changes while on a ladder or performing a wall climb, which can cause the bot to get confused.

	NewDestination is where the bot wants to go now, so it can figure out how best to abort the current move.
*/
bool AbortCurrentMove(AvHAIPlayer* pBot, const Vector NewDestination);


// Will clear current path and recalculate it for the supplied destination
bool BotRecalcPath(AvHAIPlayer* pBot, const Vector Destination);

/*	Main function for instructing a bot to move to the destination, and what type of movement to favour. Should be called every frame you want the bot to move.
	Will handle path calculation, following the path, detecting if stuck and trying to unstick itself.
	Will only recalculate paths if it decides it needs to, so is safe to call every frame.
*/
bool MoveTo(AvHAIPlayer* pBot, const Vector Destination, const BotMoveStyle MoveStyle, const float MaxAcceptableDist = max_ai_use_reach);

void UpdateBotStuck(AvHAIPlayer* pBot);

// Used by the MoveTo command, handles the bot's movement and inputs to follow a path it has calculated for itself
void BotFollowPath(AvHAIPlayer* pBot);
void BotFollowFlightPath(AvHAIPlayer* pBot, bool bAllowSkip);
void BotFollowSwimPath(AvHAIPlayer* pBot);

bool NAV_GenerateNewMoveTaskPath(AvHAIPlayer* pBot, const Vector NewDestination, const BotMoveStyle MoveStyle);

void SkipAheadInFlightPath(AvHAIPlayer* pBot);


// Walks directly towards the destination. No path finding, just raw movement input. Will detect obstacles and try to jump/duck under them.
void MoveToWithoutNav(AvHAIPlayer* pBot, const Vector Destination);

// Check if there are any players in our way and try to move around them. If we can't, then back up to let them through
void HandlePlayerAvoidance(AvHAIPlayer* pBot, const Vector MoveDestination);

Vector AdjustPointForPathfinding(const Vector Point);
Vector AdjustPointForPathfinding(const Vector Point, const NavAgentProfile& NavProfile);

// Special path finding that takes the presence of phase gates into account 
dtStatus FindFlightPathToPoint(const NavAgentProfile& NavProfile, Vector FromLocation, Vector ToLocation, std::vector<bot_path_node>& path, float MaxAcceptableDistance);

Vector UTIL_FindHighestSuccessfulTracePoint(const Vector TraceFrom, const Vector TargetPoint, const Vector NextPoint, const float IterationStep, const float MinIdealHeight, const float MaxHeight);

// Similar to FindPathToPoint, but you can specify a max acceptable distance for partial results. Will return a failure if it can't reach at least MaxAcceptableDistance away from the ToLocation
dtStatus FindPathClosestToPoint(AvHAIPlayer* pBot, const BotMoveStyle MoveStyle, const Vector ToLocation, std::vector<bot_path_node>& path, float MaxAcceptableDistance);
dtStatus FindPathClosestToPoint(const NavAgentProfile& NavProfile, const Vector FromLocation, const Vector ToLocation, std::vector<bot_path_node>& path, float MaxAcceptableDistance);

DynamicMapObject* UTIL_GetLiftReferenceByEdict(const edict_t* SearchEdict);
NavOffMeshConnection* UTIL_GetOffMeshConnectionForPlatform(const NavAgentProfile& NavProfile, DynamicMapObject* LiftRef);
Vector NAV_GetNearestPlatformDisembarkPoint(const NavAgentProfile& NavProfile, edict_t* Rider, DynamicMapObject* LiftReference);

dtStatus DEBUG_TestFindPath(const NavAgentProfile& NavProfile, const Vector FromLocation, const Vector ToLocation, std::vector<bot_path_node>& path, float MaxAcceptableDistance);

// If the bot is stuck and off the path or nav mesh, this will try to find a point it can directly move towards to get it back on track
Vector FindClosestPointBackOnPath(AvHAIPlayer* pBot, Vector Destination);

Vector FindClosestNavigablePointToDestination(const NavAgentProfile& NavProfile, const Vector FromLocation, const Vector ToLocation, float MaxAcceptableDistance);

// Will attempt to move directly towards MoveDestination while jumping/ducking as needed, and avoiding obstacles in the way
void PerformUnstuckMove(AvHAIPlayer* pBot, const Vector MoveDestination);

// Used by Detour for the FindRandomPointInCircle type functions
static float frand();

// Finds the appropriate nav mesh for the requested profile
const dtNavMesh* UTIL_GetNavMeshForProfile(const NavAgentProfile & NavProfile);
// Finds the appropriate nav mesh query for the requested profile
const dtNavMeshQuery* UTIL_GetNavMeshQueryForProfile(const NavAgentProfile& NavProfile);
// Finds the appropriatetile cache for the requested profile
const dtTileCache* UTIL_GetTileCacheForProfile(const NavAgentProfile& NavProfile);

float UTIL_PointIsDirectlyReachable_DEBUG(const Vector start, const Vector target);

/*
	Point is directly reachable:
	Determines if the bot can walk directly between the two points.
	Ignores map geometry, so will return true even if stairs are in the way, provided the bot can walk up/down them unobstructed
*/
bool UTIL_PointIsDirectlyReachable(const AvHAIPlayer* pBot, const Vector targetPoint);
bool UTIL_PointIsDirectlyReachable(const AvHAIPlayer* pBot, const Vector start, const Vector target);
bool UTIL_PointIsDirectlyReachable(const Vector start, const Vector target);
bool UTIL_PointIsDirectlyReachable(const NavAgentProfile& NavProfile, const Vector start, const Vector target);

// Will trace along the nav mesh from start to target and return true if the trace reaches within MaxAcceptableDistance
bool UTIL_TraceNav(const NavAgentProfile& NavProfile, const Vector start, const Vector target, const float MaxAcceptableDistance);

void UTIL_TraceNavLine(const NavAgentProfile& NavProfile, const Vector Start, const Vector End, nav_hitresult* HitResult);

/*
	Project point to navmesh:
	Takes the supplied location in the game world, and returns the nearest point on the nav mesh within the supplied extents.
	Uses pExtents by default if not supplying one.
	Returns ZERO_VECTOR if not projected successfully
*/
Vector UTIL_ProjectPointToNavmesh(const Vector Location, const Vector Extents = Vector(400.0f, 400.0f, 400.0f));
Vector UTIL_ProjectPointToNavmesh(const Vector Location, const NavAgentProfile& NavProfile);
Vector UTIL_ProjectPointToNavmesh(const Vector Location, const Vector Extents, const NavAgentProfile& NavProfile);
Vector UTIL_ProjectPointToNavmesh(const Vector Location, const int NavmeshIndex);
Vector UTIL_ProjectPointToNavmesh(const Vector Location, const Vector Extents, const int NavmeshIndex);

/*
	Point is on navmesh:
	Returns true if it was able to project the point to the navmesh (see UTIL_ProjectPointToNavmesh())
*/
bool UTIL_PointIsOnNavmesh(const Vector Location, const NavAgentProfile& NavProfile, const Vector SearchExtents = Vector(5.0f, 50.0f, 5.0f));
bool UTIL_PointIsOnNavmesh(const NavAgentProfile& NavProfile, const Vector Location, const Vector SearchExtents);

// Sets the BotNavInfo so the bot can track if it's on the ground, in the air, climbing a wall, on a ladder etc.
void UTIL_UpdateBotMovementStatus(AvHAIPlayer* pBot);

// Returns true if a path could be found between From and To location. Cheaper than full path finding, only a rough check to confirm it can be done.
bool UTIL_PointIsReachable(const NavAgentProfile& NavProfile, const Vector FromLocation, const Vector ToLocation, const float MaxAcceptableDistance);

// If the bot has a path, it will work out how far along the path it can see and return the furthest point. Used so that the bot looks ahead along the path rather than just at its next path point
Vector UTIL_GetFurthestVisiblePointOnPath(const AvHAIPlayer* pBot);
// For the given viewer location and path, will return the furthest point along the path the viewer could see
Vector UTIL_GetFurthestVisiblePointOnPath(const Vector ViewerLocation, std::vector<bot_path_node>& path, bool bPrecise);
Vector UTIL_GetFurthestVisiblePointOnLineWithHull(const Vector ViewerLocation, const Vector LineStart, const Vector LineEnd, int HullNumber);

// Returns the nearest nav mesh poly reference for the edict's current world position
dtPolyRef UTIL_GetNearestPolyRefForEntity(const edict_t* Edict);
dtPolyRef UTIL_GetNearestPolyRefForLocation(const Vector Location);
dtPolyRef UTIL_GetNearestPolyRefForLocation(const NavAgentProfile& NavProfile, const Vector Location);

// Returns the area for the nearest nav mesh poly to the given location. Returns BLOCKED if none found
unsigned char UTIL_GetNavAreaAtLocation(const Vector Location);
unsigned char UTIL_GetNavAreaAtLocation(const NavAgentProfile& NavProfile, const Vector Location);

// From the given start point, determine how high up the bot needs to climb to get to climb end. Will allow the bot to climb over railings
float UTIL_FindZHeightForWallClimb(const Vector ClimbStart, const Vector ClimbEnd, const int HullNum);


// Clears the bot's path and sets the path size to 0
void ClearBotPath(AvHAIPlayer* pBot);
// Clears just the bot's current stuck movement attempt (see PerformUnstuckMove())
void ClearBotStuckMovement(AvHAIPlayer* pBot);

float GetDesiredBotMovementSpeed(AvHAIPlayer* pBot);

void NAV_ClearDynamicMapData(bool bOnMapLoad);

// Based on the direction the bot wants to move and it's current facing angle, sets the forward and side move, and the directional buttons to make the bot actually move
void BotMovementInputs(AvHAIPlayer* pBot);

// Event called when a bot starts climbing a ladder
void OnBotStartLadder(AvHAIPlayer* pBot);
// Event called when a bot leaves a ladder
void OnBotEndLadder(AvHAIPlayer* pBot);

void NAV_PopulateTrainStopPoints(DynamicMapObject* Train);

void NAV_ModifyOffMeshConnectionFlag(NavOffMeshConnection* Connection, const unsigned int NewFlag);

void NAV_PopulateAllConnectionsAffectedByDynamicObjects();
void NAV_PopulateConnectionsAffectedByDynamicObject(DynamicMapObject* Object);

void NAV_ApplyTempObstaclesToObject(DynamicMapObject* Object, const int Area);

DynamicMapObject* UTIL_GetDynamicObjectByEdict(const edict_t* SearchEdict);
DynamicMapObject* UTIL_GetClosestPlatformToPoints(const Vector StartPoint, const Vector EndPoint);

Vector UTIL_AdjustPointAwayFromNavWall(const Vector Location, const float MaxDistanceFromWall);

const dtOffMeshConnection* DEBUG_FindNearestOffMeshConnectionToPoint(const Vector Point, unsigned int FilterFlags);

void NAV_SetTriggerMovementTask(AvHAIPlayer* pBot, DynamicMapObject* Trigger, DynamicMapObject* TriggerTarget);
void NAV_SetPickupMovementTask(AvHAIPlayer* pBot, edict_t* ThingToPickup, DynamicMapObject* TriggerToActivate);
void NAV_SetMoveMovementTask(AvHAIPlayer* pBot, Vector MoveLocation, DynamicMapObject* TriggerToActivate);
void NAV_SetTouchMovementTask(AvHAIPlayer* pBot, edict_t* EntityToTouch, DynamicMapObject* TriggerToActivate);
void NAV_SetUseMovementTask(AvHAIPlayer* pBot, edict_t* EntityToUse, DynamicMapObject* TriggerToActivate);
void NAV_SetBreakMovementTask(AvHAIPlayer* pBot, edict_t* EntityToBreak, DynamicMapObject* TriggerToActivate);

void NAV_ClearMovementTask(AvHAIPlayer* pBot);

void NAV_ProgressMovementTask(AvHAIPlayer* pBot);
bool NAV_IsMovementTaskStillValid(AvHAIPlayer* pBot);

bool UTIL_IsTileCacheUpToDate();

std::vector<NavHint*> NAV_GetHintsOfType(unsigned int NavMeshIndex, unsigned int HintType);
std::vector<NavHint*> NAV_GetHintsOfTypeInRadius(unsigned int NavMeshIndex, unsigned int HintType, Vector SearchLocation, float Radius);

void NAV_ClearCachedMapData();

void NAV_AddDynamicMapObject(DynamicMapPrototype* Prototype);
void NAV_PopulateDynamicMapObjects();
void NAV_UpdateDynamicMapObjects();
void NAV_LinkDynamicMapMasters();
void NAV_LinkDynamicMapObjectsToTriggers();
void NAV_LinkDynamicMapObjectsToOffmeshConnections();
void NAV_SetTrainStartPoints();
void NAV_SetPrototypeWait(int EntityIndex, char* Value);
void NAV_SetPrototypeDelay(int EntityIndex, char* Value);
void NAV_SetPrototypeLip(int EntityIndex, char* Value);
void NAV_SetPrototypeHeight(int EntityIndex, char* Value);
void NAV_SetPrototypeMaster(int EntityIndex, char* Value);
void NAV_SetPrototypeName(int EntityIndex, char* Value);
void NAV_SetPrototypeHealth(int EntityIndex, char* Value);
void NAV_SetPrototypeGlobalState(int EntityIndex, char* Value);
void NAV_SetPrototypeTriggerState(int EntityIndex, char* Value);
void NAV_SetPrototypeTriggerMode(int EntityIndex, char* Value);
void NAV_AddPrototypeTarget(int EntityIndex, char* Value);

void NAV_OnDynamicMapObjectBecomeIdle(DynamicMapObject* Object);
void NAV_OnDynamicMapObjectStopIdle(DynamicMapObject* Object);

void DEBUG_ShowDynamicObjects();

void NAV_OnTriggerActivated(DynamicMapObject* UsedObject);

DynamicMapObject* NAV_GetTriggerByEdict(edict_t* Edict);

DynamicMapObject* NAV_GetBestTriggerForObject(DynamicMapObject* ObjectToActivate, edict_t* PlayerToTrigger, const NavAgentProfile& NavProfile);
DynamicMapObject* NAV_GetTriggerReachableFromPlatform(float LiftHeight, DynamicMapObject* Platform, const Vector PlatformPosition = ZERO_VECTOR);

bool NAV_IsDynamicMapTriggerLinkedToObject(DynamicMapObject* TriggerObject, DynamicMapObject* TargetObject, std::vector<DynamicMapObject*> CheckedObjects);

void NAV_ForceActivateTrigger(AvHAIPlayer* pBot, DynamicMapObject* TriggerRef);

void DEBUG_PrintObjectInfo(DynamicMapObject* Object);

NavOffMeshConnection* NAV_GetNearestOffMeshConnectionToPoint(const NavAgentProfile& Profile, const Vector SearchPoint, NavMovementFlag SearchFlags);

void GetDesiredPlatformStartAndEnd(DynamicMapObject* PlatformRef, Vector EmbarkPoint, Vector DisembarkPoint, DynamicMapObjectStop& StartLocation, DynamicMapObjectStop& EndLocation);

bool NAV_PlatformNeedsActivating(AvHAIPlayer* pBot, DynamicMapObject* Platform, const Vector EmbarkPoint, const Vector DisembarkPoint);
bool NAV_CanBoardPlatform(AvHAIPlayer* pBot, DynamicMapObject* Platform, Vector BoardingPoint, Vector DesiredStop);

#endif // BOT_NAVIGATION_H

