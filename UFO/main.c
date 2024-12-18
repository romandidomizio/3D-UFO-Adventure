//
// main.c
// UFOPilot
//
// Created by Roman Di Domizio on 10/30/24.
//

#include "CSCIx229.h"

// ---------------------------------------------------------
// Global Variables
// ---------------------------------------------------------

// HUD variables
float titleX = 0.165f; // Horizontal position as a fraction of screen width
float titleY = 0.8f; // Vertical position as a fraction of screen height
float titleSize = 90.0f;                 // Title size (stroke font scaling)
float instructionsX = 0.5f, instructionsY = 0.3f; // Instructions position
float controlsLeftX = 0.1f, controlsLeftY = 0.1f; // Left side controls (WASD)
float controlsRightX = 0.7f, controlsRightY = 0.1f; // Right side controls (Arrow keys)
float instructionsSize = 30.0f;                   // Instructions text size (stroke font scaling)
float controlsSize = 20.0f;                       // Controls text size (stroke font scaling)

bool displayControls = false; // Track whether to display controls

// Key states
#define KEY_COUNT 256
bool keyState[KEY_COUNT] = {false};       // Regular key states
bool specialKeyState[256] = {false};      // Special key states

// Texture IDs
GLuint blockTexture, floorTexture, lightTexture, ufoTexture, skyDomeTexture, sciFiTexture, clockTowerTexture;

// UFO position and orientation
float ufoX = 0.0f, ufoY = 15.0f, ufoZ = 200.0f; // UFO position
float ufoYaw = 0.0f;  // UFO yaw (rotation around Y-axis)

// Movement speed
float moveSpeed = 1.0f;      // Base movement speed (faster for smoother motion)
float turnSpeed = 10.0f;      // Turning speed in degrees
float rotationIncrement = 0.0f; // Incremental rotation for smooth turning
float velocityX = 0.0f;
float velocityZ = 0.0f;
float velocityY = 0.0f;
float ferrisWheelAngle = 0.0f;


// Lighting properties
GLfloat lightPos[] = {0.0f, 10.0f, 10.0f, 1.0f};   // Position of the light source
GLfloat lightAmbient[] = {0.01f, 0.01f, 0.01f, 1.0f}; // Ambient light intensity
GLfloat lightDiffuse[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Diffuse light intensity
GLfloat lightSpecular[] = {0.25f, 0.25f, 0.25f, 1.0f}; // Specular light intensity
int isOrbiting = 1;                  // Flag to toggle orbiting mode for light
float orbitAngle = 0.0f;             // Current orbit angle of the light
float orbitSpeed = 1.0f;             // Orbit speed of the light
float manualLightX = 0.0f, manualLightZ = 10.0f; // Manual light position (when orbiting is off)
float orbitRadius = 15.0f;           // Radius of light orbit

// ---------------------------------------------------------
// Function Prototypes
// ---------------------------------------------------------
void renderBitmapString(float x, float y, void *font, const char *string);
void drawAbstractBuilding(void);
void positionAndScaleBuilding(float x, float z, float scaleX, float scaleY, float scaleZ, float rotation);
void drawUFO(void);
void drawFloor(void);
void drawTier(float baseRadius, float height, int segments, float grooveDepth);
void drawTower(void);
void drawLaser(float x, float y, float z, float laserHeight, float pulseSpeed);
void calculateNormal(float v0[3], float v1[3], float v2[3], float normal[3]);

// ---------------------------------------------------------
// Utility Functions
// ---------------------------------------------------------

// Render Stroke Font String with Custom Scaling
void renderStyledStrokeString(float x, float y, float size, const char* string) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f); // Position the text
    glScalef(size / 100.0f, size / 100.0f, 1.0f); // Scale the stroke font size
    while (*string) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *string);
        string++;
    }
    glPopMatrix();
}

// Render Bitmap String
void renderStyledBitmapString(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

// Render Instructions
void renderHUDInstructions(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f); // Bright white
    renderStyledStrokeString(x - size * 8, y, size, "Hold 'W' to move forward");
}

// Render Controls for the Left Side
void renderHUDControlsLeft(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f); // Bright white
    float spacing = size * 2.0f; // Line spacing multiplier

    renderStyledStrokeString(x, y + spacing * 3, size, "W - Move Forward");
    renderStyledStrokeString(x, y + spacing * 2, size, "S - Move Backward");
    renderStyledStrokeString(x, y + spacing * 1, size, "A - Strafe Left");
    renderStyledStrokeString(x, y, size, "D - Strafe Right");
}

// Render Controls for the Right Side
void renderHUDControlsRight(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f); // Bright white
    float spacing = size * 2.0f; // Line spacing multiplier

    renderStyledStrokeString(x, y + spacing * 3, size, "UP Arrow - Ascend");
    renderStyledStrokeString(x, y + spacing * 2, size, "DOWN Arrow - Descend");
    renderStyledStrokeString(x, y + spacing * 1, size, "LEFT Arrow - Rotate Left");
    renderStyledStrokeString(x, y, size, "RIGHT Arrow - Rotate Right");
}

void displayHUD(void) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // Convert fractional positions to screen coordinates
    float instructionsPosX = instructionsX * width;
    float instructionsPosY = instructionsY * height;
    float controlsLeftPosX = controlsLeftX * width;
    float controlsLeftPosY = controlsLeftY * height;
    float controlsRightPosX = controlsRightX * width;
    float controlsRightPosY = controlsRightY * height;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height); // Set orthogonal projection for HUD
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING); // Disable lighting for HUD rendering

    if (!displayControls) {
        // Convert fractional positions to screen coordinates
        float titlePosX = titleX * width;
        float titlePosY = titleY * height;
        
        // Render Title
        glColor3f(0.0f, 1.0f, 1.0f); // Neon cyan
        renderStyledStrokeString(titlePosX - titleSize, titlePosY, titleSize, "3D UFO ADVENTURE");
        renderStyledStrokeString(titlePosX - (titleSize + 2), titlePosY, titleSize, "3D UFO ADVENTURE");
        renderStyledStrokeString(titlePosX - (titleSize - 2), titlePosY, titleSize, "3D UFO ADVENTURE");
        
        // Render Instructions
        renderHUDInstructions(instructionsPosX, instructionsPosY, instructionsSize);
    } else {
        // Render Controls HUD
        renderHUDControlsLeft(controlsLeftPosX, controlsLeftPosY, controlsSize);
        renderHUDControlsRight(controlsRightPosX, controlsRightPosY, controlsSize);
    }

    glEnable(GL_LIGHTING); // Re-enable lighting
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
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

// ---------------------------------------------------------
// Scene Drawing Functions
// ---------------------------------------------------------

void drawMoon(float size, float orbitRadius, float orbitSpeed) {
    static float orbitAngle = 0.0f; // Keep track of the orbit angle

    // Update the orbit position
    orbitAngle += orbitSpeed;
    if (orbitAngle > 360.0f) {
        orbitAngle -= 360.0f; // Keep angle within [0, 360]
    }

    // Calculate the moon's position in orbit
    float moonX = orbitRadius * cos(orbitAngle * M_PI / 180.0f);
    float moonZ = orbitRadius * sin(orbitAngle * M_PI / 180.0f);
    float moonY = 200.0f; // Fixed height for the moon

    // Update light source position to match the moon's position
    GLfloat lightPos[] = {moonX, moonY, moonZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Set light properties for a warm white glow
    GLfloat lightAmbient[] = {0.1f, 0.1f, 0.15f, 1.0f}; // Soft ambient light
    GLfloat lightDiffuse[] = {0.9f, 0.85f, 0.8f, 1.0f}; // Warm diffuse light
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};  // White specular highlights
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Draw the moon with texture and warm emissive glow
    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ); // Move to the moon's position

    // Enable emissive glow for the moon
    GLfloat emissiveWarmWhite[] = {0.9f, 0.85f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveWarmWhite);

    // Enable the moon's texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lightTexture);
    glColor3f(1.0, 1.0, 1.0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Create the moon sphere
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, size, 40, 40); // Larger, more detailed moon
    gluDeleteQuadric(quad);

    // Disable texture and emissive properties after rendering
    glDisable(GL_TEXTURE_2D);
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);

    glPopMatrix();
}

void drawTier(float baseRadius, float height, int segments, float grooveDepth) {
    float angleStep = 360.0f / segments; // Angle between each segment
    float innerRadius = baseRadius - grooveDepth; // Radius for grooves

    GLfloat normal[3]; // To hold calculated normals

    // Bind the sci-fi texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sciFiTexture);

    // Draw the jagged vertical walls with texture mapping
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep * (M_PI / 180.0f); // Convert degrees to radians
        float xOuter = baseRadius * cos(angle);
        float zOuter = baseRadius * sin(angle);
        float xInner = innerRadius * cos(angle);
        float zInner = innerRadius * sin(angle);

        // Calculate normals for vertical wall
        float v0[] = {xOuter, 0.0f, zOuter};
        float v1[] = {xInner, 0.0f, zInner};
        float v2[] = {xOuter, height, zOuter};
        calculateNormal(v0, v1, v2, normal); // Calculate the normal for the surface
        glNormal3fv(normal);

        // Texture mapping for the outer vertex
        glTexCoord2f(1.0f * i / segments, 0.0f); // Texture coordinate for the bottom
        glVertex3f(xOuter, 0.0f, zOuter);       // Bottom outer

        glTexCoord2f(1.0f * i / segments, 1.0f); // Texture coordinate for the top
        glVertex3f(xOuter, height, zOuter);      // Top outer

        // Texture mapping for the inner vertex
        glTexCoord2f(1.0f * i / segments, 0.0f); // Texture coordinate for the bottom
        glVertex3f(xInner, 0.0f, zInner);       // Bottom inner

        glTexCoord2f(1.0f * i / segments, 1.0f); // Texture coordinate for the top
        glVertex3f(xInner, height, zInner);      // Top inner
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Draw the flat black top
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 0.0, 0.0); // Black color
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing upward
    glVertex3f(0.0f, height, 0.0f); // Center point of the top face

    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float xOuter = baseRadius * cos(angle);
        float zOuter = baseRadius * sin(angle);

        glVertex3f(xOuter, height, zOuter); // Outer edge of the top face
    }
    glEnd();

    // Draw the pink glowing edge around the top
    glBegin(GL_LINE_LOOP);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat[]){1.0f, 0.078f, 0.576f, 1.0f}); // Emissive pink glow
    glColor3f(1.0, 0.078, 0.576); // Pink color
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing upward

    for (int i = 0; i < segments; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float xOuter = baseRadius * cos(angle);
        float zOuter = baseRadius * sin(angle);

        glVertex3f(xOuter, height, zOuter); // Outer edge for the glow
    }
    glEnd();

    // Disable emissive property after use
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
}

void drawTower(void) {
    int numTiers = 30;          // Number of tiers in the tower
    float baseRadius = 5.0f;    // Radius of the bottom-most tier
    float height = 0.5f;        // Height of each tier
    float grooveDepth = 0.5f;   // Depth of grooves
    float scaleStep = 0.9f;     // Scaling factor for each tier
    float rotationStep = 10.0f; // Rotation increment per tier

    for (int i = 0; i < numTiers; i++) {
        glPushMatrix();

        // Stack tiers vertically
        glTranslatef(0.0f, i * height, 0.0f); // Offset each tier by its height

        // Scale and rotate for variation
        glScalef(pow(scaleStep, i), 1.0f, pow(scaleStep, i)); // Scale down progressively
        glRotatef(i * rotationStep, 0.0f, 1.0f, 0.0f);        // Rotate around Y-axis

        // Draw tier
        drawTier(baseRadius, height, 32, grooveDepth);

        glPopMatrix();
    }

    // Add the blue laser beam at the top of the tower
    float laserHeight = 500.0f;  // Length of the laser beam
    float pulseSpeed = 1.5f;    // Speed of pulsating effect
    drawLaser(0.0f, numTiers * height, 0.0f, laserHeight, pulseSpeed);
}

