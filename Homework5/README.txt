Homework 5: Lighting

Author: Roman Di Domizio

Overview:
-----------
This program implements a sci-fi city scene with lighting effects in OpenGL. The user can switch between three different view modes, control the light’s position, and toggle between automatic light orbiting and manual light movement. The light simulates the behavior of the sun orbiting around the city, and the user can stop the orbit and move the light manually. Proper lighting calculations are used for all objects, ensuring correct shading and reflections based on the light’s position.

Features:
----------
1. Three View Modes:
   - Press 'M' to cycle between:
     - Orthogonal overhead view.
     - Perspective view.
     - First-person view (use arrow keys to move around)

2. Light Control:
   - The light orbits the scene automatically, simulating the sun's motion.
   - The user can stop the orbit and move the light manually.
   - Press 'O' to toggle between automatic orbit mode and manual control.
   - Use 'W', 'A', 'S', 'D' to move the light manually when in manual mode.
   - Press ‘R’ to raise the light.
   - Press ‘F’ to lower the light.
   - Press ‘Q’ to increase the orbit radius.
   - Press ‘E’ to decrease the orbit radius.

Controls:
----------
- View Modes: Press 'M' to toggle between view modes (orthogonal, perspective, first-person).
- Light Control:
  - Press 'O' to toggle between automatic orbit mode and manual light control.
  - Press ‘R’ to raise the light.
  - Press ‘F’ to lower the light.
  - Press ‘Q’ to increase the orbit radius.
  - Press ‘E’ to decrease the orbit radius.
  - In manual mode:
    - Press 'W' to move the light forward.
    - Press 'S' to move the light backward.
    - Press 'A' to move the light left.
    - Press 'D' to move the light right.
- Exit: Press 'ESC' to exit the program.

Compilation:
-------------
To compile the program, run the following command:
make

This will generate an executable file named `hw5`.

Execution:
----------
To run the program, use the following command:
./hw5

Time Spent:
------------------------
12+ hours were spent on the assignment.
