#ifndef AVH_AI_PLAYER_H
#define AVH_AI_PLAYER_H

#include "AvHAIConstants.h"

// These define the bot's view frustum sides
#define FRUSTUM_PLANE_TOP 0
#define FRUSTUM_PLANE_BOTTOM 1
#define FRUSTUM_PLANE_LEFT 2
#define FRUSTUM_PLANE_RIGHT 3
#define FRUSTUM_PLANE_NEAR 4
#define FRUSTUM_PLANE_FAR 5

static const float BOT_FOV = 100.0f;  // Bot's field of view;
static const float BOT_MAX_VIEW = 9999.0f; // Bot's maximum view distance;
static const float BOT_MIN_VIEW = 5.0f; // Bot's minimum view distance;
static const float BOT_ASPECT_RATIO = 1.77778f; // Bot's view aspect ratio, 1.333333 for 4:3, 1.777778 for 16:9, 1.6 for 16:10;

static const float f_fnheight = 2.0f * tan((BOT_FOV * 0.0174532925f) * 0.5f) * BOT_MIN_VIEW;
static const float f_fnwidth = f_fnheight * BOT_ASPECT_RATIO;

static const float f_ffheight = 2.0f * tan((BOT_FOV * 0.0174532925f) * 0.5f) * BOT_MAX_VIEW;
static const float f_ffwidth = f_ffheight * BOT_ASPECT_RATIO;

// Bot will jump. Will duck-jump if bDuckJump is set to true (true by default)
void BotJump(AvHAIPlayer* pBot, bool bDuckJump = true);
// Bot will suicide (equivalent of "kill" in the console)
void BotSuicide(AvHAIPlayer* pBot);
// Tells the bot to look at the target location. Overridden by BotMoveLookAt if set
void BotLookAt(AvHAIPlayer* pBot, Vector NewLocation);
// Tells the bot to look at the centre of the target entity. Is overridden by BotMoveLookAt if set
void BotLookAt(AvHAIPlayer* pBot, edict_t* target);
// Informs the bot that it must look at this spot to complete its current move (e.g. look at top of ladder to climb). Overrides BotLookAt
void BotMoveLookAt(AvHAIPlayer* pBot, const Vector target);
// Instantly snaps the bot's view to the target position
void BotDirectLookAt(AvHAIPlayer* pBot, Vector target);

// Bot will attempt to look at the object and use it. If bContinuous is set, the bot will hold the use key, otherwise they will spam every 0.5s as requested
bool BotUseObject(AvHAIPlayer* pBot, edict_t* Target, bool bContinuous);

// Make the bot type something in either global or team chat
void BotSay(AvHAIPlayer* pBot, bool bTeamSay, float Delay, char* textToSay);

// Helper function so the bot can find an empty message slot in its list of pending messages to print out
bot_msg* GetAvailableBotMsgSlot(AvHAIPlayer* pBot);

// Bot will attempt to drop the currently-held weapon
void BotDropWeapon(AvHAIPlayer* pBot);

void BotShootTarget(AvHAIPlayer* pBot, AIWeaponType AttackWeapon, edict_t* Target);
void BotShootLocation(AvHAIPlayer* pBot, AIWeaponType AttackWeapon, const Vector TargetLocation);

void BotUpdateDesiredViewRotation(AvHAIPlayer* pBot);
void BotUpdateViewRotation(AvHAIPlayer* pBot, float DeltaTime);
void BotUpdateView(AvHAIPlayer* pBot);
bool IsEntityInBotFOV(AvHAIPlayer* Observer, const edict_t* Entity);
bool IsEntityVisibleToBot(AvHAIPlayer* pBot, const edict_t* Entity);
void UpdateAIPlayerViewFrustum(AvHAIPlayer* pBot);

// Find a point on the target player that is visible from the view of the observer. Checks head, centre and feet for visibility
Vector GetVisiblePointOnPlayerFromObserver(edict_t* Observer, edict_t* TargetPlayer);

void UpdateBotChat(AvHAIPlayer* pBot);

void ClearBotInputs(AvHAIPlayer* pBot);
void StartNewBotFrame(AvHAIPlayer* pBot);
void EndBotFrame(AvHAIPlayer* pBot);

void OnBotTeleport(AvHAIPlayer* pBot);

// Sets up the bot's think routine
void RunAIPlayerFrame(AvHAIPlayer* pBot);
// The bot's actual think routine. All the logic to make the bot do something sits here
void AIPlayerThink(AvHAIPlayer* pBot);
// The bot's think routine if they need to do something to join a team and start playing (e.g. pick a class)
void AIPlayerStartGameThink(AvHAIPlayer* pBot);

// Called when the bot is killed by someone
void AIPlayerKilled(AvHAIPlayer* pBot, edict_t* Killer);

void BotSwitchToWeapon(AvHAIPlayer* pBot, AIWeaponType NewWeaponSlot);

bool ShouldBotThink(AvHAIPlayer* pBot);

void BotResumePlay(AvHAIPlayer* pBot);

void AIPlayerTakeDamage(AvHAIPlayer* pBot, int damageTaken, edict_t* aggressor);

// Simulates a fake client inputting a console command
void FakeClientCommand(edict_t* pBot, const char* arg1, const char* arg2, const char* arg3);

#endif