void drawLaser(float x, float y, float z, float laserHeight, float pulseSpeed) {
    static float time = 0.0f; // Time variable for pulsating effect
    time += 0.1f; // Increment time for pulsating effect

    // Sharp pulse effect using a custom sawtooth-like function
    float intensity = fmod(time * pulseSpeed, 1.0f); // Create a rising intensity (0 to 1)
    intensity = intensity < 0.5f ? intensity * 2.0f : (1.0f - intensity) * 2.0f; // Make it sharp

    // Add random flicker for instability
    float flicker = ((rand() % 10) / 100.0f) - 0.05f; // Small random offset (-0.05 to 0.05)
    intensity = fmax(0.0f, fmin(1.0f, intensity + flicker)); // Clamp to [0, 1]

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Laser color (project's blue) with intensity
    GLfloat laserColor[] = {0.0f, 0.5f, 1.0f, intensity * 0.8f}; // Blue with alpha for inner beam
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, laserColor);   // Emissive blue glow for the inner beam
    glColor4f(0.0f, 0.5f, 1.0f, intensity * 0.8f);              // Set color with alpha for inner beam

    // Draw the inner beam
    glBegin(GL_QUAD_STRIP);
    float laserRadius = 0.1f; // Radius of the inner beam
    int segments = 32;        // Smoothness of the circular beam
    float angleStep = 360.0f / segments;

    for (int i = 0; i <= segments; i++) {
        float angle1 = i * angleStep * (M_PI / 180.0f);

        // Bottom and top vertices of the inner beam
        float xOuter = x + laserRadius * cos(angle1);
        float zOuter = z + laserRadius * sin(angle1);

        // Bottom vertex
        glVertex3f(xOuter, y, zOuter);

        // Top vertex
        glVertex3f(xOuter, y + laserHeight, zOuter);
    }
    glEnd();

    // Draw the outer glow aura
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat[]){0.0f, 0.3f, 0.8f, intensity * 0.5f}); // Dimmer blue glow
    glColor4f(0.0f, 0.3f, 0.8f, intensity * 0.5f); // Slightly dimmer for aura

    glBegin(GL_QUAD_STRIP);
    float auraRadius = 0.2f; // Larger radius for the outer aura
    for (int i = 0; i <= segments; i++) {
        float angle1 = i * angleStep * (M_PI / 180.0f);

        // Bottom and top vertices of the outer glow
        float xOuter = x + auraRadius * cos(angle1);
        float zOuter = z + auraRadius * sin(angle1);

        // Bottom vertex
        glVertex3f(xOuter, y, zOuter);

        // Top vertex
        glVertex3f(xOuter, y + laserHeight, zOuter);
    }
    glEnd();

    // Disable emissive property and blending
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
    glDisable(GL_BLEND);
}

void drawAbstractBuilding(void) {
    GLfloat normal[3]; // For normal calculations
    float textureRepeat = 1.0f; // Controls how many times the texture repeats (adjust as needed)

    // Main body of the building
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);

    // Front slanted face
    float v0[] = {-0.5, 0.0, 0.5};
    float v1[] = {0.5, 0.0, 0.5};
    float v2[] = {0.3, 1.0, 0.3};
    float v3[] = {-0.3, 1.0, 0.3};
    calculateNormal(v0, v1, v2, normal);
    glNormal3fv(normal);
    glTexCoord2f(0.0, 0.0); glVertex3f(v0[0], v0[1], v0[2]);
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v1[0], v1[1], v1[2]);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(v2[0], v2[1], v2[2]);
    glTexCoord2f(0.0, textureRepeat); glVertex3f(v3[0], v3[1], v3[2]);

    // Back slanted face
    float v4[] = {-0.5, 0.0, -0.5};
    float v5[] = {0.5, 0.0, -0.5};
    float v6[] = {0.3, 1.0, -0.3};
    float v7[] = {-0.3, 1.0, -0.3};
    calculateNormal(v4, v5, v6, normal);
    glNormal3fv(normal);
    glTexCoord2f(0.0, 0.0); glVertex3f(v4[0], v4[1], v4[2]);
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v5[0], v5[1], v5[2]);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(v6[0], v6[1], v6[2]);
    glTexCoord2f(0.0, textureRepeat); glVertex3f(v7[0], v7[1], v7[2]);

    // Left slanted face
    float v8[] = {-0.5, 0.0, -0.5};
    float v9[] = {-0.5, 0.0, 0.5};
    float v10[] = {-0.3, 1.0, 0.3};
    float v11[] = {-0.3, 1.0, -0.3};
    calculateNormal(v8, v9, v10, normal);
    glNormal3fv(normal);
    glTexCoord2f(0.0, 0.0); glVertex3f(v8[0], v8[1], v8[2]);
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v9[0], v9[1], v9[2]);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(v10[0], v10[1], v10[2]);
    glTexCoord2f(0.0, textureRepeat); glVertex3f(v11[0], v11[1], v11[2]);

    // Right slanted face
    float v12[] = {0.5, 0.0, -0.5};
    float v13[] = {0.5, 0.0, 0.5};
    float v14[] = {0.3, 1.0, 0.3};
    float v15[] = {0.3, 1.0, -0.3};
    calculateNormal(v12, v13, v14, normal);
    glNormal3fv(normal);
    glTexCoord2f(0.0, 0.0); glVertex3f(v12[0], v12[1], v12[2]);
    glTexCoord2f(textureRepeat, 0.0); glVertex3f(v13[0], v13[1], v13[2]);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(v14[0], v14[1], v14[2]);
    glTexCoord2f(0.0, textureRepeat); glVertex3f(v15[0], v15[1], v15[2]);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Neon edges
    GLfloat emissiveBlue[] = {0.0f, 1.0f, 2.0f, 1.0f};
    GLfloat emissivePink[] = {1.0f, 0.078f, 0.576f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal for edges
    glColor3f(0.0, 1.0, 2.0);
    glVertex3f(-0.5, 0.0, 0.5); glVertex3f(-0.3, 1.0, 0.3);
    glVertex3f(-0.35, 1.05, 0.35); glVertex3f(-0.55, 0.0, 0.55);

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissivePink);
    glColor3f(1.0, 0.078, 0.576);
    glVertex3f(0.5, 0.0, -0.5); glVertex3f(0.3, 1.0, -0.3);
    glVertex3f(0.35, 1.05, -0.35); glVertex3f(0.55, 0.0, -0.55);
    glEnd();

    // Neon spikes
    glBegin(GL_TRIANGLES);

    // Neon spike 1
    float v26[] = {0.0, 1.5, 0.0};
    float v27[] = {-0.1, 1.0, 0.1};
    float v28[] = {0.1, 1.0, 0.1};
    calculateNormal(v26, v27, v28, normal);
    glNormal3fv(normal);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissivePink);
    glColor3f(1.0, 0.078, 0.576);
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(-0.1, 1.0, 0.1);
    glVertex3f(0.1, 1.0, 0.1);

    // Neon spike 2
    float v29[] = {0.0, 1.5, 0.0};
    float v30[] = {0.1, 1.0, -0.1};
    float v31[] = {-0.1, 1.0, -0.1};
    calculateNormal(v29, v30, v31, normal);
    glNormal3fv(normal);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);
    glColor3f(0.0, 1.0, 2.0);
    glVertex3f(0.0, 1.5, 0.0);
    glVertex3f(0.1, 1.0, -0.1);
    glVertex3f(-0.1, 1.0, -0.1);

    glEnd();

    // Reset emissive properties
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
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

void drawAlien(void) {
    // Save the current transformation state
    glPushMatrix();

    // Disable lighting for consistent coloring
    glDisable(GL_LIGHTING);

    // Set the alien's color to playful pink
    glColor3f(1.0f, 0.078f, 0.576f); // Bright pink color

    // Draw the head (a smaller sphere)
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);  // Position head above the body
    glutSolidSphere(0.08f, 20, 20);    // Smaller sphere for the head
    glPopMatrix();

    // Draw the body (an elongated ellipsoid)
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f); // Position the body below the head
    glScalef(0.5f, 0.7f, 0.5f);      // Scale for an ellipsoid shape
    glutSolidSphere(0.1f, 20, 20);   // Body sphere
    glPopMatrix();

    // Draw stubby arms (small spheres)
    glPushMatrix();
    glTranslatef(-0.1f, 0.1f, 0.0f); // Left arm
    glutSolidSphere(0.04f, 15, 15);  // Smaller sphere for the arm
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.1f, 0.1f, 0.0f); // Right arm
    glutSolidSphere(0.04f, 15, 15); // Smaller sphere for the arm
    glPopMatrix();

    // Draw stubby legs (small spheres)
    glPushMatrix();
    glTranslatef(-0.05f, -0.02f, 0.0f); // Left leg
    glutSolidSphere(0.04f, 15, 15);     // Smaller sphere for the leg
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.05f, -0.02f, 0.0f); // Right leg
    glutSolidSphere(0.04f, 15, 15);    // Smaller sphere for the leg
    glPopMatrix();

    // Draw antennae (thin cylinders connected vertically to the head)
    glPushMatrix();
    glTranslatef(-0.03f, 0.21f, 0.0f); // Left antenna base, adjusted to touch head
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Vertical orientation
    GLUquadric* leftAntenna = gluNewQuadric();
    gluQuadricNormals(leftAntenna, GLU_SMOOTH);
    gluCylinder(leftAntenna, 0.008f, 0.008f, 0.08f, 10, 5); // Thin cylinder for antenna
    gluDeleteQuadric(leftAntenna);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.03f, 0.21f, 0.0f); // Right antenna base, adjusted to touch head
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Vertical orientation
    GLUquadric* rightAntenna = gluNewQuadric();
    gluQuadricNormals(rightAntenna, GLU_SMOOTH);
    gluCylinder(rightAntenna, 0.008f, 0.008f, 0.08f, 10, 5); // Thin cylinder for antenna
    gluDeleteQuadric(rightAntenna);
    glPopMatrix();

    // Antenna tips
    glPushMatrix();
    glTranslatef(-0.03f, 0.29f, 0.0f); // Left antenna tip directly above the base
    glutSolidSphere(0.02f, 10, 10);    // Small sphere for tip
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.03f, 0.29f, 0.0f); // Right antenna tip directly above the base
    glutSolidSphere(0.02f, 10, 10);   // Small sphere for tip
    glPopMatrix();

    // Re-enable lighting
    glEnable(GL_LIGHTING);

    // Restore transformation state
    glPopMatrix();
}

