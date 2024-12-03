//
//  main.c
//  Homework6
//
//  Created by Roman Di Domizio on 10/16/24.
//

#include "CSCIx229.h"

// Texture IDs
GLuint blockTexture, floorTexture, lightTexture, ufoTexture;

// Camera control variables
int viewMode = 0;  // Global variable to track the current view mode
float cameraDistance = 13.0f;  // Distance from the center of the scene
float cameraX = 0.0f;  // Initial X position of the camera
float cameraY = 5.0f;  // Start slightly above the UFO (UFO is at 5.0f)
float cameraZ = 13.0f; // Set a distance away from the UFO
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
void calculateNormal(float v0[3], float v1[3], float v2[3], float normal[3]);

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
        gluLookAt(cameraX, cameraY, cameraZ,  // Eye position at cameraX, cameraY units above ground, cameraZ
                  cameraX - sin(yawRadians), cameraY, cameraZ - cos(yawRadians),  // Look in the yaw direction (subtract to flip)
                  0.0, 1.0, 0.0);  // Up vector (Y-axis is up)
    }

    // Draw the light source as a textured sphere in 3D context
    glPushMatrix();
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);  // Move to the light position

    // Enable 2D texturing and bind the light texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lightTexture);  // Make sure lightTexture is loaded and assigned
    
    // Set the object color to white to display the texture's actual color
    glColor3f(1.0, 1.0, 1.0);

    // Set texture environment mode to modulate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Create a new quadric object for rendering the sphere
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);  // Enable texturing for the quadric

    // Draw a textured sphere with the applied texture
    gluSphere(quad, 0.5, 20, 20);  // Radius 0.5, 20 slices, 20 stacks

    // Free the quadric object
    gluDeleteQuadric(quad);

    // Disable texturing after drawing the sphere
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();  // Restore the transformation state
    
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

    // Vertical movement controls for first-person mode
    if (viewMode == 2) {  // Only apply in first-person view
        float verticalSpeed = 0.5f;  // Speed of vertical movement
        if (key == 'p' || key == 'P') {
            cameraY += verticalSpeed;  // Move up
        } else if (key == 'l' || key == 'L') {
            cameraY -= verticalSpeed;  // Move down
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
    glClearColor(0.02, 0.02, 0.1, 1.0);  // Dark night sky color (very dark blue)

    glEnable(GL_LIGHTING);  // Enable lighting calculations in OpenGL
    glEnable(GL_LIGHT0);    // Enable the first light source (Light 0)
    glEnable(GL_COLOR_MATERIAL);  // Enable OpenGL to use the object's color for lighting calculations

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enables two-sided lighting

    // Set ambient light to a very low value (darker environment)
    GLfloat ambientLight[] = {0.05f, 0.05f, 0.1f, 1.0f}; // Dim ambient light (slightly blue-tinted)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    // Set diffuse light to a brighter white (moonlight effect)
    GLfloat diffuseLight[] = {0.8f, 0.8f, 1.0f, 1.0f}; // Bright, cool white moonlight
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

    // Set specular light for shiny reflections (moonlight reflections)
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White specular highlights
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Position of the moonlight (bright light source in the sky)
    GLfloat lightPos[] = {0.0f, 30.0f, 30.0f, 1.0f}; // Positioned above and in front of the scene
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Load the BMP texture
    blockTexture = LoadTexBMP("Facade008_2K-JPG_Color.bmp");
    floorTexture = LoadTexBMP("Tiles055_2K-JPG_Color.bmp");
    lightTexture = LoadTexBMP("moon.bmp");
    ufoTexture = LoadTexBMP("MetalPlates006_2K-JPG_Color.bmp");


    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Enable two-sided lighting for more realistic lighting
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    // Automatically normalize normals after transformations
    glEnable(GL_NORMALIZE);
}

// Reshape function to handle resizing and aspect ratio
void reshape(int width, int height) {
    if (height == 0) height = 1;  // Prevent division by zero if height is zero
    glViewport(0, 0, width, height);  // Set the viewport to cover the new window size
}

// Function to calculate normals for surfaces
void calculateNormal(float v0[3], float v1[3], float v2[3], float normal[3]) {
    float A[3], B[3];

    A[0] = v1[0] - v0[0];
    A[1] = v1[1] - v0[1];
    A[2] = v1[2] - v0[2];

    B[0] = v2[0] - v0[0];
    B[1] = v2[1] - v0[1];
    B[2] = v2[2] - v0[2];

    // Cross product to compute normal
    normal[0] = A[1] * B[2] - A[2] * B[1];
    normal[1] = A[2] * B[0] - A[0] * B[2];
    normal[2] = A[0] * B[1] - A[1] * B[0];

    // Normalize the normal vector
    float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] /= length;
    normal[1] /= length;
    normal[2] /= length;
}

