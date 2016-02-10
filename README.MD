#Pigment

Pigment is a video game I developed as my graduation project. 

It is a multiplayer FPS game. There are two teams, blue and red. Objective is taking control of a flag, and guard it in your base for a duration get a point.

One 'originality' in Portal is being able to paint terrain. Each team can play terrain with their own color using their paint guns. You can only move flag on the color of your team. So you should build paths from flag to your base in order to move. Enemies should stop you by blocking your path and killing you.

Portal is a DirectX game. It uses stencil shadows for shadows and deferred lighting for lights. 

##Video
[![video](http://img.youtube.com/vi/Ks0NSFObpo4/0.jpg)](https://www.youtube.com/watch?v=Ks0NSFObpo4)

##Usage

When you run the client, it is in single player mode. You should also run a server for multiplayer.

To connect a server, press F12 to access console and type `LOCAL` to connect local host. If server is not local type `CONNECT XXX.XXX.XXX.XXX` to connect it. 

Controls

* WASD	Movement
* Mouse1	Shot

Test Keys

* F	fly on/off
* C	third person camera on/off
* J-K	Increase/Decrease life

Test commands (for console)

* LIGHTTEST	Light test mode
* LIGHTNUM	Number of Lights in Light test mode
* SHADOW		enable/disable shadows

##Installation

Portal requires DirectX 9. VS projects files are included in the git.
