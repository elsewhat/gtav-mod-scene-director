# gtav-mod-scene-director
Scene director 1.0 release by elsewhat

Scene Director is a mod for GTA V specifically aimed at Director Mode and Rockstar Editor. It will allow you to set up a movie shot by yourself and should hopefully contribute to some new awesome movies. It can also be heaps of fun if you just want to play around.

It's intended to play together with other trainers. For example once you've clone or possessed a pedestrian, you could change the skin of the model using any trainer (I prefer Enhanced Trainer as it allows me to store skins and vehicles I want to use in a video). Another is example is that you might use a trainer which allows you to start animations or scenarios (like smoking etc). This animation will continue to play as you swap away from the model, but when you switch back the animation will be stopped so you can take control of the character.

Download:https://www.gta5-mods.com/scripts/scene-director
Source available at: https://github.com/elsewhat/gtav-mod-scene-director
Demo video: http://youtu.be/RATa0J01Vis


Installation: Requires Scripthook V. Copy SceneDirector.asi into game directory

The controls are as follows:

ALT+SPACE
Change scene mode. Scene mode is default active and waypoints will be acted on immediately. Setup scene mode, waypoint will be stored, and all of them triggered at the same time when scene mode is set to active

CTRL + 1-9 Assign current player to slot 1-9
ALT + 1-9 Swap to player in slot 1-9

Blips with number in the map will indicate the location of the pedestraian in slot 1-9.

If the player being swapped from is the driver of a vehicle and has a waypoint, the player will continue to drive towards the waypoint.

ALT+DELETE
Teleport all actors to the location they have when the scene mode was first set to active.

ALT+INSERT
Set the same waypoint for all actors

ALT+PAGE_UP
Start vehicle chase. The other actors will chase the player if he's in a vehicle.
(currently no way to stop it)

ALT+PAGE_DOWN
Start vehicle escort. The other actors will escort the player if he's in a vehicle.
(currently no way to stop it)

ALT+A
Autopilot for player to waypoint (current actor must be in a slot first)

F9 Possess nearest pedestrian
Free aim + F9 Possess targeted pedestrian

F10 Clone current player
If current player is in a vehicle, clones will become passengers

ALT+0 Possess previous pedestrian or clone

ALT + T Teleport the player to waypoint

CTRL + F Enter nearest vehicle as a passenger
If the player passenger has a waypoint, the vehicle will proceed to this waypoint. 
If the player passenger changes the waypoint while he's in the car, the vehicle will proceed to the new waypoint

Note: 
- that the vehicle currently won't stop at the waypoint, but will continue to circle the location 

ALT+PAUSE/Break
Reset scene director