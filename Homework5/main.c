//
//  main.c
//  Homework5
//
//  Created by Roman Di Domizio on 10/9/24.
//

#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Camera control variables
int viewMode = 0;  // Global variable to track the current view mode
float cameraDistance = 13.0f;  // Distance from the center of the scene
float cameraX = 0.0f;     // Camera X position
float cameraZ = 10.0f;    // Camera Z position
float yaw = 0.0f;         // Yaw angle for rotating left and right (in degrees)
float moveSpeed = 0.5f;   // Speed of movement for forward/backward
float rotationSpeed = 2.0f;  // Speed of yaw rotation

// Lighting properties
GLfloat lightPos[] = {0.0f, 10.0f, 10.0f, 1.0f};   // Position of the light
GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Ambient light (low level of overall light)
GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f}; // Diffuse light (brightness of light)
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Specular light (shiny reflections)
int isOrbiting = 1;  // 1 if the light is orbiting, 0 if in manual mode
float orbitAngle = 0.0f;  // Angle of the light's orbit (in degrees)
float orbitSpeed = 1.0f;  // Speed of the orbit (degrees per frame)
float manualLightX = 0.0f, manualLightZ = 10.0f;  // Manual light position (XZ plane)
float orbitRadius = 15.0f;  // Radius of the orbit (controls distance of light from the center)

// Function prototypes
void renderBitmapString(float x, float y, void *font, const char *string);
void drawAbstractBuilding(void);
void positionAndScaleBuilding(float x, float z, float scaleX, float scaleY, float scaleZ, float rotation);
void drawUFO(void);
void drawFloor(void);
void calculateNormal(float v0[3], float v1[3], float v2[3]);

