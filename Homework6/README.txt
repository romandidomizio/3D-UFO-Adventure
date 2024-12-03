Homework 6: Textures

Author: Roman Di Domizio

Overview:
-----------
This program builds upon Homework 5 by introducing texture mapping alongside the existing lighting in a sci-fi city scene rendered with OpenGL. Textures have been applied to various objects to enhance realism, including a futuristic metallic texture on the buildings, sci-fi textures for the floor, and glowing emissive neon lights. The light source simulates a bright moonlight orbiting the city, while the user can toggle between automatic orbiting and manual control to adjust the scene’s illumination. The addition of textures and improved lighting effects provides a more immersive, dynamic environment.

Features:
----------
1. Three View Modes:
   - Press 'M' to cycle between:
     - Orthogonal overhead view.
     - Perspective view.
     - First-person view (use arrow keys to move around, and ‘P’ and ‘L’ to move up and down).

2. Light Control:
   - The light orbits the scene automatically, simulating the sun's motion.
   - The user can stop the orbit and move the light manually.
   - Press 'O' to toggle between automatic orbit mode and manual control.
   - Use 'W', 'A', 'S', 'D' to move the light manually when in manual mode.
   - Press ‘R’ to raise the light.
   - Press ‘F’ to lower the light.
   - Press ‘Q’ to increase the orbit radius.
   - Press ‘E’ to decrease the orbit radius.
   
3. Texturing:
   - Textures are applied to the buildings, floor, UFO, and light source
   - Each object uses texture mapping to achieve a futuristic look.
   - The UFO’s lights are emissive, appearing as glowing neon, adding to the sci-fi aesthetic.

4. First-Person Vertical Movement:
   - In first-person view, press ‘P’ to move the camera up and ‘L’ to move it down.

5. Neon Edges and Spikes:
   - The neon edges and spikes on the buildings are not textured. Adding textures to these elements caused the neon glow effect to become distorted and less effective. The neon effect relies on emissive lighting and color blending, which gets compromised by textures covering these surfaces. Therefore, keeping the neon elements without textures ensures a cleaner, glowing appearance that enhances the sci-fi aesthetic.


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
- First-Person Movement:
  - Use the arrow keys to move forward, backward, or rotate left and right.
  - Press ‘P’ to move up.
  - Press ‘L’ to move down.
- Exit: Press 'ESC' to exit the program.

Compilation:
-------------
To compile the program, run the following command:
make

This will generate an executable file named `hw6`.

Execution:
----------
To run the program, use the following command:
./hw6

Time Spent:
------------------------
16+ hours were spent on the assignment.
