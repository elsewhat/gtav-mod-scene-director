Scene director 1.3 release by elsewhat

Scene Director is a mod for GTA V specifically aimed at Director Mode and Rockstar Editor. It will allow you to set up a movie shot by yourself and should hopefully contribute to some new awesome movies. It can also be heaps of fun if you just want to play around.

It's intended to play together with other trainers. For example once you've clone or possessed a pedestrian, you could change the skin of the model using any trainer (I prefer Enhanced Trainer as it allows me to store skins and vehicles I want to use in a video). Another is example is that you might use a trainer which allows you to start animations or scenarios (like smoking etc). This animation will continue to play as you swap away from the model, but when you switch back the animation will be stopped so you can take control of the character.

Download: https://www.gta5-mods.com/scripts/scene-director
Source available at: https://github.com/elsewhat/gtav-mod-scene-director

Installation: Requires Scripthook V. Copy SceneDirector.asi and scene_director.ini into game directory

Changelog 
Version 1.3
- World menu where you can control weather, wind, blackout and a timelapse
- Actor menu 
	- Choose between 28 walking styles
	- Set max health
	- Choose one of three relationship groups. Group A and B hates each other, whilst Group C is neutral.
	 ( will only be applied when the scene is switched to active)
	- If in vehicle there is now a setting for avoiding damage (some cosemtic damage still occurs unfortunately)
- Set a spotlight following each actors. Different spot locations and colors are available
- Firing squad now copies all 217 scenarios (use together with Scenario Menu 2.0)

Version 1.2 
Context sensitive menu system and HUD. No need to remember key combinations any more

Version 1.1
- Copy mode aka firing squad activated with ALT+C
- Configuration file
- Config options for rebinding the default F9 and F10 for possess and clone
- Config options for settings the height above ground for plane and heli waypoints

Version 1.02
- Cloned peds will no longer attack other actors
- Move to waypoint and autopilot now works for planes (now using AI::TASK_PLANE_MISSION)
(should be a great way of doing an overhead shot of a scene)

Version 1.0.1 
Fixed bugs with escort,chase and heading of entities after scene reset


The controls are as follows:

F10 - Show/hide menu HUD
Can be configured in .ini file

NUMSPACE2,8,4,6,5 - Controlling menu HUD
Can be configured in .ini file


Change scene mode - From menu
Change scene mode. Scene mode is default active and waypoints will be acted on immediately. Setup scene mode, waypoint will be stored, and all of them triggered at the same time when scene mode is set to active

Assign current player to slot 1-9 - From menu (or CTRL+1-9)

Blips with number in the map will indicate the location of the pedestraian in slot 1-9.
If the player being swapped from is the driver of a vehicle and has a waypoint, the player will continue to drive towards the waypoint.

Reset scene - ALT+DELETE
Teleport all actors to the location they have when the scene mode was first set to active.

Firing squad mode - ALT+C 
Copy mode aka. firing squad mode. Other actors will aim and fire at the same targets as the player. They will also enter vehicles and skydive when the player does so.

ALT + T Teleport the player to waypoint

ALT+PAUSE/Break
Reset scene director

CTRL + F Enter nearest vehicle as a passenger
If the player passenger has a waypoint, the vehicle will proceed to this waypoint. 
If the player passenger changes the waypoint while he's in the car, the vehicle will proceed to the new waypoint

ALT+0 Possess previous pedestrian or clone