// Display 3D scene
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen and depth buffer
    glLoadIdentity();  // Reset transformations
    
    // Set the lighting properties
    if (isOrbiting) {
        // Light is orbiting the scene
        lightPos[0] = orbitRadius * cos(orbitAngle * M_PI / 180.0f);  // X position of the light (orbiting)
        lightPos[2] = orbitRadius * sin(orbitAngle * M_PI / 180.0f);  // Z position of the light (orbiting)
    } else {
        // Light is in manual mode, use manualLightX and manualLightZ
        lightPos[0] = manualLightX;  // X position for manual mode
        lightPos[2] = manualLightZ;  // Z position for manual mode
    }

    // Set the updated light position in the scene
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);   // Set the ambient light component
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);   // Set the diffuse light component
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular); // Set the specular light component

    // Set up the projection matrix (3D perspective or orthogonal)
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (height == 0) height = 1;  // Prevent division by zero
    float aspect = (float)width / height;

    glMatrixMode(GL_PROJECTION);  // Switch to projection matrix for 3D rendering
    glLoadIdentity();  // Reset the projection matrix

    if (viewMode == 0) {
        // Orthogonal projection (Overhead view)
        if (aspect >= 1.0f) {
            glOrtho(-20.0 * aspect, 20.0 * aspect, -20.0, 20.0, 1.0, 100.0);
        } else {
            glOrtho(-20.0, 20.0, -20.0 / aspect, 20.0 / aspect, 1.0, 100.0);
        }
    } else if (viewMode == 1 || viewMode == 2) {
        // Perspective projection
        gluPerspective(60.0, aspect, 1.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);  // Switch back to modelview matrix for rendering the scene
    glLoadIdentity();  // Reset the modelview matrix

    // Set up the camera view for each mode
    if (viewMode == 0 || viewMode == 1) {
        // Overhead camera view for orthogonal and perspective projections
        gluLookAt(0.0, 30.0, 30.0,  // Eye position (high and far back)
                  0.0, 0.0, 0.0,    // Looking at the origin (center of the city)
                  0.0, 1.0, 0.0);   // Up vector (Y-axis is up)
    } else if (viewMode == 2) {
        // First-person view with dynamic movement
        float yawRadians = yaw * (M_PI / 180.0f);  // Convert yaw to radians

        // Update the camera position and direction based on yaw
        gluLookAt(cameraX, 1.8, cameraZ,  // Eye position at cameraX, 1.8 units above ground, cameraZ
                  cameraX - sin(yawRadians), 1.8, cameraZ - cos(yawRadians),  // Look in the yaw direction (subtract to flip)
                  0.0, 1.0, 0.0);  // Up vector (Y-axis is up)
    }

    // Now, draw the light source as a sphere in 3D context
    glPushMatrix();
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);  // Move to the light position
    glColor3f(1.0, 1.0, 0.0);  // Yellow color for the light (sun-like appearance)
    glutSolidSphere(0.5, 20, 20);  // Draw the sphere with radius 0.5
    glPopMatrix();
    
    drawFloor();  // Draw gray floor

    // Central building cluster
    positionAndScaleBuilding(0.0, 0.0, 1.0, 2.0, 1.0, 0.0);  // Center building
    positionAndScaleBuilding(2.0, -2.0, 0.8, 2.5, 0.8, 45.0);  // Right-forward building
    positionAndScaleBuilding(-2.0, -2.0, 0.8, 2.5, 0.8, -45.0);  // Left-forward building

    // Symmetrical side clusters (right side)
    positionAndScaleBuilding(3.0, -5.0, 1.5, 3.0, 1.5, 30.0);  // Larger building
    positionAndScaleBuilding(1.0, -4.0, 0.7, 2.0, 0.7, 15.0);  // Smaller, rotated building
    positionAndScaleBuilding(4.0, -7.0, 1.0, 2.5, 1.0, 60.0);  // Rotated and scaled building

    // Symmetrical side clusters (left side)
    positionAndScaleBuilding(-3.0, -5.0, 1.5, 3.0, 1.5, -30.0);  // Larger building (mirrored)
    positionAndScaleBuilding(-1.0, -4.0, 0.7, 2.0, 0.7, -15.0);  // Smaller, rotated building (mirrored)
    positionAndScaleBuilding(-4.0, -7.0, 1.0, 2.5, 1.0, -60.0);  // Rotated and scaled building (mirrored)

    // Further back buildings (right side)
    positionAndScaleBuilding(6.0, -8.0, 0.6, 4.0, 0.6, 0.0);  // Tall, narrow building
    positionAndScaleBuilding(5.0, -10.0, 1.2, 1.5, 1.2, 0.0);  // Shorter, wider building

    // Further back buildings (left side)
    positionAndScaleBuilding(-6.0, -8.0, 0.6, 4.0, 0.6, 0.0);  // Tall, narrow building (mirrored)
    positionAndScaleBuilding(-5.0, -10.0, 1.2, 1.5, 1.2, 0.0);  // Shorter, wider building (mirrored)

    // Place the UFO above the center of the city
    glPushMatrix();
    glTranslatef(0.0, 5.0, 0.0);  // Hovering above the center
    glScalef(1.0, 1.0, 1.0);  // Uniform scaling for the UFO
    drawUFO();
    glPopMatrix();
    
    // Draw HUD in 2D over the scene
    glMatrixMode(GL_PROJECTION);  // Switch to projection matrix for 2D overlay
    glLoadIdentity();  // Reset projection matrix
    gluOrtho2D(0, width, 0, height);  // Set orthogonal projection for 2D text
    glMatrixMode(GL_MODELVIEW);  // Switch back to modelview matrix
    glLoadIdentity();  // Reset modelview matrix
    
    // Disable lighting temporarily for text rendering
    glDisable(GL_LIGHTING);
    
    // Render the current view mode in the top-left corner
    if (viewMode == 0) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: Orthogonal");
    } else if (viewMode == 1) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: Perspective");
    } else if (viewMode == 2) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: First-Person");
        renderBitmapString(10, height - 40, GLUT_BITMAP_HELVETICA_18, "Use arrow keys to move");
    }
    
    // Re-enable lighting after rendering the text
    glEnable(GL_LIGHTING);
    
    glutSwapBuffers();
}

// Handles keyboard input for camera movement
void specialKeys(int key, int x, int y) {
    if (viewMode == 2) {  // Only apply movement in first-person view
        switch (key) {
            case GLUT_KEY_LEFT:  // Rotate right (yaw right)
                yaw += rotationSpeed;
                break;
            case GLUT_KEY_RIGHT:  // Rotate left (yaw left)
                yaw -= rotationSpeed;
                break;
            case GLUT_KEY_UP:  // Move forward (increased speed)
                cameraX -= moveSpeed * sin(yaw * M_PI / 180.0f);
                cameraZ -= moveSpeed * cos(yaw * M_PI / 180.0f);
                break;
            case GLUT_KEY_DOWN:  // Move backward (increased speed)
                cameraX += moveSpeed * sin(yaw * M_PI / 180.0f);
                cameraZ += moveSpeed * cos(yaw * M_PI / 180.0f);
                break;
        }
    }
    glutPostRedisplay();
}