void drawAbstractBuilding(void) {
    // Define the texture repetition factor
    float textureRepeat = 1.0f; // Controls how many times the texture repeats (adjust as needed)

    // Draw the main body of the building (single-sided lighting)
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);  // Ensure two-sided lighting is off for main building

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockTexture);  // Bind the block texture

    // Set texture environment mode to modulate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Set texture wrapping mode to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set the object color to white to display the texture's actual color
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);  // Start defining quadrilaterals

    // Front slanted face (with adjustable texture repetition)
    float v0[] = {-0.5, 0.0, 0.5};
    float v1[] = {0.5, 0.0, 0.5};
    float v2[] = {0.3, 1.0, 0.3};  // Slanted top-right point
    float v3[] = {-0.3, 1.0, 0.3}; // Slanted top-left point
    float normal[3];
    calculateNormal(v0, v1, v2, normal);  // Calculate normal for lighting
    glNormal3fv(normal);  // Apply the normal
    glTexCoord2f(0.0, 0.0); glVertex3f(v0[0], v0[1], v0[2]);  // Bottom-left corner
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v1[0], v1[1], v1[2]);  // Bottom-right corner
    glTexCoord2f(textureRepeat * 0.8, textureRepeat); glVertex3f(v2[0], v2[1], v2[2]);  // Top-right corner
    glTexCoord2f(textureRepeat * 0.2, textureRepeat); glVertex3f(v3[0], v3[1], v3[2]);  // Top-left corner

    // Back slanted face
    float v4[] = {-0.5, 0.0, -0.5};
    float v5[] = {0.5, 0.0, -0.5};
    float v6[] = {0.3, 1.0, -0.3};  // Slanted top-right point
    float v7[] = {-0.3, 1.0, -0.3}; // Slanted top-left point
    calculateNormal(v4, v5, v6, normal);  // Calculate normal for lighting
    glNormal3fv(normal);  // Apply the normal
    glTexCoord2f(0.0, 0.0); glVertex3f(v4[0], v4[1], v4[2]);  // Bottom-left corner
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v5[0], v5[1], v5[2]);  // Bottom-right corner
    glTexCoord2f(textureRepeat * 0.8, textureRepeat); glVertex3f(v6[0], v6[1], v6[2]);  // Top-right corner
    glTexCoord2f(textureRepeat * 0.2, textureRepeat); glVertex3f(v7[0], v7[1], v7[2]);  // Top-left corner

    // Left slanted face
    float v8[] = {-0.5, 0.0, -0.5};
    float v9[] = {-0.5, 0.0, 0.5};
    float v10[] = {-0.3, 1.0, 0.3};  // Slanted top-right point
    float v11[] = {-0.3, 1.0, -0.3}; // Slanted top-left point
    calculateNormal(v8, v9, v10, normal);  // Calculate normal for lighting
    glNormal3fv(normal);  // Apply the normal
    glTexCoord2f(0.0, 0.0); glVertex3f(v8[0], v8[1], v8[2]);  // Bottom-left corner
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v9[0], v9[1], v9[2]);  // Bottom-right corner
    glTexCoord2f(textureRepeat * 0.8, textureRepeat); glVertex3f(v10[0], v10[1], v10[2]);  // Top-right corner
    glTexCoord2f(textureRepeat * 0.2, textureRepeat); glVertex3f(v11[0], v11[1], v11[2]);  // Top-left corner

    // Right slanted face
    float v12[] = {0.5, 0.0, -0.5};
    float v13[] = {0.5, 0.0, 0.5};
    float v14[] = {0.3, 1.0, 0.3};  // Slanted top-right point
    float v15[] = {0.3, 1.0, -0.3}; // Slanted top-left point
    calculateNormal(v12, v13, v14, normal);  // Calculate normal for lighting
    glNormal3fv(normal);  // Apply the normal
    glTexCoord2f(0.0, 0.0); glVertex3f(v12[0], v12[1], v12[2]);  // Bottom-left corner
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v13[0], v13[1], v13[2]);  // Bottom-right corner
    glTexCoord2f(textureRepeat * 0.8, textureRepeat); glVertex3f(v14[0], v14[1], v14[2]);  // Top-right corner
    glTexCoord2f(textureRepeat * 0.2, textureRepeat); glVertex3f(v15[0], v15[1], v15[2]);  // Top-left corner

    glEnd();  // End quadrilateral definitions

    // Disable texturing after applying to the building base
    glDisable(GL_TEXTURE_2D);

    // Now enable two-sided lighting for neon edges and spikes
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enable two-sided lighting for neon edges

    // Draw the neon edges and spikes (neon-highlighted spikes)
    glBegin(GL_QUADS);

    // Front neon edge
    float v20[] = {-0.5, 0.0, 0.5};
    float v21[] = {-0.3, 1.0, 0.3};
    float v22[] = {-0.35, 1.05, 0.35};
    calculateNormal(v20, v21, v22, normal);  // Calculate normal for this face
    glNormal3fv(normal);  // Apply the normal
    glColor3f(0.0, 1.0, 1.0);  // Neon cyan color
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(-0.3, 1.0, 0.3);
    glVertex3f(-0.35, 1.05, 0.35);
    glVertex3f(-0.55, 0.0, 0.55);

    // Back neon edge
    float v23[] = {0.5, 0.0, -0.5};
    float v24[] = {0.3, 1.0, -0.3};
    float v25[] = {0.35, 1.05, -0.35};
    calculateNormal(v23, v24, v25, normal);  // Calculate normal for the back face
    glNormal3fv(normal);  // Apply the normal
    glColor3f(0.0, 1.0, 0.0);  // Neon green color
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(0.35, 1.05, -0.35);
    glVertex3f(0.55, 0.0, -0.55);

    glEnd();  // End neon edges

    // Draw the neon spikes
    glBegin(GL_TRIANGLES);

    // Neon spike 1
    float v26[] = {0.0, 1.5, 0.0};
    float v27[] = {-0.1, 1.0, 0.1};
    float v28[] = {0.1, 1.0, 0.1};
    calculateNormal(v26, v27, v28, normal);  // Calculate normal for spike 1
    glNormal3fv(normal);  // Apply the normal
    glColor3f(0.0, 1.0, 0.5);  // Neon green spike color
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(-0.1, 1.0, 0.1);
    glVertex3f(0.1, 1.0, 0.1);

    // Neon spike 2
    float v29[] = {0.0, 1.5, 0.0};
    float v30[] = {0.1, 1.0, -0.1};
    float v31[] = {-0.1, 1.0, -0.1};
    calculateNormal(v29, v30, v31, normal);  // Calculate normal for spike 2
    glNormal3fv(normal);  // Apply the normal
    glColor3f(0.0, 1.0, 1.0);  // Neon blue spike color
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(0.1, 1.0, -0.1);
    glVertex3f(-0.1, 1.0, -0.1);

    glEnd();  // End neon spikes

    // Disable two-sided lighting after drawing the neon edges and spikes
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

    // Enable normalizing of the normals after transformations like scaling
    glEnable(GL_NORMALIZE);

    // Draw the abstract building with these transformations
    drawAbstractBuilding();

    glDisable(GL_NORMALIZE);  // Disable normalizing after the building is drawn

    glPopMatrix();  // Restore the transformation state after drawing
}

