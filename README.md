# COL781 Assignment 2
A 3D game in which your task is to avoid being caught by frog(s). You can add multiple frogs with varying speeds in the simulation. Each frog tries to align itself along your location (First-Person camera) once it lands after completing a jump. If you are caught by a frog, screen turns red.

This is a Visual Studio 2015 project that uses OpenGL for graphical rendering. Read assignment report for certain implementational details.

### Running the code
The project is implemented in Visual Studio. Open the solution file for running the code.

1.) Press SPACE to begin the game. On pressing SPACE, mouse pointer disappears (and is anchored to screen centre). At the beginning, screen will be blank as you have to add frogs.

2.) Press L to add a frog. On pressing L, mouse pointer is released. You are free to close the execution or add a new frog. Type the speed of frog in terminal (60 is moderate. 30 is slow and 120 is fast). As soon as you enter the speed and press ENTER, game will begin and mouse will be disappeared again. 

3.) Controls are similar to an FPS camera (WASD for motion nad mouse for changing yaw and pitch).

4.) Press L to add new frogs along with current frogs.

5.) Currently, after each completion of jump, frogs re-align themselves to point towards your position. Difficulty can be reduced, if we choose to re-align or not based on probability using a random variable. Refer "void animate()" function where you assign "frog.torso" (angle of frog) to a value to point to camera. 
