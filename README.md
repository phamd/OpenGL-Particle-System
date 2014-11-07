OpenGL Particle System
===============================
- A 3D particle system where a stream of particles emit from a source.
- This project is written with OpenGL and C++ for a class assignment.
- Full requirements listed in [3GC3 - A2_2014.pdf](<3GC3 - A2_2014.pdf>).

![Screenshot](screenshot.png?raw=true)

Required libraries
------------------
- freeglut3

Compiling
-------------------
- A makefile is included and is customized to work on either of Windows/Linux/Mac.
- Alternatively, a Visual Studio solution file is also present.
- Tested with:
    - Platform: Windows
        - Compiler: VS2013, MinGW(g++)
    - Platform: Linux (Debian)
        - Compiler: g++ 4.7.2

Controls
-------------------
- Instructions of operation are also printed to the console on startup.

|  Key        | Function                 |
| ------------|------------------------- |
|  Arrow Keys | Camera x,y-position      |
|  PgUp,PgDwn | Camera z-position        |
|  w,a,s,d    | Cannon angle left/right  |
|  e,c        | Cannon angle up/down     |
|  space      | Toggle particle flow     |
|  +,-        | +/- flow rate            |
|  f          | Toggle Friction          |
|  g          | Toggle gravity           |
|  i          | Toggle Wind              |
|  u,j        | +/- Wind Power           |
|  r          | Clear particles          |
|  y          | Particle Cam             |
|  b          | Burst 100 particles      |
|  z          | Fast mode (Cubes only)   |
