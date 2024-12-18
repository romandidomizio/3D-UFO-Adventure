3D UFO Adventure

Overview
========
UFO Adventure is an interactive 3D simulation showcasing a UFO navigating a vibrant, futuristic cityscape. This project demonstrates advanced OpenGL techniques, including dynamic lighting, texture mapping, complex object modeling, and real-time interactivity. The scene features multiple complex glowing neon structures, a smoothly moving UFO, a tunnel entrance, a curved tunnel system, a dynamic Ferris wheel, a massive clock tower, and a dynamically orbiting moon. The project highlights not only technical competence in OpenGL with C, but also creative design elements that blend seamlessly into a sci-fi aesthetic.

Features
========
1. Dynamic UFO Controls:
   - Navigate the UFO with precise controls for movement, strafing, ascending, and rotation.
   - A third-person camera dynamically follows the UFO to keep the action in view.

2. Advanced Lighting and Emission Effects:
   - Emissive neon lighting and glow effects across diverse structures, beams, and fog fields.
   - Dynamic fog effects to simulate futuristic energy fields.
   - Realistic moonlight with ambient, diffuse, and specular lighting properties.
   - Shader applied to the UFO for a reflective, metallic effect.

3. Diverse and Complex Structures:
   - Central Energy Hub Tower: Grooved and stacked tier tower with blue energy beam shooting out.
   - Neon Helical Spirals: Glowing spirals that portray energy motion for hover power.
   - Hovering Neo-Gothic Castle: Detailed tessellated walls and spires with glowing accents.
   - Neon Glowing Ferris Wheel: Rotating wheels with glowing spokes and a glowing fog axis.
   - Curved Neon Tunnel: A glowing tunnel that curves around the city.
   - Clock Tower: Includes detailed clock faces and glowing beams.
   - Building Layout Grid: Metallic, glowing, and textured elements create a cyberpunk atmosphere.
   - Laser Beams: Connecting elements with neon energy beams.
   - Alien: Small, pink alien inside the transparent UFO dome.

4. Realistic Textures and Transparency:
   - Metallic and reflective materials.
   - Transparent elements such as fog fields and glowing edges.

5. Dynamic HUD:
   - A styled HUD displays a neon-styled title and interactive controls for navigating the UFO.
   - Separate HUD sections for movement and rotation controls.

Controls
========
UFO Controls:
- W: Move forward
- S: Move backward
- A: Strafe left
- D: Strafe right
- Up Arrow: Ascend
- Down Arrow: Descend
- Left Arrow: Rotate left
- Right Arrow: Rotate right
- ESC: Exit the game

Compilation Instructions
========================
To ensure the program compiles and runs cleanly across systems, use the provided Makefile.

Requirements:
- Linux: Requires OpenGL (`libGL`), GLU (`libGLU`), and GLUT (`freeglut`) libraries.
  - Install these with:
    sudo apt-get install freeglut3 freeglut3-dev

- macOS: Requires Xcode with OpenGL and GLUT installed. Compilation may require modifying the `Makefile` to use `-framework` flags instead of `-lGL -lGLU -lglut`.

- Windows: Use an IDE like Code::Blocks or Visual Studio. Ensure you have GLUT, GLU, and OpenGL libraries correctly installed and linked.

Building:
1. Navigate to the project directory in the terminal.
2. Run:
    make
   This will produce the executable `ufo`.

Cleaning:
- To remove compiled files, use:
    make clean

Packaging:
- To package the project into a ZIP file, use:
    make zip

Running the Simulation
======================
1. After building, execute the program:
    ./ufo
2. The game will launch in full-screen mode.

3. Starting the Game:
   - The HUD will display "3D UFO ADVENTURE" with basic instructions: "Press W to move forward."
   - Once you press W, the HUD will switch to full UFO controls.

4. Exiting the Game:
   - Press ESC at any time to quit.

Key Highlights
==============
1. Advanced Lighting Effects:
   - Majority of the scene features neon glowing effects using emissive lighting.
   - Structures and objects blend metallic and glowing neon aesthetics.
   - Dynamic fog fields simulate energy effects.
   - Shader effects applied to the UFO for a reflective and metallic appearance.

2. Complex Structures:
   - Central Energy Hub Tower: Grooved and stacked tier tower with blue energy beam shooting out.
   - Neon Helical Spirals: Glowing spirals that portray energy motion for hover power.
   - Hovering Neo-Gothic Castle: Detailed tessellated walls and spires with glowing accents.
   - Neon Glowing Ferris Wheel: Rotating wheels with glowing spokes and a glowing fog axis.
   - Curved Neon Tunnel: A glowing tunnel that curves around the city.
   - Clock Tower: Includes detailed clock faces and glowing beams.
   - Building Layout Grid: Metallic, glowing, and textured elements create a cyberpunk atmosphere.
   - Laser Beams: Connecting elements with neon energy beams.
   - Alien: Small, pink alien inside the transparent UFO dome.

3. Interactive HUD:
   - Styled neon HUD with dynamically switching content.
   - Separate left and right HUD sections for controls, designed for readability.

4. Smooth Integration:
   - All components blend seamlessly into a neon glowing cyberpunk city, providing an immersive visual and interactive experience.

Future Work
============================
1. Add more interactivity to the UFO, such as collision detection or gameplay mechanics.
2. Expand on the cityscape with additional structures or moving elements.
3. Optimize the scene for better performance on lower-end systems.