void drawUFO(void) {
    // Enable 2D texturing and bind the UFO texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ufoTexture);

    // Set texture environment mode to modulate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Set texture wrapping mode to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set up material properties for metallic look
    GLfloat ambient[] = {0.1f, 0.1f, 0.2f, 1.0f};
    GLfloat diffuse[] = {0.2f, 0.4f, 0.8f, 1.0f};
    GLfloat specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat shininess = 128.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Emissive glow for force-field effect on base
    GLfloat baseEmission[] = {0.0f, 0.5f, 1.0f, 1.0f}; // Blue glow
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, baseEmission);

    // Draw the base of the UFO (spheroid)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, -0.3f, 0.0f);

    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (M_PI / 180.0f);
        float x = 1.5f * cos(angle);
        float z = 1.5f * sin(angle);
        float y = -0.1f;

        float v0[] = {0.0f, -0.3f, 0.0f};
        float v1[] = {x, y, z};
        float v2[] = {1.5f * cos((i + 10) * (M_PI / 180.0f)), -0.1f, 1.5f * sin((i + 10) * (M_PI / 180.0f))};
        GLfloat normal[3];
        calculateNormal(v0, v1, v2, normal);

        glNormal3fv(normal);
        glTexCoord2f(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
        glVertex3f(x, y, z);
    }
    glEnd();

    // Reset emissive properties
    GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up material properties for transparent glass
    GLfloat glassAmbient[] = {0.1f, 0.1f, 0.1f, 0.5f}; // Slightly tinted glass
    GLfloat glassDiffuse[] = {0.3f, 0.3f, 0.3f, 0.5f}; // Moderate transparency
    GLfloat glassSpecular[] = {0.9f, 0.9f, 0.9f, 0.5f}; // Bright specular for shine
    GLfloat glassShininess = 100.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glassAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glassDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, glassShininess);
    
    // Draw the alien inside the dome
    glPushMatrix();
    glTranslatef(0.0f, 0.01f, 0.0f); // Centered at the bottom of the dome
    drawAlien();
    glPopMatrix();

    // Set emissive properties for pink glow
    GLfloat emissivePink[] = {1.0f, 0.078f, 0.576f, 1.0f}; // Bright pink emissive glow
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissivePink);
    
    // Disable texturing to avoid inherited textures
    glDisable(GL_TEXTURE_2D);

    // Start drawing the dome
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.078f, 0.576f, 0.25f); // Pink with alpha for transparency
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.5f, 0.0f); // Tip of the dome

    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (M_PI / 180.0f);
        float x = 0.8f * cos(angle);
        float z = 0.8f * sin(angle);

        // Calculate normals
        float v0[] = {0.0f, 0.5f, 0.0f};
        float v1[] = {x, 0.0f, z};
        float v2[] = {0.8f * cos((i + 10) * (M_PI / 180.0f)), 0.0f, 0.8f * sin((i + 10) * (M_PI / 180.0f))};
        GLfloat normal[3];
        calculateNormal(v0, v1, v2, normal);

        // Set normal and texture coordinates
        glNormal3fv(normal);
        glTexCoord2f(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
        glVertex3f(x, 0.0f, z); // Vertex on the dome's base
    }
    glEnd();

    // Draw a pink ring at the base of the dome
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.078f, 0.576f); // Pink glow color
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (M_PI / 180.0f);
        float x = 0.8f * cos(angle);
        float z = 0.8f * sin(angle);
        glVertex3f(x, 0.01f, z); // Base of the dome
    }
    glEnd();

    // Disable emissive properties after drawing
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    // Disable blending
    glDisable(GL_BLEND);
    
    // Re-enable texturing for subsequent objects
    glEnable(GL_TEXTURE_2D);

    // Reset material properties
    GLfloat defaultAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat defaultDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat defaultSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defaultAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

void drawFloor(void) {
    float floorSize = 500.0f;  // Extra-large floor size

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 1.0f, 0.0f);

    // Adjusted texture coordinates for a larger floor
    glTexCoord2f(0.0, 0.0); glVertex3f(-floorSize, 0.0f, -floorSize);
    glTexCoord2f(10.0, 0.0); glVertex3f(floorSize, 0.0f, -floorSize);
    glTexCoord2f(10.0, 10.0); glVertex3f(floorSize, 0.0f, floorSize);
    glTexCoord2f(0.0, 10.0); glVertex3f(-floorSize, 0.0f, floorSize);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawSkyDome(float radius, GLuint textureID) {
    GLUquadric *quadric = gluNewQuadric(); // Create a quadric object
    gluQuadricTexture(quadric, GL_TRUE);   // Enable texturing on the quadric
    gluQuadricNormals(quadric, GLU_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the space texture

    // Set texture wrapping mode for repeating
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Scale the texture coordinates
    glMatrixMode(GL_TEXTURE); // Switch to texture matrix
    glPushMatrix();
    glLoadIdentity();
    glScalef(6.0f, 6.0f, 1.0f); // Scale texture (increase repetition)
    glMatrixMode(GL_MODELVIEW); // Switch back to modelview matrix

    // Invert the normals for inside viewing
    glPushMatrix();
    glScalef(-1.0f, 1.0f, 1.0f); // Invert X-axis to flip normals
    gluSphere(quadric, radius, 50, 50); // Draw the dome as a sphere
    glPopMatrix();

    // Reset the texture matrix
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadric); // Clean up the quadric object
}

void drawHexagonalBaseWithLattice(float radius, float height, int latticeDivisions) {
    int sides = 6; // Number of sides for the hexagon
    float angleStep = 360.0f / sides; // Angle between each vertex (in degrees)
    GLfloat normal[3]; // Normal vector for lighting calculations

    // Save OpenGL state
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Enable blending for glowing effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set emissive glow for the hexagonal base
    GLfloat emissiveDarkBlue[] = {0.0f, 0.0f, 1.0f, 1.0f}; // Dark blue glow
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveDarkBlue);

    // Draw the vertical faces with lattice cutouts
    glBegin(GL_QUADS);
    for (int i = 0; i < sides; i++) {
        float angle1 = i * angleStep * (M_PI / 180.0f);
        float angle2 = (i + 1) * angleStep * (M_PI / 180.0f);

        // Bottom and top corners of the face
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);

        // Divide each vertical face into smaller quads (lattice grid)
        for (int j = 0; j < latticeDivisions; j++) {
            float y1 = j * (height / latticeDivisions);           // Bottom of this subdivision
            float y2 = (j + 1) * (height / latticeDivisions);     // Top of this subdivision

            // Skip certain divisions to create "cutouts"
            if ((i + j) % 2 == 0) continue; // Creates a checkered lattice pattern

            // Normals for this quad
            float v0[] = {x1, y1, z1};
            float v1[] = {x2, y1, z2};
            float v2[] = {x1, y2, z1};
            calculateNormal(v0, v1, v2, normal);
            glNormal3fv(normal);

            // Draw the quad
            glColor4f(0.0f, 0.0f, 1.0f, 1.0f); // Dark blue color
            glVertex3f(x1, y1, z1); // Bottom-left corner
            glVertex3f(x2, y1, z2); // Bottom-right corner
            glVertex3f(x2, y2, z2); // Top-right corner
            glVertex3f(x1, y2, z1); // Top-left corner
        }
    }
    glEnd();

    // Draw the top face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal for the top face
    glVertex3f(0.0f, height, 0.0f); // Center of the top face
    for (int i = 0; i <= sides; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, height, z); // Outer edge of the top face
    }
    glEnd();

    // Disable emissive properties and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);

    // Restore OpenGL state
    glPopAttrib();
}

void drawVerticalLightBeams(float radius, float height) {
    int sides = 6; // Hexagon has 6 sides
    float angleStep = 360.0f / sides; // Angle between vertices in degrees

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set emissive blue color
    GLfloat emissiveBlue[] = {0.0f, 1.0f, 2.0f, 1.0f}; // Bright blue glow
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);

    glBegin(GL_QUADS);
    for (int i = 0; i < sides; i++) {
        float angle = i * angleStep * (M_PI / 180.0f); // Convert to radians
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Draw a vertical quad beam
        glVertex3f(x - 0.05f, 0.0f, z - 0.05f); // Bottom-left of the beam
        glVertex3f(x + 0.05f, 0.0f, z + 0.05f); // Bottom-right of the beam
        glVertex3f(x + 0.05f, height, z + 0.05f); // Top-right of the beam
        glVertex3f(x - 0.05f, height, z - 0.05f); // Top-left of the beam
    }
    glEnd();

    // Disable emissive lighting after use
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);

    // Disable blending
    glDisable(GL_BLEND);
}

void drawInnerPrism(float radius, float height) {
    int sides = 6; // Number of sides for the hexagon
    float angleStep = 360.0f / sides; // Angle between each vertex (in degrees)
    GLfloat normal[3]; // Normal vector for lighting calculations

    // Draw the vertical walls of the prism
    glBegin(GL_QUADS);
    for (int i = 0; i < sides; i++) {
        float angle1 = i * angleStep * (M_PI / 180.0f);
        float angle2 = (i + 1) * angleStep * (M_PI / 180.0f);

        // Bottom and top corners of the face
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);

        // Normals for this face
        float v0[] = {x1, 0.0f, z1};
        float v1[] = {x2, 0.0f, z2};
        float v2[] = {x1, height, z1};
        calculateNormal(v0, v1, v2, normal);
        glNormal3fv(normal);

        // Draw the quad
        glColor3f(0.0f, 0.0f, 0.0f); // Black color for the solid prism
        glVertex3f(x1, 0.0f, z1); // Bottom-left corner
        glVertex3f(x2, 0.0f, z2); // Bottom-right corner
        glVertex3f(x2, height, z2); // Top-right corner
        glVertex3f(x1, height, z1); // Top-left corner
    }
    glEnd();

    // Draw the top face of the prism
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the solid prism
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing upward
    glVertex3f(0.0f, height, 0.0f); // Center of the top face
    for (int i = 0; i <= sides; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, height, z); // Outer edge of the top face
    }
    glEnd();

    // Draw the bottom face of the prism
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the solid prism
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal pointing downward
    glVertex3f(0.0f, 0.0f, 0.0f); // Center of the bottom face
    for (int i = 0; i <= sides; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, 0.0f, z); // Outer edge of the bottom face
    }
    glEnd();
}

void drawHelicalHourglass(float baseRadius, float topRadius, float height, int twists, int segments) {
    float angleStep = 360.0f / segments; // Angle step for creating smooth curves
    float heightStep = height / (segments * twists); // Height increment for each step
    float radiusStep = (baseRadius - topRadius) / (segments * twists / 2); // Radius change per step
    GLfloat emissiveBlue[] = {0.0f, 1.0f, 2.0f, 1.0f}; // Emissive blue for the beams

    // Enable emissive properties and blending for glow effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);

    glBegin(GL_QUADS);
    for (int v = 0; v < 6; v++) { // Loop over the six vertices
        float startAngle = v * (360.0f / 6) * (M_PI / 180.0f); // Starting angle for each vertex
        float currentRadius = baseRadius; // Initialize radius

        for (int i = 0; i < segments * twists; i++) {
            float angle1 = startAngle + i * angleStep * (M_PI / 180.0f); // Current angle
            float angle2 = startAngle + (i + 1) * angleStep * (M_PI / 180.0f); // Next angle
            float y1 = i * heightStep; // Current height
            float y2 = (i + 1) * heightStep; // Next height

            // Update the radius for the hourglass narrowing
            if (i < segments * twists / 2) {
                currentRadius -= radiusStep; // Narrowing phase
            } else {
                currentRadius += radiusStep; // Widening phase
            }

            // Compute vertices for the beam quad
            float x1 = currentRadius * cos(angle1);
            float z1 = currentRadius * sin(angle1);
            float x2 = currentRadius * cos(angle2);
            float z2 = currentRadius * sin(angle2);

            // Create pulsating energy effect based on height, moving downward
            float pulseEffect = 0.5f + 0.5f * sin((y1 - glutGet(GLUT_ELAPSED_TIME) * 0.003f)); // Reverse direction
            float alphaEffect = 1.8f + 0.2f * cos((y1 - glutGet(GLUT_ELAPSED_TIME) * 0.002f)); // Smooth alpha oscillation

            // Set dynamic color for the quad
            glColor4f(0.0f, 1.0f, 2.0f, alphaEffect * pulseEffect); // Brightness and transparency modulation

            // Draw the quad representing the beam
            glVertex3f(x1 - 0.05f, y1, z1 - 0.05f); // Bottom-left corner
            glVertex3f(x2 - 0.05f, y2, z2 - 0.05f); // Top-left corner
            glVertex3f(x2 + 0.05f, y2, z2 + 0.05f); // Top-right corner
            glVertex3f(x1 + 0.05f, y1, z1 + 0.05f); // Bottom-right corner
        }
    }
    glEnd();

    // Disable emissive properties and blending after rendering
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);
}

