Scene director 3.4.1 release by elsewhat

Scene Director is a mod for GTA V specifically aimed at recording Machinima.
In many ways its an extension to Director Mode and Rockstar Editor. 

It is active in use among most well known GTA V Machinima videos.

Stage lights is the major new functionality of version 3.4. Now you can light your scene as in a real movie.
In version 3.4.1 you can add complex move, rotate and flicker effects to stage lights.

Action types which are currently recorded are: 
- On foot movement
- In vehicle movement (cars, helis, boats, planes++)
- Entering vehicle (including as a passenger with ALT+F)
- Exit vehicle
- Play synchronized animation (ALT+NUM1-9)
- Play animation sequence (ALT+NUM1-9)
- Play scenario from other mod (such as scenario menu)
- Gun fire recording!
- Cover
- Jumping
- Climb
- Rocket vehicle (voltic2)


Follow Scene Director development on Discord: https://discord.gg/eDVwSN8 
Here you can: 
- Get the latest beta version and follow the development 
- Influence new features 
- View some awesome videos created with Scene Director 
- Share tip and tricks for Scene Director

I have also started doing live coding streams on Youtube live. Subscribe to be notified when I have the next one at 
https://www.youtube.com/c/dparnas9scenedirector

Download: https://www.gta5-mods.com/scripts/scene-director
Source available at: https://github.com/elsewhat/gtav-mod-scene-director
Animaton videos: http://bit.ly/GTAVAnims
Community support/dev channel at Discord: https://discord.gg/pEhNdpT

Installation
Prerequisites:
	- Scripthook V. 
Copy the following files into game directory
	- SceneDirector.asi
	- SceneDirectorAnim.txt
	- scene_director.ini 
	- SceneDirectorStageLights_config.xml
	- SceneDirectorSynchedAnim.xml
Add Stage lights as a DLC
	- Follow the instructions in ScenedirectorInstallDLC.txt to add it through OpenIV.

Kudso to Rockstar for standing up to Take-Two and saving the modding community
Kudos to Guadmaz for the list of animation
Kudos to CamxxCore for his assistance on controlling the camera movement used in Edit scene
Kudos to MuzTube for synchronized animation analysis


Changelog
Version 3.4.1 - See TBD
- Movement, rotation and flicker effects to Stage lights
- Edit effects in SceneDirectorStageLights_save.xml
- Tons of new Synchronized animations 
(distributed in SceneDirectorSynchedAnim.xml file)

Version 3.4 - See https://youtu.be/WI1Lj9NYmAY
- Stage lights (static and follow actor)

Version 3.3 - See https://youtu.be/9Nkff_FbD6k
- Synchronized animations
- Search animations
- Edit scene improvement (including disabling individual recording items)

Version 3.2 - See https://youtu.be/vXTkbvsorCc
- Edit Scene mode!

Version 3.1
- Record speaking animation
- Fixed resurrection of dead actors on scene reset
- Record reloading (default turned off)
- Biker update animations

Version 3.0 - See https://www.youtube.com/watch?v=avJHegqnTKI
- Gun fire recording!

- More animations (97100)
- Tweaks for vehicle recording at high speed (works for straights, but must be more tweaked for turns)
- Disable relationship groups from rel 1.3 as it's not really working

Version 2.2.2
- Adding almost 6000 new animations (huge thanks to Nucky and his brother Dennis)

Version 2.2.1 - See https://www.youtube.com/watch?v=N4-Ij3oGqrg
- END Explode nearby vehicles 
- ALT+END Out of control nearby vehicles
- Improved resetting of dead actors
- Replay correct heading after movement
- Various bugfixes

Version 2.2
- Save / Load actors
- ALT character wheel only disabled when menu is shown

Version 2.1 - See https://www.youtube.com/watch?v=6ciwi4nvBL4
- Up to 20 actors supported (though menu might not fit)
- Actor now automatically updated if you swap ped model 
- Time offset / delay for playback of recording
- PAGE-UP+1-9 - Teleport into actor 1-9's vehicle
- Copy a recording to other actors (without existing recording)
 
Version 2.0.1 - See https://www.youtube.com/watch?v=7ItsYHQW5CM
- Improved vehicle recording
- Animation preview improved

Version 2.0 - See https://www.youtube.com/watch?v=4qwxPX53iuQ
- Rewritten most of the mod with focus on actor recording. 
- Animation support (beta)


Version 1.3 - See https://www.youtube.com/watch?v=c7X_kkoAgM8
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