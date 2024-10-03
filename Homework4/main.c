//
//  main.c
//  Homework4
//
//  Created by Roman Di Domizio on 10/2/24.
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

// Function prototypes
void renderBitmapString(float x, float y, void *font, const char *string);
void drawAbstractBuilding(void);
void positionAndScaleBuilding(float x, float z, float scaleX, float scaleY, float scaleZ, float rotation);
void drawUFO(void);
void drawFloor(void);

// Display 3D scene
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen and depth buffer
    glLoadIdentity();  // Reset transformations

    // Calculate the aspect ratio based on the window size
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (height == 0) height = 1;  // Prevent division by zero
    float aspect = (float)width / height;

    // Set up the projection based on the current view mode
    glMatrixMode(GL_PROJECTION);  // Switch to projection matrix
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

    glMatrixMode(GL_MODELVIEW);  // Switch back to the modelview matrix
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
    
    // Render the current view mode in the top-left corner
    if (viewMode == 0) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: Orthogonal");
    } else if (viewMode == 1) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: Perspective");
    } else if (viewMode == 2) {
        renderBitmapString(10, height - 20, GLUT_BITMAP_HELVETICA_18, "View Mode: First-Person");
        renderBitmapString(10, height - 40, GLUT_BITMAP_HELVETICA_18, "Use arrow keys to move");
    }
    
    glutSwapBuffers();  // Swap buffers for smooth rendering
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
    glutPostRedisplay();  // Redraw the scene after movement
}

// Handles keyboard input for switching views and exiting the program
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // 27 is the ASCII code for the ESC key
        exit(0);  // Exit the program when ESC is pressed
    } else if (key == 'm' || key == 'M') {
        viewMode = (viewMode + 1) % 3;  // Cycle through 0, 1, 2
        glutPostRedisplay();  // Redraw the scene
    }
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
}

// Reshape function to handle resizing and aspect ratio
void reshape(int width, int height) {
    if (height == 0) height = 1;  // Prevent division by zero if height is zero
    glViewport(0, 0, width, height);  // Set the viewport to cover the new window size
}