void drawTorusPlatform(float outerRadius, float innerRadius, float height, int radialSegments, int tubularSegments) {
    GLfloat normal[3]; // Normal vector for lighting calculations
    float flowOffset = 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.002f); // Dynamic upward offset

    // Save OpenGL state
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Set emissive dark blue glow
    GLfloat emissiveDarkBlue[] = {0.0f, 0.0f, 1.0f, 1.0f}; // Dark blue emissive light
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveDarkBlue);

    // Loop to create the toroidal surface
    glBegin(GL_QUADS);
    for (int i = 0; i < radialSegments; i++) {
        float theta1 = i * (2.0f * M_PI / radialSegments);     // Current angle
        float theta2 = (i + 1) * (2.0f * M_PI / radialSegments); // Next angle

        for (int j = 0; j < tubularSegments; j++) {
            float phi1 = j * (2.0f * M_PI / tubularSegments);     // Current tubular angle
            float phi2 = (j + 1) * (2.0f * M_PI / tubularSegments); // Next tubular angle

            // Vertices of the torus, displaced by height
            float x1 = (outerRadius + innerRadius * cos(phi1)) * cos(theta1);
            float z1 = (outerRadius + innerRadius * cos(phi1)) * sin(theta1);
            float y1 = innerRadius * sin(phi1) + flowOffset + height;

            float x2 = (outerRadius + innerRadius * cos(phi2)) * cos(theta1);
            float z2 = (outerRadius + innerRadius * cos(phi2)) * sin(theta1);
            float y2 = innerRadius * sin(phi2) + flowOffset + height;

            float x3 = (outerRadius + innerRadius * cos(phi2)) * cos(theta2);
            float z3 = (outerRadius + innerRadius * cos(phi2)) * sin(theta2);
            float y3 = innerRadius * sin(phi2) + flowOffset + height;

            float x4 = (outerRadius + innerRadius * cos(phi1)) * cos(theta2);
            float z4 = (outerRadius + innerRadius * cos(phi1)) * sin(theta2);
            float y4 = innerRadius * sin(phi1) + flowOffset + height;

            // Calculate normal for this quad
            float v0[] = {x1, y1, z1};
            float v1[] = {x2, y2, z2};
            float v2[] = {x4, y4, z4};
            calculateNormal(v0, v1, v2, normal);
            glNormal3fv(normal);

            // Draw the quad with slight transparency
            glColor3f(0.0f, 0.0f, 1.0f); // Dark blue with slight transparency
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
            glVertex3f(x4, y4, z4);
        }
    }
    glEnd();

    // Disable emissive properties and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);

    // Restore OpenGL state
    glPopAttrib();
}

void drawTorusPlatforms(void) {
    float outerRadius = 4.5f; // Radius larger than the hexagonal base
    float innerRadius = 1.0f; // Thickness of the rounded edge
    float height = 0.8f;      // Vertical displacement for flow effect
    int radialSegments = 64;  // Smoothness of the outer edge
    int tubularSegments = 32; // Smoothness of the inner edge

    // Left base platform
    glPushMatrix();
    glTranslatef(-10.0f, 12.5f, -35.0f); // Position slightly below helix
    drawTorusPlatform(outerRadius, innerRadius, height, radialSegments, tubularSegments);
    glPopMatrix();

    // Right base platform
    glPushMatrix();
    glTranslatef(10.0f, 12.5f, -35.0f); // Position slightly below helix
    drawTorusPlatform(outerRadius, innerRadius, height, radialSegments, tubularSegments);
    glPopMatrix();

    // Back base platform
    glPushMatrix();
    glTranslatef(0.0f, 12.5f, -20.0f); // Position slightly below helix
    drawTorusPlatform(outerRadius, innerRadius, height, radialSegments, tubularSegments);
    glPopMatrix();
}

void drawFogParticle(float x, float y, float z, float size, float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glowing effect
    GLfloat emissiveColor[] = {0.0f, 0.8f, 1.0f, alpha}; // Emissive blue with alpha
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);

    glColor4f(0.0f, 0.8f, 1.0f, alpha); // Set particle color and alpha
    glBegin(GL_QUADS);

    // Create a billboarded square (always facing the camera)
    glVertex3f(x - size, y - size, z); // Bottom-left
    glVertex3f(x + size, y - size, z); // Bottom-right
    glVertex3f(x + size, y + size, z); // Top-right
    glVertex3f(x - size, y + size, z); // Top-left

    glEnd();

    // Disable emissive properties and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);
}

void drawFogField(float baseX, float baseY, float baseZ, int particleCount) {
    static float time = 0.0f; // Time variable for animation
    time += 0.01f;           // Increment time for animation

    for (int i = 0; i < particleCount; i++) {
        // Randomize initial positions and movement
        float angle = rand() % 360 * (M_PI / 180.0f); // Random angle around the platform
        float distance = (rand() % 100 / 100.0f) * 5.0f; // Random radius within the platform
        float x = baseX + cos(angle) * distance;
        float z = baseZ + sin(angle) * distance;
        float y = baseY + (rand() % 100 / 100.0f) * 3.0f; // Random height near the platform

        // Simulate upward flow
        float upwardFlow = sin(time + i * 0.1f) * 0.5f;

        // Particle size and alpha fade
        float size = 0.1f + 0.2f * (rand() % 100 / 100.0f); // Slight size variation
        float alpha = 0.2f + 0.5f * (1.0f - (y / (baseY + 10.0f))); // Fade out as it rises

        drawFogParticle(x, y + upwardFlow, z, size, alpha);
    }
}

void drawFogFields(void) {
    int particleCount = 300; // Number of particles per platform

    // Fog field for left platform
    drawFogField(-10.0f, 12.0f, -35.0f, particleCount);

    // Fog field for right platform
    drawFogField(10.0f, 12.0f, -35.0f, particleCount);

    // Fog field for back platform
    drawFogField(0.0f, 12.0f, -20.0f, particleCount);
}

void drawGlowingTriangularBase(float baseHeight) {
    GLfloat normal[3]; // For normal calculations
    GLfloat emissiveBlue[] = {0.0f, 1.0f, 2.0f, 1.0f}; // Bright blue emissive glow

    // Larger coordinates of the triangle vertices
    float vertex1[] = {-17.0f, baseHeight, -34.0f}; // Left fog position
    float vertex2[] = {17.0f, baseHeight, -34.0f};  // Right fog position
    float vertex3[] = {0.0f, baseHeight, -7.0f};   // Back fog position

    // Center of the triangle (for edge glow calculations)
    float center[] = {0.0f, baseHeight - 0.5f, -27.0f};

    // Draw the triangle base
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.5f, 1.0f); // Solid blue for the triangle face

    // First face
    calculateNormal(center, vertex1, vertex2, normal);
    glNormal3fv(normal);
    glVertex3fv(vertex1);
    glVertex3fv(vertex2);
    glVertex3fv(center);

    // Second face
    calculateNormal(center, vertex2, vertex3, normal);
    glNormal3fv(normal);
    glVertex3fv(vertex2);
    glVertex3fv(vertex3);
    glVertex3fv(center);

    // Third face
    calculateNormal(center, vertex3, vertex1, normal);
    glNormal3fv(normal);
    glVertex3fv(vertex3);
    glVertex3fv(vertex1);
    glVertex3fv(center);
    glEnd();

    // Draw the glowing edges
    glEnable(GL_BLEND); // Enable blending for the glow
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);

    glLineWidth(8.0f); // Make the edge thick
    glBegin(GL_LINE_LOOP);
    glColor4f(0.0f, 1.0f, 2.0f, 1.0f); // Bright blue glow
    glVertex3fv(vertex1);
    glVertex3fv(vertex2);
    glVertex3fv(vertex3);
    glEnd();
    glLineWidth(1.0f); // Reset line width

    // Disable emissive and blending after use
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);
}

void drawArchedColumn(float baseRadius, float height, int segments) {
    GLfloat normal[3]; // For normal calculations
    float angleStep = 360.0f / segments; // Angle step for smooth curve

    // Save current OpenGL states
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Enable blending for neon glow effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set the emissive blue glow
    GLfloat emissiveBlue[] = {0.0f, 1.0f, 1.0f, 1.0f}; // Strong blue emissive light
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBlue);

    // Draw the cylindrical column
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep * (M_PI / 180.0f);
        float x = baseRadius * cos(angle);
        float z = baseRadius * sin(angle);

        // Calculate normals
        float v0[] = {x, 0.0f, z};
        float v1[] = {x, height, z};
        float v2[] = {x, height, z + 0.01f}; // Small offset for normal calculation
        calculateNormal(v0, v1, v2, normal);
        glNormal3fv(normal);

        // Draw the sides of the column
        glColor4f(0.0f, 1.0f, 1.0f, 1.0f); // Bright blue color with alpha for glow
        glVertex3f(x, 0.0f, z);      // Bottom vertex
        glVertex3f(x, height, z);   // Top vertex
    }
    glEnd();

    // Add decorative arched top
    glBegin(GL_LINE_STRIP);
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f); // Bright cyan for the arch
    for (int i = 0; i <= 180; i++) { // Semi-circle for the arch
        float angle = i * (M_PI / 180.0f);
        float x = baseRadius * cos(angle);
        float y = height + baseRadius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    // Restore OpenGL states
    glDisable(GL_BLEND);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f}); // Reset emissive property
    glPopAttrib();
}

void drawTessellatedWall(float width, float height, int rows, int columns) {
    GLfloat normal[3]; // For normal calculations
    float dx = width / columns; // Width of each quad
    float dy = height / rows;  // Height of each quad

    // Save current OpenGL states
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Enable blending for the glowing effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set emissive purple glow for the wall
    GLfloat emissivePurple[] = {0.5f, 0.0f, 0.5f, 1.0f}; // Purple emissive light
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissivePurple);

    glBegin(GL_QUADS);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // Calculate vertices
            float x1 = j * dx - width / 2.0f;
            float x2 = (j + 1) * dx - width / 2.0f;
            float y1 = i * dy;
            float y2 = (i + 1) * dy;

            // Skip some quads to create gothic cutouts
            if ((i + j) % 3 == 0) continue;

            // Calculate normals
            float v0[] = {x1, y1, 0.0f};
            float v1[] = {x2, y1, 0.0f};
            float v2[] = {x1, y2, 0.0f};
            calculateNormal(v0, v1, v2, normal);
            glNormal3fv(normal);

            // Draw the quad
            glColor4f(0.5f, 0.0f, 0.5f, 1.0f); // Purple with glow effect
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x2, y1, 0.0f);
            glVertex3f(x2, y2, 0.0f);
            glVertex3f(x1, y2, 0.0f);
        }
    }
    glEnd();

    // Disable emissive properties and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);

    // Restore OpenGL states
    glPopAttrib();
}

void drawNeoGothicSpire(float baseRadius, float height, int segments) {
    GLfloat normal[3]; // For normal calculations

    // Save current OpenGL states
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Enable blending for glowing effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set emissive light for the spire body
    GLfloat emissiveBody[] = {0.5f, 0.5f, 0.8f, 1.0f}; // Metallic blue emissive light
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveBody);

    // Draw the tapered spire
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.5f, 0.5f, 0.8f, 1.0f); // Metallic blue
    glVertex3f(0.0f, height, 0.0f); // Tip of the spire

    for (int i = 0; i <= segments; i++) {
        float angle = i * (360.0f / segments) * (M_PI / 180.0f);
        float x = baseRadius * cos(angle);
        float z = baseRadius * sin(angle);

        float v0[] = {x, 0.0f, z};
        float v1[] = {0.0f, height, 0.0f};
        calculateNormal(v0, v1, v1, normal); // Simplified normal calculation
        glNormal3fv(normal);

        glVertex3f(x, 0.0f, z); // Base circle
    }
    glEnd();

    // Add enlarged glowing tip
    GLfloat emissiveTip[] = {0.2f, 0.6f, 1.0f, 1.0f}; // Light blue glow for the tip
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveTip);
    glColor4f(0.2f, 0.6f, 1.0f, 1.0f); // Light blue for tip glow

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i += 20) { // Circular glow for the tip
        float angle = i * (M_PI / 180.0f);
        float x = 0.2f * cos(angle); // Larger radius for glowing tip
        float z = 0.2f * sin(angle);
        glVertex3f(x, height, z); // Tip glow vertices
    }
    glEnd();

    // Disable emissive properties and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);

    // Restore OpenGL states
    glPopAttrib();
}