// Handles keyboard input for switching views and exiting the program
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // ESC key to exit
        exit(0);  // Exit the program
    } else if (key == 'm' || key == 'M') {
        viewMode = (viewMode + 1) % 3;  // Cycle through view modes
        glutPostRedisplay();  // Redraw the scene
    } else if (key == 'o' || key == 'O') {
        // Toggle orbiting mode
        isOrbiting = !isOrbiting;  // Switch between orbiting and manual control
        glutPostRedisplay();  // Redraw the scene
    } else if (!isOrbiting) {  // Manual light control (only when orbiting is off)
        float moveAmount = 1.0f;  // Amount to move the light
        if (key == 'w' || key == 'W') {
            manualLightZ -= moveAmount;  // Move light forward
        } else if (key == 's' || key == 'S') {
            manualLightZ += moveAmount;  // Move light backward
        } else if (key == 'a' || key == 'A') {
            manualLightX -= moveAmount;  // Move light left
        } else if (key == 'd' || key == 'D') {
            manualLightX += moveAmount;  // Move light right
        }
    }

    // Controls for raising/lowering the light and adjusting orbit radius
    if (key == 'r' || key == 'R') {
        lightPos[1] += 1.0f;  // Raise the light
    } else if (key == 'f' || key == 'F') {
        lightPos[1] -= 1.0f;  // Lower the light
    } else if (key == 'q' || key == 'Q') {
        orbitRadius += 1.0f;  // Increase orbit radius
    } else if (key == 'e' || key == 'E') {
        orbitRadius -= 1.0f;  // Decrease orbit radius
    }

    glutPostRedisplay();
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    // Set the position of the text in window coordinates
    glRasterPos2f(x, y);
    
    // Loop through each character and render it
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Initialization function for setting up OpenGL properties
void initOpenGL(void) {
    glEnable(GL_DEPTH_TEST);  // Enable depth testing (for hidden surface removal)
    glClearColor(0.05, 0.05, 0.15, 1.0);  // Dark blue sky color
    
    glEnable(GL_LIGHTING);  // Enable lighting calculations in OpenGL
    glEnable(GL_LIGHT0);    // Enable the first light source (Light 0)
    glEnable(GL_COLOR_MATERIAL);  // Enable OpenGL to use the object's color for lighting calculations
    
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enables two-sided lighting
}

// Reshape function to handle resizing and aspect ratio
void reshape(int width, int height) {
    if (height == 0) height = 1;  // Prevent division by zero if height is zero
    glViewport(0, 0, width, height);  // Set the viewport to cover the new window size
}

// Function to calculate normals for surfaces
void calculateNormal(float v0[3], float v1[3], float v2[3]) {
    float A[3], B[3], normal[3];

    A[0] = v1[0] - v0[0];
    A[1] = v1[1] - v0[1];
    A[2] = v1[2] - v0[2];

    B[0] = v2[0] - v0[0];
    B[1] = v2[1] - v0[1];
    B[2] = v2[2] - v0[2];

    normal[0] = A[1] * B[2] - A[2] * B[1];
    normal[1] = A[2] * B[0] - A[0] * B[2];
    normal[2] = A[0] * B[1] - A[1] * B[0];

    float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] /= length;
    normal[1] /= length;
    normal[2] /= length;

    glNormal3f(normal[0], normal[1], normal[2]);
}