// Creates building structure
void drawAbstractBuilding(void) {
    // Draw the main body of the building
    glBegin(GL_QUADS);  // Start defining quadrilaterals

    // Front face with neon highlight
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, 0.3);

    // Neon edge highlights on the front
    glColor3f(0.0, 1.0, 1.0);  // Neon cyan
    glVertex3f(-0.5, 0.0, 0.5);  // Bottom-left corner highlight
    glVertex3f(-0.3, 1.0, 0.3);  // Top-left corner highlight
    glVertex3f(-0.35, 1.05, 0.35);  // Extruding neon highlight
    glVertex3f(-0.55, 0.0, 0.55);  // Bottom extrusion highlight

    // Back face (dark tone)
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(-0.3, 1.0, -0.3);

    // Neon edge highlights on the back
    glColor3f(0.0, 1.0, 0.0);  // Neon green
    glVertex3f(0.5, 0.0, -0.5);  // Bottom-right highlight
    glVertex3f(0.3, 1.0, -0.3);  // Top-right highlight
    glVertex3f(0.35, 1.05, -0.35);  // Extruding neon highlight
    glVertex3f(0.55, 0.0, -0.55);  // Bottom extrusion highlight

    // Left face
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(-0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, -0.3);

    // Right face
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(0.3, 1.0, -0.3);

    // Bottom face (not visible but included)
    glColor3f(0.1, 0.1, 0.1);  // Dark body color
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(-0.5, 0.0, 0.5);

    // Top face with a slight glow
    glColor3f(0.0, 0.5, 1.0);  // Dark blue with a neon glow
    glVertex3f(-0.3, 1.0, -0.3);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(0.3, 1.0, 0.3);
    glVertex3f(-0.3, 1.0, 0.3);

    glEnd();  // End quadrilateral definitions

    // Draw spikes on top of the building (neon-highlighted spikes)
    glBegin(GL_TRIANGLES);

    // Neon spike 1
    glColor3f(0.0, 1.0, 0.5);  // Neon green spike
    glVertex3f(0.0, 1.5, 0.0);   // Tip of the spike
    glVertex3f(-0.1, 1.0, 0.1);  // Base
    glVertex3f(0.1, 1.0, 0.1);   // Base

    // Neon spike 2
    glColor3f(0.0, 1.0, 1.0);  // Neon blue spike
    glVertex3f(0.0, 1.5, 0.0);   // Tip of the spike
    glVertex3f(0.1, 1.0, -0.1);  // Base
    glVertex3f(-0.1, 1.0, -0.1);  // Base

    glEnd();
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
    glColor3f(0.1, 0.1, 0.1);  // Dark gray color for the base
    glBegin(GL_TRIANGLE_FAN);  // Using triangle fan to create a disc shape
    glVertex3f(0.0, 0.0, 0.0);  // Center of the base

    // Loop to create the circular base
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159 / 180.0);  // Convert degrees to radians
        float x = 1.5 * cos(angle);  // X-coordinate of the point on the circle
        float z = 1.5 * sin(angle);  // Z-coordinate of the point on the circle
        glVertex3f(x, 0.0, z);  // Base of the UFO at y = 0
    }
    glEnd();

    // Draw the top dome (dark color)
    glColor3f(0.1, 0.1, 0.1);  // Dark gray for the dome
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.5, 0.0);  // Tip of the dome
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159 / 180.0);  // Convert degrees to radians
        float x = 0.8 * cos(angle);  // X-coordinate of the point on the dome's base
        float z = 0.8 * sin(angle);  // Z-coordinate of the point on the dome's base
        glVertex3f(x, 0.0, z);  // Bottom of the dome at y = 0
    }
    glEnd();

    // Draw neon lights on the edge of the base
    glColor3f(0.0, 1.0, 1.0);  // Neon cyan for the lights
    glBegin(GL_POINTS);
    for (int i = 0; i <= 360; i += 20) {  // Place neon lights every 20 degrees
        float angle = i * (3.14159 / 180.0);  // Convert degrees to radians
        float x = 1.5 * cos(angle);  // X-coordinate of the neon light
        float z = 1.5 * sin(angle);  // Z-coordinate of the neon light
        glVertex3f(x, 0.0, z);  // Place the light on the edge of the base
    }
    glEnd();

    // Draw neon lights on the edge of the top dome
    glColor3f(0.0, 1.0, 0.0);  // Neon green for the top lights
    glBegin(GL_POINTS);
    for (int i = 0; i <= 360; i += 20) {
        float angle = i * (3.14159 / 180.0);  // Convert degrees to radians
        float x = 0.8 * cos(angle);  // X-coordinate of the neon light
        float z = 0.8 * sin(angle);  // Z-coordinate of the neon light
        glVertex3f(x, 0.0, z);  // Place the light on the edge of the dome
    }
    glEnd();
}

// Floor panel
void drawFloor(void) {
    glBegin(GL_QUADS);

    // Smaller floor
    glColor3f(0.3, 0.3, 0.3);  // Medium gray color
    glVertex3f(-25.0, 0.0, -25.0);  // Bottom-left corner of the floor (smaller)
    glVertex3f(25.0, 0.0, -25.0);   // Bottom-right corner of the floor
    glVertex3f(25.0, 0.0, 25.0);    // Top-right corner of the floor
    glVertex3f(-25.0, 0.0, 25.0);   // Top-left corner of the floor

    glEnd();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Set display mode (double buffering, RGB, depth)
    glutInitWindowSize(800, 600);  // Set initial window size
    glutCreateWindow("Roman Di Domizio - Homework 4");  // Create window with name in title

    initOpenGL();  // Call our function to set up OpenGL settings

    // Register callback functions
    glutDisplayFunc(display);  // Display function
    glutReshapeFunc(reshape);  // Reshape function for when window is resized
    glutSpecialFunc(specialKeys);  // Register the special keys handler (for arrow keys)
    glutKeyboardFunc(keyboard);  // Register the keyboard handler for exiting

    glutMainLoop();  // Enter the main loop and wait for events
    return 0;  // Exit the program
}