void drawArchedButtress(float startX, float startY, float startZ,
                        float endX, float endY, float endZ,
                        float archHeight, int segments) {
    GLfloat emissiveGlow[] = {0.0f, 0.8f, 1.0f, 1.0f}; // Blue glow for the edge

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveGlow);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments; // Parameter for interpolation (0 to 1)
        float x = (1 - t) * startX + t * endX; // Linear interpolation for X
        float z = (1 - t) * startZ + t * endZ; // Linear interpolation for Z
        float y = startY + archHeight * sin(t * M_PI); // Create arch curve for Y

        glVertex3f(x, y, z); // Point on the arch
    }
    glEnd();

    // Disable emissive and blending
    GLfloat noEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmissive);
    glDisable(GL_BLEND);
}

void drawDenseNeonTunnel(float startZ, float endZ, float radius, int segments, int rings, float baseX, float baseY) {
    float angleStep = 360.0f / segments; // Angle step for each segment of the ring
    float ringSpacing = (endZ - startZ) / rings; // Distance between each ring

    static float motionOffset = 0.0f; // To animate forward motion
    motionOffset += 0.01f; // Increment for smooth forward motion

    // Enable emissive lighting for the tunnel
    glPushAttrib(GL_LIGHTING_BIT); // Save current lighting state
    glEnable(GL_LIGHTING);

    for (int i = 0; i < rings; i++) {
        float z = startZ + i * ringSpacing; // Z-coordinate of the current ring

        // Draw the ring
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < segments; j++) {
            float angle = j * angleStep * (M_PI / 180.0f); // Current angle in radians
            float x = baseX + radius * cos(angle); // Apply baseX offset
            float y = baseY + radius * sin(angle); // Apply baseY offset

            GLfloat emissiveColor[4];

            // Dynamic emissive colors for forward motion
            if (j % 3 == 0) {
                emissiveColor[0] = fabs(sin(motionOffset + z * 0.02f)); // Blue
                emissiveColor[1] = 0.5f;
                emissiveColor[2] = 1.0f;
                emissiveColor[3] = 1.0f; // Alpha
            } else if (j % 3 == 1) {
                emissiveColor[0] = 1.0f; // Pink
                emissiveColor[1] = 0.078f;
                emissiveColor[2] = fabs(cos(motionOffset + z * 0.02f));
                emissiveColor[3] = 1.0f; // Alpha
            } else {
                emissiveColor[0] = 0.0f; // Green
                emissiveColor[1] = fabs(sin(motionOffset + z * 0.02f));
                emissiveColor[2] = 0.0f;
                emissiveColor[3] = 1.0f; // Alpha
            }

            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor); // Set emissive property
            glColor3f(emissiveColor[0], emissiveColor[1], emissiveColor[2]);

            glVertex3f(x, y, z); // Position the vertex
        }
        glEnd();

        // Add vertical connecting lines for density
        if (i > 0) {
            float prevZ = z - ringSpacing;

            glBegin(GL_LINES);
            for (int j = 0; j < segments; j++) {
                float angle = j * angleStep * (M_PI / 180.0f);
                float xCurrent = baseX + radius * cos(angle); // Apply baseX offset
                float yCurrent = baseY + radius * sin(angle); // Apply baseY offset

                float xPrev = baseX + radius * cos(angle); // Apply baseX offset
                float yPrev = baseY + radius * sin(angle); // Apply baseY offset

                glVertex3f(xCurrent, yCurrent, z);
                glVertex3f(xPrev, yPrev, prevZ);
            }
            glEnd();
        }
    }

    // Restore previous lighting state
    glPopAttrib();
}

void drawClockTowerBase(float width, float height, float depth, float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z); // Position the tower base

    // Save the current lighting state
    glPushAttrib(GL_LIGHTING_BIT);

    // Enable texturing for the base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, clockTowerTexture); // Bind the texture

    glColor3f(1.0f, 1.0f, 1.0f); // Set white base color for neutral texture

    glBegin(GL_QUADS);

    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);

    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, -depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, -depth / 2);

    // Left Face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, -depth / 2);

    // Right Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(width / 2, height, -depth / 2);

    // Top Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, height, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, height, -depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    
    // Restore previous lighting state
    glPopAttrib();
    glPopMatrix();
}

void drawVerticalPillars(float baseWidth, float baseHeight, float baseDepth, float pillarRadius, float baseX, float baseY, float baseZ) {
    float halfWidth = baseWidth / 2.0f;
    float halfDepth = baseDepth / 2.0f;

    // Emissive darker blue glow properties
    GLfloat pillarEmission[] = {0.0f, 0.1f, 0.3f, 1.0f}; // Darker neon blue emissive
    GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};      // Reset emission

    // Enable blending for glow effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for intense glow

    // Set the material emission for the pillars
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pillarEmission);

    // Draw four vertical pillars at each corner
    GLUquadric* pillar = gluNewQuadric();
    for (int i = 0; i < 4; i++) {
        glPushMatrix();

        // Determine pillar position based on corner index
        float x = baseX + (i % 2 == 0 ? -halfWidth : halfWidth);
        float z = baseZ + (i < 2 ? -halfDepth : halfDepth);

        // Position and align the pillar vertically
        glTranslatef(x, baseY, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Align cylinder vertically
        gluQuadricNormals(pillar, GLU_SMOOTH);

        // Draw the pillar with the emissive darker blue glow
        glColor4f(0.0f, 0.2f, 0.6f, 0.8f); // Darker blue with slight transparency for blending
        gluCylinder(pillar, pillarRadius * 0.5f, pillarRadius * 0.5f, baseHeight, 20, 5);

        glPopMatrix();
    }

    // Delete the quadric object
    gluDeleteQuadric(pillar);

    // Disable blending and reset emissive properties
    glDisable(GL_BLEND);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

void drawHorizontalBeams(float baseWidth, float baseHeight, float baseDepth, float beamRadius, int beamCount, float baseX, float baseY, float baseZ) {
    float halfWidth = baseWidth / 2.0f;
    float halfDepth = baseDepth / 2.0f;

    // Small offset to avoid z-fighting
    float offset = 0.02f;

    // Save current OpenGL states
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Disable lighting to ensure the beams emit their own light
    glDisable(GL_LIGHTING);

    // Enable blending for neon glow effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set constant emissive pink glow
    glColor4f(1.0f, 0.078f, 0.576f, 1.0f); // Hot pink with full glow effect

    GLUquadric* beam = gluNewQuadric();

    // Determine vertical spacing for the beams, starting at the bottom
    float spacing = baseHeight / (beamCount - 1);

    // Draw beams connecting the pillars on all four faces
    for (int i = 0; i < beamCount; i++) {
        float y = baseY + spacing * i; // Position each beam evenly spaced along height

        // Front Face (connect left and right pillars)
        glPushMatrix();
        glTranslatef(baseX - halfWidth + offset, y, baseZ + halfDepth + offset); // Slight offset for anti z-fighting
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Align cylinder horizontally
        gluCylinder(beam, beamRadius, beamRadius, baseWidth - 2 * offset, 20, 5); // Slightly shorter length
        glPopMatrix();

        // Back Face (connect left and right pillars)
        glPushMatrix();
        glTranslatef(baseX - halfWidth + offset, y, baseZ - halfDepth - offset); // Slight offset for anti z-fighting
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Align cylinder horizontally
        gluCylinder(beam, beamRadius, beamRadius, baseWidth - 2 * offset, 20, 5); // Slightly shorter length
        glPopMatrix();

        // Left Face (connect front and back pillars)
        glPushMatrix();
        glTranslatef(baseX - halfWidth - offset, y, baseZ - halfDepth + offset); // Slight offset for anti z-fighting
        gluCylinder(beam, beamRadius, beamRadius, baseDepth - 2 * offset, 20, 5); // Slightly shorter length
        glPopMatrix();

        // Right Face (connect front and back pillars)
        glPushMatrix();
        glTranslatef(baseX + halfWidth + offset, y, baseZ - halfDepth + offset); // Slight offset for anti z-fighting
        gluCylinder(beam, beamRadius, beamRadius, baseDepth - 2 * offset, 20, 5); // Slightly shorter length
        glPopMatrix();
    }

    gluDeleteQuadric(beam);

    // Restore OpenGL states
    glDisable(GL_BLEND);
    glPopAttrib();
}

void drawFuturisticClock(float radius, float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z); // Position the clock

    // Save the current OpenGL state
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Disable default lighting for the neon effect
    glDisable(GL_LIGHTING);

    // Enable blending for the glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // **Clock Frame**: Neon green glowing circle
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // Bright green glow
    GLUquadric* frame = gluNewQuadric();
    gluQuadricNormals(frame, GLU_SMOOTH);
    gluDisk(frame, radius * 0.9f, radius, 50, 1); // Outer ring of the clock
    gluDeleteQuadric(frame);

    // **Tick Marks**: Short green lines for hours
    glColor4f(0.0f, 0.8f, 0.0f, 1.0f); // Slightly brighter ticks
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
        glRotatef(i * 30.0f, 0.0f, 0.0f, 1.0f); // Rotate for each tick
        glBegin(GL_LINES);
        glVertex2f(0.0f, radius * 0.7f); // Start inside the frame
        glVertex2f(0.0f, radius * 0.85f); // End near the frame
        glEnd();
        glPopMatrix();
    }

    // **Clock Hands**: Dynamic rotation for minute and hour hands
    float hourAngle = (glutGet(GLUT_ELAPSED_TIME) / 64000.0f) * 360.0f; // Slow rotation for hours
    float minuteAngle = (glutGet(GLUT_ELAPSED_TIME) / 24000.0f) * 360.0f; // Faster rotation for minutes

    // Hour Hand
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // Bright green for hands
    glPushMatrix();
    glRotatef(- hourAngle, 0.0f, 0.0f, 1.0f); // Rotate hour hand
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, radius * 0.5f); // Shorter for hours
    glEnd();
    glPopMatrix();

    // Minute Hand
    glPushMatrix();
    glRotatef(- minuteAngle, 0.0f, 0.0f, 1.0f); // Rotate minute hand
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, radius * 0.75f); // Longer for minutes
    glEnd();
    glPopMatrix();

    // Restore OpenGL states
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopAttrib();

    glPopMatrix();
}

void drawTopPinkBeam(float baseWidth, float baseDepth, float yPosition, float beamRadius, float baseX, float baseZ) {
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 0.078f, 0.576f, 1.0f); // Neon pink color
    GLUquadric* beam = gluNewQuadric();

    float halfWidth = baseWidth / 2.0f;
    float halfDepth = baseDepth / 2.0f;

    // Front beam
    glPushMatrix();
    glTranslatef(baseX - halfDepth, yPosition, baseZ + halfDepth); // Match front face
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(beam, beamRadius, beamRadius, baseWidth, 20, 5);
    glPopMatrix();

    // Back beam
    glPushMatrix();
    glTranslatef(baseX - halfDepth, yPosition, baseZ - halfDepth); // Match back face
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(beam, beamRadius, beamRadius, baseWidth, 20, 5);
    glPopMatrix();

    // Left beam
    glPushMatrix();
    glTranslatef(baseX - halfWidth, yPosition, baseZ - halfDepth); // Match left face
    gluCylinder(beam, beamRadius, beamRadius, baseDepth, 20, 5);
    glPopMatrix();

    // Right beam
    glPushMatrix();
    glTranslatef(baseX + halfWidth, yPosition, baseZ - halfDepth); // Match right face
    gluCylinder(beam, beamRadius, beamRadius, baseDepth, 20, 5);
    glPopMatrix();

    gluDeleteQuadric(beam);
    glDisable(GL_BLEND);
    glPopAttrib();
}

