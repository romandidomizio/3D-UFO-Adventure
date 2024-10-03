//
//  main.c
//  Homework3
//
//  Created by Roman Di Domizio on 9/25/24.
//

#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Camera control variables
float yaw = 0.0f;     // Yaw angle for rotating left and right (start on the opposite side, 180 degrees)
float pitch = 20.0f;    // Pitch angle for rotating up and down (slightly looking down)
float cameraDistance = 13.0f;  // Distance from the center of the scene
float rotationSpeed = 2.0f;    // Speed of rotation for the camera

// Function prototypes
void drawAbstractBuilding(void);
void positionAndScaleBuilding(float x, float z, float scaleX, float scaleY, float scaleZ, float rotation);
void drawUFO(void);
void drawFloor(void);

// Display 3D scene
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen and depth buffer

    glLoadIdentity();  // Reset transformations

    // Convert yaw and pitch angles to radians for trigonometric functions
    float yawRadians = yaw * (M_PI / 180.0f);
    float pitchRadians = pitch * (M_PI / 180.0f);

    // Calculate the camera position using spherical coordinates
    float cameraX = cameraDistance * cos(pitchRadians) * sin(yawRadians);
    float cameraY = cameraDistance * sin(pitchRadians);
    float cameraZ = cameraDistance * cos(pitchRadians) * cos(yawRadians);

    gluLookAt(cameraX, cameraY, cameraZ,  // Camera position
              0.0, 2.0, 0.0,             // Look above origin (center of the city)
              0.0, 1.0, 0.0);            // Up vector (Y-axis is up)
    
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
    
    glutSwapBuffers();  // Swap buffers for smooth rendering
}

// Handles keyboard input for camera movement
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  // Rotate left (yaw left)
            yaw -= rotationSpeed;  // Decrease yaw angle
            break;
        case GLUT_KEY_RIGHT:  // Rotate right (yaw right)
            yaw += rotationSpeed;  // Increase yaw angle
            break;
        case GLUT_KEY_UP:  // Rotate up (pitch up)
            if (pitch < 89.0f) {  // Limit pitch to avoid flipping over
                pitch += rotationSpeed;
            }
            break;
        case GLUT_KEY_DOWN:  // Rotate down (pitch down)
            if (pitch > -89.0f) {  // Limit pitch to avoid flipping over
                pitch -= rotationSpeed;
            }
            break;
    }
    glutPostRedisplay();  // Redraw the scene after camera movement
}

// ESC to quit
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // 27 is the ASCII code for the ESC key
        exit(0);  // Exit the program when ESC is pressed
    }
}

// Initialization function for setting up OpenGL properties
void initOpenGL(void) {
    glEnable(GL_DEPTH_TEST);  // Enable depth testing (for hidden surface removal)
    glClearColor(0.05, 0.05, 0.15, 1.0);  // Dark blue sky color
}

// Reshape function for when the window size changes
void reshape(int width, int height) {
    glViewport(0, 0, width, height);  // Set the viewport to cover the new window size
    glMatrixMode(GL_PROJECTION);  // Switch to projection matrix
    glLoadIdentity();  // Reset the projection matrix
    gluPerspective(60.0, (float)width / height, 1.0, 100.0);  // Set perspective projection
    glMatrixMode(GL_MODELVIEW);  // Back to modelview matrix
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
    glutCreateWindow("Roman Di Domizio - Homework 3");  // Create window with name in title

    initOpenGL();  // Call our function to set up OpenGL settings

    // Register callback functions
    glutDisplayFunc(display);  // Display function
    glutReshapeFunc(reshape);  // Reshape function for when window is resized
    glutSpecialFunc(specialKeys);  // Register the special keys handler (for arrow keys)
    glutKeyboardFunc(keyboard);  // Register the keyboard handler for exiting

    glutMainLoop();  // Enter the main loop and wait for events
    return 0;  // Exit the program
}
