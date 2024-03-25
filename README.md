# TerraCraft
## Is a simple 2d block building sanbox game, with an infinite height and depth limit.
This project is only about two weeks old, and I only started learing C two weeks before that, so the code is probably bad.
This was made withought a game engine, but uses the Tigr graphics library along with with fastNoiseLite (licence in fastNoiseLite.h).
Currently support is for Windows only, but if you modify the saveAndLoad.c file you should be able to get it working on other operating systems.
I am compiling with GCC (MinGW) on Windows, with buildDev.bat to compile for debugging and buildRelease.bat for compiling with optimisations.

The game is currently configured to run at 1440p, so you need a monitor of that resolution. If you want to run the game in 1080p, first
go into chunk.c and over to the renderWorld function and change the value of the wierdX and wierdY variables to the values listed for 1080p.
I dont even know why this is nesscary, but the camera gets messed up if you dont do this.
Then go to main.c and where tigrWindow is called change the resolution from 2560, 1440 to 1920, 1080 and then it *should* work. I think.
Then compile as normal and thats it! (worst game settings "menu" ever)