void drawBlueCornerBeamsPair1(float baseWidth, float baseDepth, float baseHeight, float beamRadius, float baseX, float baseY, float baseZ) {
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(0.0f, 0.1f, 0.3f, 1.0f); // Neon blue color
    GLUquadric* beam = gluNewQuadric();

    float halfWidth = baseWidth / 2.0f;
    float halfDepth = baseDepth / 2.0f;
    float topY = baseY + baseHeight; // Top of the base
    float centerX = baseX;
    float centerY = topY + 10.0f;   // Central convergence point above the base
    float centerZ = baseZ;

    // First diagonal pair
    for (int i = 0; i < 2; i++) {
        glPushMatrix();

        // Determine corner position
        float x = baseX + (i == 0 ? -halfWidth : halfWidth);
        float z = baseZ + (i == 0 ? -halfDepth : halfDepth);

        glTranslatef(x, topY, z);
        float dx = centerX - x;
        float dy = centerY - topY;
        float dz = centerZ - z;
        float length = sqrt(dx * dx + dy * dy + dz * dz);

        // Calculate rotation
        float yaw = atan2(dz, dx) * 180.0f / M_PI;
        float pitch = -atan2(dy, sqrt(dx * dx + dz * dz)) * 180.0f / M_PI;

        glRotatef(yaw, 0.0f, 1.0f, 0.0f);  // Rotate horizontally toward center
        glRotatef(pitch, 1.0f, 0.0f, 0.0f); // Rotate vertically toward center
        gluCylinder(beam, beamRadius * 1.5f, beamRadius * 1.5f, length, 20, 5);

        glPopMatrix();
    }

    gluDeleteQuadric(beam);
    glDisable(GL_BLEND);
    glPopAttrib();
}

void drawBlueCornerBeamsPair2(float baseWidth, float baseDepth, float baseHeight, float beamRadius, float baseX, float baseY, float baseZ) {
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(0.0f, 0.1f, 0.3f, 1.0f); // Neon blue color
    GLUquadric* beam = gluNewQuadric();

    float halfWidth = baseWidth / 2.0f;
    float halfDepth = baseDepth / 2.0f;
    float topY = baseY + baseHeight; // Top of the base
    float centerX = baseX;
    float centerY = topY + 10.0f;   // Central convergence point above the base
    float centerZ = baseZ;

    // Second diagonal pair
    for (int i = 0; i < 2; i++) {
        glPushMatrix();

        // Determine corner position
        float x = baseX + (i == 0 ? -halfWidth : halfWidth);
        float z = baseZ + (i == 0 ? halfDepth : -halfDepth);

        glTranslatef(x, topY, z);
        float dx = centerX - x;
        float dy = centerY - topY;
        float dz = centerZ - z;
        float length = sqrt(dx * dx + dy * dy + dz * dz);

        // Calculate rotation
        float yaw = atan2(dz, dx) * 180.0f / M_PI;
        float pitch = -atan2(dy, sqrt(dx * dx + dz * dz)) * 410.0f / M_PI;

        glRotatef(yaw, 0.0f, 1.0f, 0.0f);  // Rotate horizontally toward center
        glRotatef(pitch, 1.0f, 0.0f, 0.0f); // Rotate vertically toward center
        gluCylinder(beam, beamRadius * 1.5f, beamRadius * 1.5f, length, 20, 5);

        glPopMatrix();
    }

    gluDeleteQuadric(beam);
    glDisable(GL_BLEND);
    glPopAttrib();
}

void drawFerrisWheelBase(float width, float height, float depth, float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z); // Position the base at the specified location

    // Save the current lighting state
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Enable texturing for the base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, clockTowerTexture); // Reuse the same texture as the tower base

    // Set material properties for a metallic, shiny effect
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};   // Moderate ambient reflection
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};   // High diffuse reflection
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};  // Bright white specular reflection
    GLfloat shininess = 128.0f;                     // High shininess for a metallic effect

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Ensure lighting interacts with the texture
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Set base color to white for neutral texture display
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);

    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, -depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, -depth / 2);

    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, -depth / 2);

    // Right Face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, 0.0f, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(width / 2, height, -depth / 2);

    // Top Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, height, -depth / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, height, -depth / 2);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(width / 2, height, depth / 2);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-width / 2, height, depth / 2);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Restore previous lighting state
    glPopAttrib();

    // Draw Vertical Beams
    drawVerticalPillars(width, height, depth, 0.5f, 0, 0, 0);

    // Draw Horizontal Beams
    drawHorizontalBeams(width, height, depth, 0.1f, 2, 0, 0, 0);

    glPopMatrix();
}

void drawFerrisWheelWheel(float radius, float thickness, float x, float y, float z, float angle) {
    glPushMatrix();
    glTranslatef(x, y, z); // Position the wheel at the specified location
    glRotatef(angle, 0.0f, 0.0f, 1.0f); // Apply rotation for movement

    // Save the current OpenGL state
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Disable default lighting for the neon effect
    glDisable(GL_LIGHTING);

    // Enable blending for the neon glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    GLUquadric* quadric = gluNewQuadric();

    // **Outer Wheel Frame**: Two concentric neon green cylinders
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // Bright green glow

    // First cylinder (back edge of the wheel)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -thickness / 1); // Position at the back
    gluCylinder(quadric, radius, radius, thickness, 100, 1); // Outer frame cylinder
    glPopMatrix();

    // Second cylinder (front edge of the wheel)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, thickness / 1); // Position at the front
    gluCylinder(quadric, radius - 0.3f, radius - 0.3f, thickness, 100, 1); // Slightly inner cylinder for depth
    glPopMatrix();

    // **Spokes**: Alternating neon blue and pink lines connecting the center to the outer ring
    int spokeCount = 16; // Number of spokes
    for (int i = 0; i < spokeCount; i++) {
        glPushMatrix();
        glRotatef(i * (360.0f / spokeCount), 0.0f, 0.0f, 1.0f); // Rotate for each spoke

        // Alternate between neon blue and pink
        if (i % 2 == 0) {
            glColor4f(0.0f, 0.1f, 1.0f, 1.0f); // Neon blue
        } else {
            glColor4f(1.0f, 0.0f, 0.6f, 1.0f); // Neon pink
        }

        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -thickness / 2); // Start at the center, back face
        glVertex3f(radius, 0.0f, -thickness / 2); // End at outer frame, back face
        glVertex3f(0.0f, 0.0f, thickness / 2);  // Start at the center, front face
        glVertex3f(radius, 0.0f, thickness / 2);  // End at outer frame, front face
        glEnd();

        glPopMatrix();
    }

    gluDeleteQuadric(quadric);

    // Restore OpenGL states
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopAttrib();

    glPopMatrix();
}

void drawWheelFog(float x, float y, float zStart, float zEnd, float radius, int density) {
    glPushMatrix();
    glTranslatef(x, y, (zStart + zEnd) / 2.0f); // Position the fog field between the wheels

    // Save OpenGL states
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Disable lighting to allow the glow effect
    glDisable(GL_LIGHTING);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set color for the fog (light neon blue with transparency)
    glColor4f(0.0f, 0.7f, 1.0f, 0.5f); // Light blue glow with semi-transparency

    // Draw many small quads or particles for the fog effect
    for (int i = 0; i < density; i++) {
        // Randomize positions slightly around the centerline for a natural foggy look
        float randomX = ((rand() % 100) / 100.0f - 0.5f) * radius * 0.5f; // Slightly random X
        float randomY = ((rand() % 100) / 100.0f - 0.5f) * radius * 0.5f; // Slightly random Y
        float randomZ = ((rand() % 100) / 100.0f - 0.5f) * (zEnd - zStart) * 0.5f; // Random Z along the axis

        glPushMatrix();
        glTranslatef(randomX, randomY, randomZ); // Offset the quad for randomness

        // Draw a small quad representing a fog particle
        glBegin(GL_QUADS);
        glVertex3f(-0.1f, -0.1f, 0.0f);
        glVertex3f(0.1f, -0.1f, 0.0f);
        glVertex3f(0.1f, 0.1f, 0.0f);
        glVertex3f(-0.1f, 0.1f, 0.0f);
        glEnd();

        glPopMatrix();
    }

    // Restore OpenGL states
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopAttrib();

    glPopMatrix();
}

void drawCurvedNeonTunnel(float radius, int segments, int rings) {
    float angleStep = 360.0f / segments; // Angle step for each segment of the ring
    static float motionOffset = 0.0f;   // To animate forward motion
    motionOffset += 0.01f;              // Increment for smooth forward motion

    // Control points for the curve path
    float controlPoints[4][3] = {
        {70.0f, 0.0f, 50.0f},   // Start point
        {50.0f, 120.0f, -100.0f}, // Top curve control point
        {-75.0f, 50.0f, -50.0f}, // Bottom return
        {-60.0f, 0.0f, 50.0f}   // End point
    };

    // Enable emissive lighting for the tunnel
    glPushAttrib(GL_LIGHTING_BIT);
    glEnable(GL_LIGHTING);

    for (int i = 0; i < rings; i++) {
        float t = (float)i / (rings - 1); // Interpolation factor [0, 1]

        // Compute the position and tangent of the curve
        float p[3] = {0}, tangent[3] = {0};
        for (int j = 0; j < 3; j++) { // Interpolating X, Y, Z
            p[j] =
                (1 - t) * (1 - t) * (1 - t) * controlPoints[0][j] +
                3 * (1 - t) * (1 - t) * t * controlPoints[1][j] +
                3 * (1 - t) * t * t * controlPoints[2][j] +
                t * t * t * controlPoints[3][j];
            tangent[j] =
                -3 * (1 - t) * (1 - t) * controlPoints[0][j] +
                (3 * (1 - t) * (1 - t) - 6 * (1 - t) * t) * controlPoints[1][j] +
                (6 * (1 - t) * t - 3 * t * t) * controlPoints[2][j] +
                3 * t * t * controlPoints[3][j];
        }

        // Normalize the tangent
        float tangentLength = sqrt(tangent[0] * tangent[0] + tangent[1] * tangent[1] + tangent[2] * tangent[2]);
        for (int j = 0; j < 3; j++) {
            tangent[j] /= tangentLength;
        }

        // Compute the normal and binormal vectors for consistent orientation
        float normal[3] = {0.0f, 1.0f, 0.0f}; // Default upward vector
        float binormal[3];
        binormal[0] = tangent[1] * normal[2] - tangent[2] * normal[1];
        binormal[1] = tangent[2] * normal[0] - tangent[0] * normal[2];
        binormal[2] = tangent[0] * normal[1] - tangent[1] * normal[0];

        // Recompute the normal to ensure perpendicularity
        normal[0] = binormal[1] * tangent[2] - binormal[2] * tangent[1];
        normal[1] = binormal[2] * tangent[0] - binormal[0] * tangent[2];
        normal[2] = binormal[0] * tangent[1] - binormal[1] * tangent[0];

        // Normalize the binormal and normal
        float binormalLength = sqrt(binormal[0] * binormal[0] + binormal[1] * binormal[1] + binormal[2] * binormal[2]);
        for (int j = 0; j < 3; j++) {
            binormal[j] /= binormalLength;
            normal[j] /= binormalLength;
        }

        // Draw the ring with consistent radius
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < segments; j++) {
            float angle = j * angleStep * (M_PI / 180.0f); // Current angle in radians
            float x = radius * cos(angle) * binormal[0] + radius * sin(angle) * normal[0] + p[0];
            float y = radius * cos(angle) * binormal[1] + radius * sin(angle) * normal[1] + p[1];
            float z = radius * cos(angle) * binormal[2] + radius * sin(angle) * normal[2] + p[2];

            GLfloat emissiveColor[4];

            // Dynamic emissive colors for forward motion
            if (j % 3 == 0) {
                emissiveColor[0] = fabs(sin(motionOffset)); // Blue
                emissiveColor[1] = 0.5f;
                emissiveColor[2] = 1.0f;
                emissiveColor[3] = 1.0f; // Alpha
            } else if (j % 3 == 1) {
                emissiveColor[0] = 1.0f; // Pink
                emissiveColor[1] = 0.078f;
                emissiveColor[2] = fabs(cos(motionOffset));
                emissiveColor[3] = 1.0f; // Alpha
            } else {
                emissiveColor[0] = 0.0f; // Green
                emissiveColor[1] = fabs(sin(motionOffset));
                emissiveColor[2] = 0.0f;
                emissiveColor[3] = 1.0f; // Alpha
            }

            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);
            glColor3f(emissiveColor[0], emissiveColor[1], emissiveColor[2]);

            glVertex3f(x, y, z);
        }
        glEnd();

        // Add vertical connecting lines for density
        if (i > 0) {
            float prevT = (float)(i - 1) / (rings - 1);
            float prevP[3] = {0};
            for (int j = 0; j < 3; j++) {
                prevP[j] =
                    (1 - prevT) * (1 - prevT) * (1 - prevT) * controlPoints[0][j] +
                    3 * (1 - prevT) * (1 - prevT) * prevT * controlPoints[1][j] +
                    3 * (1 - prevT) * prevT * prevT * controlPoints[2][j] +
                    prevT * prevT * prevT * controlPoints[3][j];
            }

            glBegin(GL_LINES);
            for (int j = 0; j < segments; j++) {
                float angle = j * angleStep * (M_PI / 180.0f);
                float xCurrent = radius * cos(angle) * binormal[0] + radius * sin(angle) * normal[0] + p[0];
                float yCurrent = radius * cos(angle) * binormal[1] + radius * sin(angle) * normal[1] + p[1];
                float zCurrent = radius * cos(angle) * binormal[2] + radius * sin(angle) * normal[2] + p[2];

                float xPrev = radius * cos(angle) * binormal[0] + radius * sin(angle) * normal[0] + prevP[0];
                float yPrev = radius * cos(angle) * binormal[1] + radius * sin(angle) * normal[1] + prevP[1];
                float zPrev = radius * cos(angle) * binormal[2] + radius * sin(angle) * normal[2] + prevP[2];

                glVertex3f(xCurrent, yCurrent, zCurrent);
                glVertex3f(xPrev, yPrev, zPrev);
            }
            glEnd();
        }
    }

    // Restore previous lighting state
    glPopAttrib();
}