void drawUFO(void) {
    // Enable 2D texturing and bind the UFO texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ufoTexture);  // Make sure "ufoTexture" is loaded

    // Set texture environment mode to modulate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Set texture wrapping mode to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Draw the base of the UFO (dark disc)
    glBegin(GL_TRIANGLE_FAN);  // Using triangle fan to create a disc shape
    glNormal3f(0.0f, -1.0f, 0.0f);  // Normal points downwards for the base
    glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white for texture
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f, 0.0f);  // Center of the base

    // Loop to create the circular base of the UFO
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159f / 180.0f);  // Convert degrees to radians
        float x = 1.5f * cos(angle);  // X-coordinate of the point on the circle
        float z = 1.5f * sin(angle);  // Z-coordinate of the point on the circle

        // Normal for the circular base points straight down
        glNormal3f(0.0f, -1.0f, 0.0f);  // Ensuring each point on the base has a downward normal
        glTexCoord2f(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));  // Texture coordinates for circular mapping
        glVertex3f(x, 0.0f, z);  // Base of the UFO at y = 0
    }
    glEnd();

    // Now draw the dome of the UFO
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white for texture
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.5f, 0.0f);  // Tip of the dome

    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14159f / 180.0f);  // Convert degrees to radians
        float x = 0.8f * cos(angle);  // X-coordinate
        float z = 0.8f * sin(angle);  // Z-coordinate

        // Calculate the normal for the dome
        float length = sqrt(x * x + 0.5f * 0.5f + z * z);
        glNormal3f(x / length, 0.5f / length, z / length);  // Normal pointing outward
        glTexCoord2f(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));  // Texture coordinates for circular mapping
        glVertex3f(x, 0.0f, z);  // Bottom vertices of the dome
    }
    glEnd();

    // Disable texturing after drawing the UFO
    glDisable(GL_TEXTURE_2D);

    // Enable two-sided lighting for neon lights
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // Enable two-sided lighting for neon lights

    // Set up emissive properties for the neon lights
    GLfloat neonEmissive[] = {0.0f, 1.0f, 1.0f, 1.0f};  // Bright neon cyan
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, neonEmissive);

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

    // Set up emissive properties for the top neon lights
    GLfloat greenEmissive[] = {0.0f, 1.0f, 0.0f, 1.0f};  // Bright neon green
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, greenEmissive);

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

    // Disable emissive properties after drawing the neon lights
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);

    // Disable two-sided lighting for the rest of the scene
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}


void drawFloor(void) {
    float floorSize = 25.0f;

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTexture);  // Bind the floor texture

    // Set texture wrapping mode to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture environment mode to modulate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Set the object color to white to display the texture's actual color
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);  // Start defining the floor quad

    // Set the normal for the entire floor, as it is a flat surface pointing upwards
    glNormal3f(0.0f, 1.0f, 0.0f);  // Normal pointing straight up

    // Define the vertices for the floor quad with repeated texture coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-floorSize, 0.0f, -floorSize);  // Bottom-left corner
    glTexCoord2f(10.0, 0.0); glVertex3f(floorSize, 0.0f, -floorSize);   // Bottom-right corner (repeat 10 times)
    glTexCoord2f(10.0, 10.0); glVertex3f(floorSize, 0.0f, floorSize);    // Top-right corner (repeat 10 times)
    glTexCoord2f(0.0, 10.0); glVertex3f(-floorSize, 0.0f, floorSize);   // Top-left corner

    glEnd();  // End defining the floor quad

    // Disable texturing after drawing the floor
    glDisable(GL_TEXTURE_2D);
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
    glutCreateWindow("Roman Di Domizio - Homework 6");  // Create window with name in title

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