void drawAbstractBuilding(void) {
    // Draw the main body of the building (single-sided lighting)
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);  // Ensure two-sided lighting is off for main building

    glBegin(GL_QUADS);  // Start defining quadrilaterals

    // Front face
    float v0[] = {-0.5, 0.0, 0.5};
    float v1[] = {0.5, 0.0, 0.5};
    float v2[] = {0.3, 1.0, 0.3};
    calculateNormal(v0, v1, v2);
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, 0.3);

    // Back face
    float v3[] = {-0.5, 0.0, -0.5};
    float v4[] = {0.5, 0.0, -0.5};
    float v5[] = {0.3, 1.0, -0.3};
    calculateNormal(v3, v4, v5);
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(-0.3, 1.0, -0.3);

    // Left face
    float v6[] = {-0.5, 0.0, -0.5};
    float v7[] = {-0.5, 0.0, 0.5};
    float v8[] = {-0.3, 1.0, 0.3};
    calculateNormal(v6, v7, v8);
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(-0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, -0.3);

    // Right face
    float v9[] = {0.5, 0.0, -0.5};
    float v10[] = {0.5, 0.0, 0.5};
    float v11[] = {0.3, 1.0, 0.3};
    calculateNormal(v9, v10, v11);
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(0.3, 1.0, -0.3);

    // Top face
    float v12[] = {-0.3, 1.0, -0.3};
    float v13[] = {0.3, 1.0, -0.3};
    float v14[] = {0.3, 1.0, 0.3};
    calculateNormal(v12, v13, v14);
    glColor3f(0.0, 0.5, 1.0);  // Dark blue with a neon glow
    glVertex3f(-0.3, 1.0, -0.3);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, 0.3);

    glEnd();  // End quadrilateral definitions

    // Now enable two-sided lighting for neon edges and spikes
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enable two-sided lighting for neon edges

    // Draw the neon edges and spikes (neon-highlighted spikes)
    glBegin(GL_QUADS);

    // Front neon edge
    float v15[] = {-0.5, 0.0, 0.5};
    float v16[] = {-0.3, 1.0, 0.3};
    float v17[] = {-0.35, 1.05, 0.35};
    calculateNormal(v15, v16, v17);
    glColor3f(0.0, 1.0, 1.0);  // Neon cyan
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(-0.3, 1.0, 0.3);
    glVertex3f(-0.35, 1.05, 0.35);
    glVertex3f(-0.55, 0.0, 0.55);

    // Back neon edge
    float v18[] = {0.5, 0.0, -0.5};
    float v19[] = {0.3, 1.0, -0.3};
    float v20[] = {0.35, 1.05, -0.35};
    calculateNormal(v18, v19, v20);
    glColor3f(0.0, 1.0, 0.0);  // Neon green
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(0.35, 1.05, -0.35);
    glVertex3f(0.55, 0.0, -0.55);

    glEnd();  // End neon edges

    // Draw spikes
    glBegin(GL_TRIANGLES);

    // Neon spike 1
    float v21[] = {0.0, 1.5, 0.0};
    float v22[] = {-0.1, 1.0, 0.1};
    float v23[] = {0.1, 1.0, 0.1};
    calculateNormal(v21, v22, v23);
    glColor3f(0.0, 1.0, 0.5);  // Neon green spike
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(-0.1, 1.0, 0.1);
    glVertex3f(0.1, 1.0, 0.1);

    // Neon spike 2
    float v24[] = {0.0, 1.5, 0.0};
    float v25[] = {0.1, 1.0, -0.1};
    float v26[] = {-0.1, 1.0, -0.1};
    calculateNormal(v24, v25, v26);
    glColor3f(0.0, 1.0, 1.0);  // Neon blue spike
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(0.1, 1.0, -0.1);
    glVertex3f(-0.1, 1.0, -0.1);

    glEnd();  // End spikes

    // Disable two-sided lighting after neon edges and spikes
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

// Helper function to randomly position and scale each building symmetrically
void positionAndScaleBuilding(float x, float z, float scaleX, float scaleY, float scaleZ, float rotation) {
    glPushMatrix();  // Save the current transformation state

    // Translate the building to its position (x, z coordinates for ground placement)
    glTranslatef(x, 0.0, z);

    // Apply scaling for width (scaleX), height (scaleY), and depth (scaleZ)
    glScalef(scaleX, scaleY, scaleZ);

    // Apply rotation around the Y-axis to add variety
    glRotatef(rotation, 0.0, 1.0, 0.0);

    // Draw the abstract building with these transformations
    drawAbstractBuilding();

    glPopMatrix();  // Restore the transformation state after drawing
}