// ---------------------------------------------------------
// OpenGL Callbacks
// ---------------------------------------------------------

void display(void) {
    // Clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up the third-person camera to follow the UFO
    gluLookAt(
        ufoX - 6.0f * sin(ufoYaw * M_PI / 180.0f), // Camera X (behind the UFO)
        ufoY + 2.0f,                               // Camera Y (above the UFO)
        ufoZ + 6.0f * cos(ufoYaw * M_PI / 180.0f), // Camera Z (behind the UFO)
        ufoX, ufoY + 2.0f, ufoZ,                   // Look at the UFO
        0.0, 1.0, 0.0                              // Up vector
    );
    
    // Set up the projection matrix
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (height == 0) height = 1;  // Prevent division by zero
    float aspect = (float)width / height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 2.0, 1000.0);  // Perspective projection

    glMatrixMode(GL_MODELVIEW);

    // Unified moon function
    drawMoon(20.0f, 400.0f, 0.10f); // Size, orbit radius, orbit speed
    
    // Draw curved tunnel
    drawCurvedNeonTunnel(10.0f, 120, 600); // Radius, Segments, Rings
    
    // Draw neon tunnel
    glPushMatrix();
    drawDenseNeonTunnel(50.0f, 600.0f, 80.0f, 100, 400, 4.0f, -20.0f); // Original position
    glPopMatrix();
    
    // Draw the UFO at its current position
    glPushMatrix();
    glTranslatef(ufoX, ufoY, ufoZ);    // Position the UFO dynamically
    glRotatef(ufoYaw, 0.0f, 1.0f, 0.0f); // Rotate the UFO based on yaw
    drawUFO();                        // Call the function to render the UFO
    glPopMatrix();
    
    // Draw tower
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);  // Center the tower in the city
    drawTower();  // Render the tower
    glPopMatrix();
    
    // Draw the base
    drawFerrisWheelBase(15.0f, 3.0f, 10.0f, 35.0f, 0.0f, 0.0f);

    // Raise the middle pillar slightly
    drawFerrisWheelBase(3.0f, 14.0f, 3.0f, 35.0f, 3.0f, 0.0f); // Increased Y dimension for height

    // Raise the wheel position slightly and keep the two wheels evenly spaced
    drawFerrisWheelWheel(14.0f, 1.5f, 35.0f, 18.0f, -6.0f, ferrisWheelAngle); // Larger radius and thicker wheel
    drawFerrisWheelWheel(14.0f, 1.5f, 35.0f, 18.0f, 6.0f, ferrisWheelAngle);  // Same as above but opposite side
    // Position the fog field on the axis of the wheels
    drawWheelFog(35.0f, 18.0f, -14.0f, 14.0f, 5.0f, 3000); // X, Y, Start Z, End Z, Radius, Density

    
    // Draw clock
    // Draw the clock tower base
    drawClockTowerBase(10.0f, 50.0f, 10.0f, -32.0f, 0.0f, 0.0f);

    // Draw the vertical beams
    drawVerticalPillars(10.0f, 50.0f, 10.0f, 0.5f, -32.0f, 0.0f, 0.0f);

    // Draw the horizontal beams
    drawHorizontalBeams(10.0f, 40.0f, 10.0f, 0.1f, 8, -32.0f, 0.0f, 0.0f);
    
    // Add a pink beam around the top edge of the base
    drawTopPinkBeam(10.0f, 10.0f, 50.0f, 0.1f, -32.0f, 0.0f);

    // Draw the blue beams connecting to the center
    drawBlueCornerBeamsPair1(10.0f, 10.0f, 50.0f, 0.15f, -32.0f, 0.0f, 0.0f);
    drawBlueCornerBeamsPair2(10.0f, 10.0f, 50.0f, 0.15f, -32.0f, 0.0f, 0.0f);
    
    // Draw clocks on all four sides of the tower
    float clockRadius = 4.0f;
    float clockHeight = 45.0f; // Align with the top of the base
    float baseX = -32.0f, baseZ = 0.0f; // Base position
    float wallOffset = 0.3f; // Slightly offset to avoid z-fighting

    // Front
    glPushMatrix();
    glTranslatef(baseX, clockHeight, baseZ + 5.0f + wallOffset); // Offset forward
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // No rotation needed for front
    drawFuturisticClock(clockRadius, 0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // Back
    glPushMatrix();
    glTranslatef(baseX, clockHeight, baseZ - 5.0f - wallOffset); // Offset backward
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Rotate 180 degrees to face backward
    drawFuturisticClock(clockRadius, 0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // Left
    glPushMatrix();
    glTranslatef(baseX - 5.0f - wallOffset, clockHeight, baseZ); // Offset left
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees to face left
    drawFuturisticClock(clockRadius, 0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // Right
    glPushMatrix();
    glTranslatef(baseX + 5.0f + wallOffset, clockHeight, baseZ); // Offset right
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f); // Rotate -90 degrees to face right
    drawFuturisticClock(clockRadius, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
    
    // 3 hexagonal bases with inner prisms
    glPushMatrix();
    glTranslatef(-10.0f, 0.0f, -35.0f); // Left base
    glColor3f(0.0, 0.0, 1.0); // Blue for the lattice
    drawHexagonalBaseWithLattice(5.0f, 3.0f, 8); // Hexagonal base with lattice
    drawVerticalLightBeams(5.0f, 3.0f); // Add light beams along vertices
    glColor3f(0.0, 0.0, 0.0); // Black for the inner prism
    drawInnerPrism(4.5f, 2.99f); // Inner prism
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0f, 0.0f, -35.0f); // Right base
    glColor3f(0.0, 0.0, 1.0);
    drawHexagonalBaseWithLattice(5.0f, 3.0f, 8);
    drawVerticalLightBeams(5.0f, 3.0f);
    glColor3f(0.0, 0.0, 0.0);
    drawInnerPrism(4.5f, 2.99f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -20.0f); // Back base
    glColor3f(0.0, 0.0, 1.0);
    drawHexagonalBaseWithLattice(5.0f, 3.0f, 8);
    drawVerticalLightBeams(5.0f, 3.0f);
    glColor3f(0.0, 0.0, 0.0);
    drawInnerPrism(4.5f, 2.99f);
    glPopMatrix();
    
    // Left base spiral
    glPushMatrix();
    glTranslatef(-10.0f, 3.0f, -35.0f); // Start at the top of the base
    drawHelicalHourglass(5.0f, 0.5f, 10.0f, 3, 100); // Base radius, top radius, height, twists, segments
    glPopMatrix();

    // Right base spiral
    glPushMatrix();
    glTranslatef(10.0f, 3.0f, -35.0f); // Start at the top of the base
    drawHelicalHourglass(5.0f, 0.5f, 10.0f, 3, 100); // Base radius, top radius, height, twists, segments
    glPopMatrix();

    // Back base spiral
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, -20.0f); // Start at the top of the base
    drawHelicalHourglass(5.0f, 0.5f, 10.0f, 3, 100); // Base radius, top radius, height, twists, segments
    glPopMatrix();
    
    // Place platforms atop the helices
    drawTorusPlatforms();
    
    drawFogFields();
    
    // Place the glowing triangular base just above the fog
    glPushMatrix();
    glTranslatef(0.0f, 6.5f, -5.0f); // Position closer to fog
    drawGlowingTriangularBase(10.0f); // Adjust base height and size
    glPopMatrix();

    // Place arched columns slightly inside the triangular base
    glPushMatrix();
    glTranslatef(-14.0f, 16.5f, -38.0f); // Adjusted left vertex position
    drawArchedColumn(1.0f, 10.0f, 36);  // Base radius, height, and segments
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14.0f, 16.5f, -38.0f); // Adjusted right vertex position
    drawArchedColumn(1.0f, 10.0f, 36);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 16.5f, -14.5f); // Adjusted back vertex position
    drawArchedColumn(1.0f, 10.0f, 36);
    glPopMatrix();
    
    // Wall between left and right columns
    glPushMatrix();
    glTranslatef(0.0f, 16.5f, -38.0f); // Midpoint between left and right columns
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // No rotation needed
    drawTessellatedWall(28.0f, 10.0f, 10, 10); // Width spans distance between left and right columns
    glPopMatrix();

    // Wall between left and back columns
    glPushMatrix();
    glTranslatef(-7.0f, 16.5f, -26.25f); // Midpoint between left and back columns
    glRotatef(120.0f, 0.0f, 1.0f, 0.0f); // Rotate 120 degrees to align
    drawTessellatedWall(28.0f, 10.0f, 10, 10);
    glPopMatrix();

    // Wall between right and back columns
    glPushMatrix();
    glTranslatef(7.0f, 16.5f, -26.25f); // Midpoint between right and back columns
    glRotatef(-120.0f, 0.0f, 1.0f, 0.0f); // Rotate -120 degrees to align
    drawTessellatedWall(28.0f, 10.0f, 10, 10);
    glPopMatrix();
    
    // Add spires on top of the columns
    glPushMatrix();
    glTranslatef(-14.0f, 26.5f, -38.0f); // Left column spire
    drawNeoGothicSpire(0.5f, 8.0f, 36); // Base radius, height, segments
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14.0f, 26.5f, -38.0f); // Right column spire
    drawNeoGothicSpire(0.5f, 8.0f, 36);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 26.5f, -14.5f); // Back column spire
    drawNeoGothicSpire(0.5f, 8.0f, 36);
    glPopMatrix();
    
    // Arch between left and right columns
    drawArchedButtress(-14.0f, 26.5f, -38.0f, 14.0f, 26.5f, -38.0f, 5.0f, 20);

    // Arch between left and back columns
    drawArchedButtress(-14.0f, 26.5f, -38.0f, 0.0f, 26.5f, -14.5f, 5.0f, 20);

    // Arch between right and back columns
    drawArchedButtress(14.0f, 26.5f, -38.0f, 0.0f, 26.5f, -14.5f, 5.0f, 20);
    
    // Draw floor
    drawFloor();
    
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Center it at the origin
    drawSkyDome(500.0f, skyDomeTexture); // Large radius for the dome
    glPopMatrix();

    // Inner grid
    positionAndScaleBuilding(5.0, 5.0, 1.2, 3.5, 1.2, 15.0);
    positionAndScaleBuilding(-5.0, 5.0, 1.2, 3.5, 1.2, -15.0);
    positionAndScaleBuilding(5.0, -5.0, 1.2, 3.5, 1.2, 15.0);
    positionAndScaleBuilding(-5.0, -5.0, 1.2, 3.5, 1.2, -15.0);
    positionAndScaleBuilding(8.0, 3.0, 1.0, 3.0, 1.0, 30.0);
    positionAndScaleBuilding(-8.0, 3.0, 1.0, 3.0, 1.0, -30.0);
    positionAndScaleBuilding(8.0, -3.0, 1.0, 3.0, 1.0, 30.0);
    positionAndScaleBuilding(-8.0, -3.0, 1.0, 3.0, 1.0, -30.0);
    positionAndScaleBuilding(10.0, 0.0, 1.3, 4.0, 1.3, 10.0);
    positionAndScaleBuilding(-10.0, 0.0, 1.3, 4.0, 1.3, -10.0);
    positionAndScaleBuilding(6.0, 6.0, 1.1, 3.8, 1.1, 45.0);
    positionAndScaleBuilding(-6.0, 6.0, 1.1, 3.8, 1.1, -45.0);

    // Middle grid
    positionAndScaleBuilding(15.0, 15.0, 1.5, 5.0, 1.5, 20.0);
    positionAndScaleBuilding(-15.0, 15.0, 1.5, 5.0, 1.5, -20.0);
    positionAndScaleBuilding(15.0, -15.0, 1.5, 5.0, 1.5, 20.0);
    positionAndScaleBuilding(-15.0, -15.0, 1.5, 5.0, 1.5, -20.0);
    positionAndScaleBuilding(18.0, 12.0, 1.8, 4.8, 1.8, 30.0);
    positionAndScaleBuilding(-18.0, 12.0, 1.8, 4.8, 1.8, -30.0);
    positionAndScaleBuilding(18.0, -12.0, 1.8, 4.8, 1.8, 30.0);
    positionAndScaleBuilding(-18.0, -12.0, 1.8, 4.8, 1.8, -30.0);
    positionAndScaleBuilding(20.0, 10.0, 1.2, 6.0, 1.2, 45.0);
    positionAndScaleBuilding(-20.0, 10.0, 1.2, 6.0, 1.2, -45.0);
    positionAndScaleBuilding(20.0, -10.0, 1.2, 6.0, 1.2, 45.0);
    positionAndScaleBuilding(-20.0, -10.0, 1.2, 6.0, 1.2, -45.0);
    positionAndScaleBuilding(12.0, 12.0, 1.4, 5.5, 1.4, 15.0);
    positionAndScaleBuilding(-12.0, 12.0, 1.4, 5.5, 1.4, -15.0);

    // Outer grid
    positionAndScaleBuilding(25.0, 25.0, 2.0, 7.0, 2.0, 0.0);
    positionAndScaleBuilding(-25.0, 25.0, 2.0, 7.0, 2.0, 0.0);
    positionAndScaleBuilding(25.0, -25.0, 2.0, 7.0, 2.0, 0.0);
    positionAndScaleBuilding(-25.0, -25.0, 2.0, 7.0, 2.0, 0.0);
    positionAndScaleBuilding(30.0, 20.0, 1.8, 6.5, 1.8, 15.0);
    positionAndScaleBuilding(-30.0, 20.0, 1.8, 6.5, 1.8, -15.0);
    positionAndScaleBuilding(30.0, -20.0, 1.8, 6.5, 1.8, 15.0);
    positionAndScaleBuilding(-30.0, -20.0, 1.8, 6.5, 1.8, -15.0);
    positionAndScaleBuilding(35.0, 15.0, 1.5, 8.0, 1.5, 30.0);
    positionAndScaleBuilding(-35.0, 15.0, 1.5, 8.0, 1.5, -30.0);
    positionAndScaleBuilding(35.0, -15.0, 1.5, 8.0, 1.5, 30.0);
    positionAndScaleBuilding(-35.0, -15.0, 1.5, 8.0, 1.5, -30.0);

    // Far corners
    positionAndScaleBuilding(40.0, 40.0, 2.3, 9.0, 2.3, 10.0);
    positionAndScaleBuilding(-40.0, 40.0, 2.3, 9.0, 2.3, -10.0);
    positionAndScaleBuilding(40.0, -40.0, 2.3, 9.0, 2.3, 10.0);
    positionAndScaleBuilding(-40.0, -40.0, 2.3, 9.0, 2.3, -10.0);
    positionAndScaleBuilding(45.0, 30.0, 1.7, 7.5, 1.7, 20.0);
    positionAndScaleBuilding(-45.0, 30.0, 1.7, 7.5, 1.7, -20.0);
    positionAndScaleBuilding(45.0, -30.0, 1.7, 7.5, 1.7, 20.0);
    positionAndScaleBuilding(-45.0, -30.0, 1.7, 7.5, 1.7, -20.0);
    positionAndScaleBuilding(50.0, 35.0, 2.0, 10.0, 2.0, 45.0);
    positionAndScaleBuilding(-50.0, 35.0, 2.0, 10.0, 2.0, -45.0);
    positionAndScaleBuilding(50.0, -35.0, 2.0, 10.0, 2.0, 45.0);
    positionAndScaleBuilding(-50.0, -35.0, 2.0, 10.0, 2.0, -45.0);

    // Additional scattered buildings for density
    positionAndScaleBuilding(12.0, 30.0, 1.1, 4.5, 1.1, 60.0);
    positionAndScaleBuilding(-12.0, 30.0, 1.1, 4.5, 1.1, -60.0);
    positionAndScaleBuilding(30.0, 12.0, 1.0, 3.5, 1.0, 25.0);
    positionAndScaleBuilding(-30.0, 12.0, 1.0, 3.5, 1.0, -25.0);
    positionAndScaleBuilding(30.0, -12.0, 0.9, 2.8, 0.9, 10.0);
    positionAndScaleBuilding(-30.0, -12.0, 0.9, 2.8, 0.9, -10.0);
    positionAndScaleBuilding(40.0, 38.0, 1.5, 5.0, 1.5, 25.0);
    positionAndScaleBuilding(-40.0, 38.0, 1.5, 5.0, 1.5, -25.0);
    positionAndScaleBuilding(35.0, 42.0, 1.3, 6.0, 1.3, 35.0);
    positionAndScaleBuilding(-35.0, 42.0, 1.3, 6.0, 1.3, -35.0);

    displayHUD();
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
}

