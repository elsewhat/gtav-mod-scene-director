Scene director 2.1 release by elsewhat

Scene Director is a mod for GTA V specifically aimed at recording Machinima.
In many ways its an extension to Director Mode and Rockstar Editor. 

It has already been used by some of the most profiled GTA V Machinima videos.

Starting with version 2.0 you can record the actions of each actor in the scene.
You can even record an actor in active mode, which means the other actors will be playing there recordings at the same time.

Action types which are currently recorded are: 
- On foot movement
- In vehicle movement (cars, helis, boats, planes++)
- Entering vehicle (including as a passenger with ALT+F)
- Exit vehicle
- Play animation sequence (ALT+NUM1-9)
- Play scenario from other mod (such as scenario menu)
- Aim at entity (works ok, but could be better)
- Shoot at entity (work ok, but could be better)

Animation support is based around a five-digit unique code each animation has. 
There are 21882 animations available and you can view them at Youtube through http://bit.ly/GTAVAnims
(Youtube videos are not complete. Currently 00000-06999 are available)
List of animations is in file SceneDirectorAnim.txt which must be in the GTA V root directory.

Add one or more animations to an animations sequence, and these are automatically given a shortcut you can use whilst using the mod.
The shortcut is ALT+NUM1-9. Note that if you choose the controllable type for the animation, only the first one will be played (GTA bug/limitation).
To remove an animation sequence, press del in the menu.

Kudos to Guadmaz for this system and list of animation.

I recommend using the mod together with Enhanced Trainer (as it allows you to store player appearence and vehicles)

Download: https://www.gta5-mods.com/scripts/scene-director
Source available at: https://github.com/elsewhat/gtav-mod-scene-director
Animaton videos: http://bit.ly/GTAVAnims

Installation: Requires Scripthook V. Copy SceneDirector.asi, SceneDirectorAnim.txt and scene_director.ini into game directory

Changelog
Version 2.1
- Up to 20 actors supported (though menu might not fit)
- Actor now automatically updated if you swap ped model 
- Time offset / delay for playback of recording
- PAGE-UP+1-9 - Teleport into actor 1-9's vehicle
- Copy a recording to other actors (without existing recording)
 
Version 2.0.1
- Improved vehicle recording
- Animation preview improved

Version 2.0
- Rewritten most of the mod with focus on actor recording. 
- Animation support (beta)


Version 1.3
- World menu where you can control weather, wind, blackout and a timelapse
- Actor menu 
	- Choose between 28 walking styles
	- Set max health
	- Choose one of three relationship groups. Group A and B hates each other, whilst Group C is neutral.
	 (buggy and will only be applied when the scene is switched to active)
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

Version 1.0
Initial release

The controls are as follows:

F10 - Show/hide menu HUD
Can be configured in .ini file

NUMSPACE2,8,4,6,5 - Controlling menu HUD
Can be configured in .ini file

DEL
Delete element in HUD (animations and actors)

Change scene mode - From menu
Change scene mode. Scene mode is default active and waypoints will be acted on immediately. Setup scene mode, waypoint will be stored, and all of them triggered at the same time when scene mode is set to active

Assign current player to slot 1-9 - From menu (or CTRL+1-9)

Reset scene - ALT+DELETE
Teleport all actors to the location they have when the scene mode was first set to active.

Firing squad mode - ALT+C 
Copy mode aka. firing squad mode. Other actors will aim and fire at the same targets as the player. They will also enter vehicles and skydive when the player does so.

ALT + T Teleport the player to waypoint

ALT+PAUSE/Break
Reset scene director

ALT + F Enter nearest vehicle as a passenger