void drawUFO(void) {
    // Draw the base of the UFO (dark disc)
    glBegin(GL_TRIANGLE_FAN);  // Using triangle fan to create a disc shape
    glNormal3f(0.0f, -1.0f, 0.0f);  // Normal points downwards for the base
    glColor3f(0.1f, 0.1f, 0.1f);  // Dark gray color for the base
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center of the base

    // Loop to create the circular base of the UFO
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159f / 180.0f);  // Convert degrees to radians
        float x = 1.5f * cos(angle);  // X-coordinate of the point on the circle
        float z = 1.5f * sin(angle);  // Z-coordinate of the point on the circle

        // Normal for the circular base points straight down
        glNormal3f(0.0f, -1.0f, 0.0f);  // Ensuring each point on the base has a downward normal
        glVertex3f(x, 0.0f, z);  // Base of the UFO at y = 0
    }
    glEnd();

    // Now draw the dome of the UFO
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.5f, 0.0f);  // Tip of the dome

    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159f / 180.0f);  // Convert degrees to radians
        float x = 0.8f * cos(angle);  // X-coordinate
        float z = 0.8f * sin(angle);  // Z-coordinate

        // Calculate the normal for the dome
        float length = sqrt(x * x + 0.5f * 0.5f + z * z);
        glNormal3f(x / length, 0.5f / length, z / length);  // Normal pointing outward
        glVertex3f(x, 0.0f, z);  // Bottom vertices of the dome
    }
    glEnd();

    // Two-sided lighting is relevant here because we want the bottom of the UFO to respond to light
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enable two-sided lighting for neon lights

    // Draw neon lights on the edge of the base
    glColor3f(0.0f, 1.0f, 1.0f);  // Neon cyan for the base lights
    glBegin(GL_POINTS);
    for (int i = 0; i <= 360; i += 20) {  // Place lights every 20 degrees
        float angle = i * (3.14159f / 180.0f);
        float x = 1.5f * cos(angle);  // X-coordinate of the light
        float z = 1.5f * sin(angle);  // Z-coordinate of the light

        // Normal for each light points outward, along the radius of the UFO base
        float length = sqrt(x * x + z * z);
        glNormal3f(x / length, 0.0f, z / length);  // Normals point outward from the center of the base
        glVertex3f(x, 0.0f, z);  // Place the light on the edge of the base
    }
    glEnd();

    // Draw neon lights on the edge of the top dome
    glColor3f(0.0f, 1.0f, 0.0f);  // Neon green for the top lights
    glBegin(GL_POINTS);
    for (int i = 0; i <= 360; i += 20) {  // Place lights every 20 degrees
        float angle = i * (3.14159f / 180.0f);
        float x = 0.8f * cos(angle);  // X-coordinate of the neon light
        float z = 0.8f * sin(angle);  // Z-coordinate of the neon light

        // Normal for each light points outward, along the radius of the dome
        float length = sqrt(x * x + z * z);
        glNormal3f(x / length, 0.0f, z / length);  // Normals point outward from the center of the dome
        glVertex3f(x, 0.0f, z);  // Place the light on the edge of the dome
    }
    glEnd();

    // Disable two-sided lighting for the rest of the scene
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}


void drawFloor(void) {
    float floorSize = 25.0f;

    glBegin(GL_QUADS);

    // Set the normal for the entire floor, as it is a flat surface pointing upwards
    glNormal3f(0.0f, 1.0f, 0.0f);  // Normal pointing straight up

    // Define the vertices for the floor quad (in counter-clockwise order)
    glColor3f(0.3f, 0.3f, 0.3f);  // Medium gray color for the floor
    glVertex3f(-floorSize, 0.0f, -floorSize);  // Bottom-left corner
    glVertex3f(floorSize, 0.0f, -floorSize);   // Bottom-right corner
    glVertex3f(floorSize, 0.0f, floorSize);    // Top-right corner
    glVertex3f(-floorSize, 0.0f, floorSize);   // Top-left corner

    glEnd();
}

void idle(void) {
    if (isOrbiting) {
        orbitAngle += orbitSpeed;  // Continuously update the orbit angle
        if (orbitAngle > 360.0f) {
            orbitAngle -= 360.0f;  // Keep the angle within [0, 360]
        }
    }
    glutPostRedisplay();  // Redraw the scene to update the light position
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Set display mode (double buffering, RGB, depth)
    glutInitWindowSize(800, 600);  // Set initial window size
    glutCreateWindow("Roman Di Domizio - Homework 5");  // Create window with name in title

    initOpenGL();  // Call our function to set up OpenGL settings

    // Register callback functions
    glutDisplayFunc(display);  // Display function
    glutReshapeFunc(reshape);  // Reshape function for when window is resized
    glutSpecialFunc(specialKeys);  // Register the special keys handler (for arrow keys)
    glutKeyboardFunc(keyboard);  // Register the keyboard handler for exiting
    glutIdleFunc(idle);  // Register idle function for continuous updates

    glutMainLoop();  // Enter the main loop and wait for events
    return 0;
}