void handleInput(unsigned char key, int x, int y) {
    if (key == 27) { // ESC key to quit
        exit(0);
    }

    keyState[key] = true; // Mark key as pressed

    if ((key == 'w' || key == 'W') && !displayControls) {
        displayControls = true; // Show the control HUD
    }

    glutPostRedisplay();
}

void handleInputUp(unsigned char key, int x, int y) {
    keyState[key] = false; // Mark key as released
    glutPostRedisplay();
}

void handleSpecialKeys(int key, int x, int y) {
    specialKeyState[key] = true; // Mark special key as pressed
    glutPostRedisplay();
}

void handleSpecialKeysUp(int key, int x, int y) {
    specialKeyState[key] = false; // Mark special key as released
    if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT) {
        rotationIncrement = 0.0f; // Stop rotation
    }
    glutPostRedisplay();
}

void idle(void) {
    float yawRadians = ufoYaw * (M_PI / 180.0f); // Convert yaw to radians
    
    // Update rotation angle for the ferris wheel
    ferrisWheelAngle += 0.5f; // Adjust speed as needed
    if (ferrisWheelAngle >= 360.0f) {
        ferrisWheelAngle -= 360.0f; // Keep within 360 degrees
    }

    // Reset velocities to zero before checking keys
    velocityX = 0.0f;
    velocityZ = 0.0f;
    velocityY = 0.0f;

    // Handle forward/backward movement
    if (keyState['w'] || keyState['W']) { // Move forward
        velocityX += moveSpeed * sin(yawRadians);
        velocityZ -= moveSpeed * cos(yawRadians);
    }
    if (keyState['s'] || keyState['S']) { // Move backward
        velocityX -= moveSpeed * sin(yawRadians);
        velocityZ += moveSpeed * cos(yawRadians);
    }

    // Handle strafing
    if (keyState['a'] || keyState['A']) { // Strafe left
        velocityX -= moveSpeed * cos(yawRadians);
        velocityZ -= moveSpeed * sin(yawRadians);
    }
    if (keyState['d'] || keyState['D']) { // Strafe right
        velocityX += moveSpeed * cos(yawRadians);
        velocityZ += moveSpeed * sin(yawRadians);
    }

    // Handle vertical movement
    if (specialKeyState[GLUT_KEY_UP]) { // Move up
        velocityY += moveSpeed / 2;
    }
    if (specialKeyState[GLUT_KEY_DOWN]) { // Move down
        velocityY -= moveSpeed / 2;
    }

    // Handle rotation
    if (specialKeyState[GLUT_KEY_LEFT]) { // Rotate left
        rotationIncrement = -turnSpeed * 0.1f;
    }
    if (specialKeyState[GLUT_KEY_RIGHT]) { // Rotate right
        rotationIncrement = turnSpeed * 0.1f;
    }
    ufoYaw += rotationIncrement; // Apply rotation increment

    // Update UFO position
    ufoX += velocityX;
    ufoZ += velocityZ;
    ufoY += velocityY;

    // Prevent UFO from going below the ground
    if (ufoY < 0.5f) {
        ufoY = 0.5f;
        velocityY = 0.0f;
    }

    glutPostRedisplay(); // Redraw the scene
}

// ---------------------------------------------------------
// Initialization Functions
// ---------------------------------------------------------

// Initialization function for setting up OpenGL properties
void initOpenGL(void) {
    glEnable(GL_DEPTH_TEST);  // Enable depth testing (for hidden surface removal)
    glClearColor(0.0, 0.0, 0.05, 1.0);  // Near-black with a slight blue tint
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
    floorTexture = LoadTexBMP("metal.bmp");
    lightTexture = LoadTexBMP("moon.bmp");
    clockTowerTexture = LoadTexBMP("Sci-fi_Armor_001_basecolor.bmp");
    ufoTexture = LoadTexBMP("MetalPlates006_2K-JPG_Color.bmp");
    skyDomeTexture = LoadTexBMP("space_texture.bmp");
    sciFiTexture = LoadTexBMP("sci_fi_texture.bmp");


    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Enable two-sided lighting for more realistic lighting
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    // Automatically normalize normals after transformations
    glEnable(GL_NORMALIZE);
}

// ---------------------------------------------------------
// Main Function
// ---------------------------------------------------------

int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Set display mode (double buffering, RGB, depth)
    glutInitWindowSize(800, 600);  // Set initial window size
    glutCreateWindow("3D UFO Adventure - Roman DiDomizio");  // Create window with name in title
    glutFullScreen();  // Enable fullscreen mode

    initOpenGL();  // Call our function to set up OpenGL settings

    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handleInput);         // For key press
    glutKeyboardUpFunc(handleInputUp);     // For key release
    glutSpecialFunc(handleSpecialKeys);    // For special key press
    glutSpecialUpFunc(handleSpecialKeysUp); // For special key release
    glutIdleFunc(idle);

    glutMainLoop();  // Enter the main loop and wait for events
    return 0;
}
