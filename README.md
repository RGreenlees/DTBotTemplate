
DTBot Template - A Bot Template for GoldSrc Games, Built on the Recast and Detour Libraries
===============

## Overview

DTBot is a metamod plugin designed to provide AI-controlled players for Half-Life and its mods, either to provide single-player experiences or to fill gaps in a team of humans online.

* It uses the Recast and Detour libraries for navigation (https://github.com/recastnavigation/recastnavigation)
* Bot can be configured to automatically add/remove bots to keep teams at a certain size, or keep them balanced

Nav meshes for the bot are generated externally by a specialised tool, and held in the navmeshes folder in the dtbot addons directory.


## Basic Terminology

* Recast - This is the framework that generates a nav mesh from a raw triangle input. It is not part of the evobot plugin as the nav meshes are not generated internally but instead by an external tool.
* Detour - This is the path-finding framework supplied by the recastnavigation that uses the nav mesh as an input. The sources for this are held in Detour and DetourTileCache.
* Nav Mesh - This is the actual set of triangles that form the navigation mesh, made of polys called "dtPoly" by the Detour library.
* Tile Cache - The nav mesh is broken up into tiles of 144x144 GoldSrc units. If the nav mesh needs to be modified at runtime (e.g. dynamic obstacle placed which blocks part of the mesh), then it only needs to rebuild the tiles affected rather than the entire nav mesh which would cause noticeable hitching.
* Nav Query - This is a set of filters that define a bot's movement capabilities (e.g. don't wall climb, prefer walking over crouched movement etc.). The path finding will generate paths based on these filters and costs.


## Bot Movement

Navigation data is loaded at the point of adding a bot to the server (see BotCreate in bot.cpp). If you don't add a bot, then the plugin won't do anything. Navigation data is automatically unloaded at new map load (see Spawn() function in dll.cpp).

* The MoveTo() function in bot_navigation.cpp is the entry point for all bot movement that uses path finding. It is designed to be called every bot frame. Not calling this will make the bot stop all movement, so there is no need to explicitly stop bot movement if you want them to stop moving somewhere.
* The MoveDirectlyTo() function in bot_navigation.cpp is an alternative which makes the bot move straight towards the destination. It will not use path finding, but it will try to avoid obstacles. Designed to be called every bot frame like MoveTo().

The MoveTo() function determines if a path needs to be recalculated, and will not do it if it's not needed, making the function safe to call every frame. Paths are calculated if one of the following conditions are met:

1. The bot doesn't already have a path
2. The desired destination has changed
3. The bot's movement profile has changed, either because the move style is different, or the bot has changed class
4. Path recalculation is also limited to a max of 3 times per second. This is unlikely, but could happen if the bot is chasing a moving target in some situations.

Once the bot has a path and is ready to start following it. MoveTo() will call BotFollowPath().

* BotFollowPath will check that the bot hasn't left the path (e.g. missed a jump and fallen somewhere), if it has then it just clears the path and returns so that the next MoveTo() call will generate a new path.
* If the bot is stuck (has not been able to get closer to its destination in the last second), it will perform an unstuck move (jumping/crouching/move left or right) to try and unstick itself.
* If all is well, it will call NewMove().
* NewMove() will check the type of movement it's performing (walk, jump, climb ladder) etc. and call the appropriate function to determine which direction the bot must move in, where it should angle its view if climbing, and any inputs like jumping/crouching. It will also check for breakable obstructions and handle smashing them if they're in the way.
* Once this is done, MoveTo() will then handle player avoidance by checking if any other players are in the way of the bot's desired movement direction, and get the bot to skirt around them if they are.
* Finally, MoveTo() calls BotMovementInputs(), which handles the actual input required to perform the move (forward speed, side speed and pressing the movement keys) based on the desiredMovementDir set by NewMove().

MoveDirectlyTo() also handles player avoidance and calls BotMovementInputs() in a self-contained way.


### Bot Sight

This is still a little messy, but the basic process flow is outlined below.

A bot can either have a movement look target, or a regular look target (MoveLookLocation and LookTargetLocation). MoveLookLocation always takes priority over LookTargetLocation, and should only be set if the bot has to look somewhere specific in order to perform a movement. For example, looking at the top of a ladder when climbing. This prevents the bot getting distracted by other points of interest and messing their movement up.

Once the bot has a look target defined (either movement or regular), the following process runs:

1. BotUpdateDesiredViewRotation() is called every bot frame. If the bot is not already in the process of performing a view movement (pBot->DesiredLookDirection == ZERO_VECTOR), then it will generate a new desired look direction. This consists of the required view angle to aim directly at the look target, with a random pitch and yaw offset applied based on the delta between the bot's current view angle and the new one. This simulates the fact that humans can't just spin their view and land exactly where they want to aim.
2. BotUpdateViewRotation() is called every GAME frame, and handles the interpolation of the bot's view towards the current DesiredLookDirection. Once the interpolation is complete, the DesiredLookDirection is zeroed out so that BotUpdateDesiredViewRotation() can then set a new DesiredLookDirection. This means the bot cannot correct its aim mid-turn: once committed to a view angle, it cannot change it until it's reached.
3. UpdateView() is called in BotUpdateDesiredViewRotation() and effectively updates the list of which enemies are visible, when they were last seen and so on.

Bot vision is handled with a view frustrum that accurately simulates a real player's vision. By default, bots play with a 16:9 field of view like a modern widescreen monitor. Traces are done to a target's feet, middle and top to determine if it is actually visible or hidden behind something else.


## Enemy Tracking

The UpdateView() function will update the tracking status of all enemy targets, which are held in the pBot->TrackedEnemies array. Bots will keep track of when and where an enemy was last seen, and what direction they were moving in.


## Special Thanks

* Botman (https://hpb-bot.bots-united.com/) - For providing the plugin base for bots that DTBot is based on
* TheFatal, Pierre-Marie Baty  (https://www.bots-united.com/) - For some useful code snippets
* The Bots United community (https://www.bots-united.com) - For all the work they did over the last 20-odd years improving bot integration with GoldSrc
* [APG]RoboCop[CL] (https://apg-clan.org/) - For helping get the Linux build compiled correctly
* Jeefo (https://yapb.jeefo.net/) - For being VERY patient and helping me get the Linux build working
* AlienBird (https://github.com/EterniumDev) for excellent feedback, suggestions and code contributions
* Anonymous Player (https://github.com/caxanga334) for writing the AMBuild scripts and helping get Linux compiled correctly
