/* Authors: Tyvon Factor-Gaymon (tf18wj - 6580310) & Peter Fung(pf18np - 6509830)
* Inspiration: This code used Professor Brian Ross' original rotate.c as a template
*/

#if !defined(Linux)
#include <windows.h>           //Not Linux must be windows
#else
#include <stdlib.h>
#endif
#include <GL/gl.h>
#include <stdio.h>
#include <glut.h>
#include "Particle.h"
#include <vector>
#include <random>
#include <iostream>
#include <cstdlib> 


using namespace std;
// Function Declarations
void animate_timer(int val);
void add_particle_timer(int val);
void randomizeSpeed(void);
void randomizeSpin(void);
void toggleFriction(void);
void randomizeSize(void);
void toggleView(void);
void toggleRandomShapes(void);
void color_timer(int val);

#define X 0
#define Y 1
#define Z 2

vector<Particle> particles;
Particle focusedParticle;

float add_time = 100.0;
float animate_time = 20.0;
float color_time = 700.0;
int view = 0;
int addSpeed = 0;
int particleSpeed = 0;
int numOfParticles = 0;

// GloabalControl Flags
bool randomSpeed = false;
bool randomSpin = false;
bool randomSize = false;
bool friction = false;
bool focusParticle = false;
bool changeShapes = false;
bool randomColors = false;
bool particleCollision = false;

struct glob {
    float angle[3];
    int axis;
};

struct glob global = { {0.0,0.0,0.0},Y };
// This function prints the console and informs the user of a list of program functions.
void userintro() { // Menu
    printf("Left mouse - rotate ground left faster\n");
    printf("Right mouse - rotate ground right faster\n");
    printf("Middle mouse - reset\n");
    printf("x, y, z - Rotate ground about x, y, or z axis\n");
    printf("s - Toggle Random Particle speed\n");
    printf("f - Manually Fire Particle (hold down to keep firing)\n");
    printf("a - Toggle random spin to existing airborne particles.\n");
    printf("t - Toggle friction. (particles will retain 60%% of its current speed with each bounce)\n");
    printf("e - Toggle Random Size.\n");
    printf("v - Toggle Camera Views. (No Rotation, No rotation Flipped, Birds-Eye-View, Birds-Eye-View of opposite side, Initial View)\n");
    printf("g - Toggle Shapes (Change Particles to random Shapes / Change particle back to original shape)\n");
    printf("w - Toggle Random Color. (on/off)\n");
    printf("h - Toggle Speed at which particles are added. (Slow, Fast Default)\n");
    printf("n - Toggle Particle Speed. (Slow, Fast Default)\n");
    printf("r - Reset program to initial conditions. (for full reset press f and then r)\n");
    printf("Press f and then Press r - Full Reset, resets all glut timers");
    printf("q - Quit\n\n");

    printf("Bonuses\n");
    printf("1: e - Toggle Random Size\n");
    printf("2: v - Toggle Camera Views\n");
    printf("3: g - Toggle Random Shapes\n");
    printf("4: w - Toggle Random Color\n");
    printf("5: h - Toggle Speed at which particles are added.\n");
    printf("6: n - Toggle Particle Speed.\n");
    printf("7: p - Particle Collision\n");
    printf("Non-togglable Features: ");
    printf("8: default - Hole in the ground\n");
    printf("9: Sound FX each time particle hits the ground");

    printf("Note: For full reset press f and then r. This will fully reset the particle add timer.");
} // userintro

// This function maps the keyboard keys to different functions.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'x':
    case 'X':
        global.axis = X;
        break;
    case 'y':
    case 'Y':
        global.axis = Y;
        break;
    case 'z':
    case 'Z':
        global.axis = Z;
        break;
    case 'f':
    case 'F':
        add_time = 10000000.0; // essentially stops the firing. 
        add_particle_timer(1);
        break;
    case 's':
    case 'S':
        if (randomSpeed) {
            randomSpeed = false;
        }
        else {
            randomSpeed = true;
        }
        randomizeSpeed();
        break;
    case 'a':
    case 'A':
        if (randomSpin) {
            randomSpin = false;
        }
        else {
            randomSpin = true;
            randomizeSpin();
        }
        break;
    case 't':
    case 'T':
        if (friction) {
            friction = false;
        }
        else {
            friction = true;
        }
        toggleFriction();
        break;
    case 'e':
    case 'E':
        if (randomSize) {
            randomSize = false;
        }
        else {
            randomSize = true;
        }
        randomizeSize();
        break;
    case 'v':
    case 'V':
        view += 1;
        if (view == 5) {
            view = 0;
        }
        toggleView();
        break;
    case 'g':
    case 'G':
        if (changeShapes) {
            changeShapes = false;
        }
        else {
            changeShapes = true;
        }
        toggleRandomShapes();
        break;
    case 'w':
    case 'W':
        if (randomColors) {
            randomColors = false;
            color_time = 10000000.0; // essentially stops changing colors. 
        }
        else {
            randomColors = true;
            color_time = 300.0; // essentially stops changing colors. 
        }
        color_timer(0);
        break;
    case 'h':
    case 'H': // Asjust the speed at which particles are added.
        add_time = 10000000.0;
        add_particle_timer(1);
        addSpeed += 1;
        if (addSpeed > 2) { addSpeed = 0; }
        if (addSpeed == 0) { add_time = 100; } // Default
        else if (addSpeed == 1) { add_time = 1000; } // Slow
        else if (addSpeed == 2) { add_time = 1; } // Fast
        break;
    case 'n':
    case 'N': // Asjust the speed at which particles are added.
        animate_time = 1000000.0;
        particleSpeed += 1;
        if (particleSpeed > 2) { particleSpeed = 0; }
        if (particleSpeed == 0) { animate_time = 20; } // Default
        else if (particleSpeed == 1) { animate_time = 60; } // Slow
        else if (particleSpeed == 2) { animate_time = 1; } // Fast
        break;
    case 'r': // reset all initial values back to original values.
    case 'R':      // Important, to fully reset particle add timer, press f and then r
        //add_particle_timer(1);
        animate_time = 10000000.0;
        animate_timer(0);
        color_time = 700.0;
        view = 0;
        addSpeed = 0;
        randomSpeed = false;
        randomSpin = false;
        friction = false;
        randomSize = false;
        focusParticle = false;
        changeShapes = false;
        randomColors = false;
        particleCollision = false;
        particles.clear(); // emprty particle vector
        glLoadIdentity(); // reset view
        glRotatef(30.0, 1.0, 0.0, 0.0);
        glRotatef(30.0, 0.0, 1.0, 0.0);
        animate_time = 20.0;
        add_time = 100.0;
        add_particle_timer(1);
        break;
    case 'p':
    case 'P':
        particleCollision = true;
        break;
    case 0x1B:
    case 'q':
    case 'Q':
        exit(0);
        break;

    }
} // keyboard

/* This function maps different mouse button clicks to different functions.
* Pressing the left mouse button rotates around the positive x-axis.
* Pressing the right mouse button rotates around the negative x-axis.
* Pressing the middle mouse button resets the display to its initial position.
*/
void mouse(int btn, int state, int x, int y) {


    if (state == GLUT_DOWN) {
        if (btn == GLUT_LEFT_BUTTON) {
            global.angle[global.axis] = global.angle[global.axis] + 0.2;
        }
        else if (btn == GLUT_RIGHT_BUTTON) {
            global.angle[global.axis] = global.angle[global.axis] - 0.2;
        }
        else {
            global.angle[X] = 0.0;
            global.angle[Y] = 0.0;
            global.angle[Z] = 0.0;
            glPopMatrix();
            glPushMatrix();
        }
    }
} // mouse

// This function is repsonsible for all the drawing done on the display.
void drawGround(void) {
    // An array that contains 8 vertices of the cube
    int v[][3] = { {30,1,30}, {30,-1,30}, {-30,-1,30}, {-30,1,30},
                   {30,1,-30}, {30,-1,-30}, {-30,-1,-30}, {-30,1,-30} };
    //  an array of 6 cube faces
    int e[][4] = { {0,3,2,1},{3,7,6,2},{7,4,5,6},{4,0,1,5}, {0,4,7,3},{1,2,6,5} };
    // Represents the color of each face
    float c[][3] = { {1.0,0,0},{0,1.0,0},{1.0,1.0,1.0},
           {0,0,1.0},{.6,0,.6},{0,.6,.6} };
    int i;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    // Rotate cube
    glRotatef(global.angle[X], 1.0, 0.0, 0.0);
    glRotatef(global.angle[Y], 0.0, 1.0, 0.0);
    glRotatef(global.angle[Z], 0.0, 0.0, 1.0);

    // Iterate over the 6 faces to draw cube
    for (i = 0; i < 6; ++i) {
        // set the color for each face
        glColor3fv(c[i]);
        // Draw qualdrilateral 
        glBegin(GL_QUADS);
        glVertex3iv(v[e[i][0]]);
        glVertex3iv(v[e[i][1]]);
        glVertex3iv(v[e[i][2]]);
        glVertex3iv(v[e[i][3]]);
        glEnd();
    }

    // Draw Fountain
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glColor3f(0.5, 0.5, 0.5);
    glutSolidCube(5.0);
    glPopMatrix();

    // Draw all particles and check all conditions
    for (int i = 0; i < particles.size(); i++) {
        particles[i].draw();
        glutPostRedisplay();
        // Check to see if particle is expired.
        if (particles[i].expired()) {
            particles.erase(particles.begin() + i);
        }
        if (!particles.empty()) {
            particles[i].bounce(30.0);
        }
    }

    //drawSquareHole
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    int x = 12;
    int y = 2;
    int z = 12;
    int x2 = 6;
    int z2 = 5;
    glVertex3i(-x, y, z);
    glVertex3i(x, y, z);
    glVertex3i(x, y, z + 10);
    glVertex3i(-x, y, z + 10);
    glVertex3i(-x, y, z);
    glEnd();

    glutSwapBuffers();
    glFlush();
} // drawGround

/*This method gets the distance between one particle and another in the 3D plane
* @param Particle p is the main particle
* @param Particle x is the particle that is getting compared to Particle p
*/
double getDistance(Particle& p, Particle& x) {
    double distanceX = pow(((double)p.getX()) - ((double)x.getX()), 2);
    double distanceY = pow(((double)p.getY()) - ((double)x.getY()), 2);
    double distanceZ = pow(((double)p.getZ()) - ((double)x.getZ()), 2);
    double distance = sqrt(distanceX + distanceY + distanceZ);
    return distance;
}

/*This method resolves the collision between two particles
@param Particle p, the target particle
@param Particle x, the particle that is getting compared to the target particle
*/
void collisionResolution(Particle& p, Particle& x) {
    bool xAxis = (p.getX() > x.getX());
    bool yAxis = (p.getY() > x.getY());
    bool zAxis = (p.getZ() > x.getZ());
    float newVelocity;
    if (xAxis) {
        newVelocity = -p.getVX();
        p.setVX(newVelocity);
    }

    if (yAxis) {
        newVelocity = -p.getVY();
        p.setVY(newVelocity);
    }

    if (zAxis) {
        newVelocity = -p.getVZ();
        p.setVZ(newVelocity);
    }

}

/*This method detects any collision between two particles
@param Particle p, the target particle
*/
void collisionWithParticles(Particle& p) {
    for (Particle& x : particles) {
        if (p.getID() != x.getID()) {//checking to see if same particle
            //printf("true");
            double distance = getDistance(p, x);
            //printf("%f", distance);
            if (distance <= p.getSize() * 1.5) {
                //printf("collided");
                collisionResolution(p, x);
            }
        }

    }
}

/* This timer is used to animate the particles and update their directions
* and their positions.
*/
void animate_timer(int value) {
    // update position of all particles
    // We use & here because we want to pdate the particles in-place
    for (Particle& p : particles) {
        p.updatePos();
        p.increment_age(animate_time / 1000);
        if (particleCollision) {
            if (p.getAge() > 0.8) {
                collisionWithParticles(p);
                //printf("please please please please work");
            }
        }
        if (randomSpin) {
            p.updateAngle();
        }
    }
    //glutPostRedisplay();
    glutTimerFunc(animate_time, animate_timer, 0);
} // animate_timer

// This function adds a particle to the particles vector.
void add_particle(void) {
    Particle p;
    float vx = (((float)rand() / RAND_MAX) * 0.3) - 0.15;
    float vz = (((float)rand() / RAND_MAX) * 0.3) - 0.15;
    int r = rand() % 5;
    if (randomSpeed) {
        p.setRandomSpeed();
    }
    if (randomSpin) {
        p.toggleSpin();
    }
    if (randomSize) {
        p.setRandomSize();
    }
    if (friction) {
        p.applyFriction();
    }
    p.initializeDirection(vx, vz);
    p.setID(numOfParticles);
    numOfParticles++;
    p.setShape(r);
    particles.push_back(p);
}// add_particle

/* This function is used to add a new particle to the particles list
* and in doing so, allow a new particle to be added to the fountain.
* It also initializes the particles vx & vz direction values.
* vy will always start off at 1.
*/
void add_particle_timer(int value) {
    add_particle();
    glutTimerFunc(add_time, add_particle_timer, 0);
} // add_particle_timer

/* This timer function changes the color of the particles to a random color
 * over intervals and it depends on the color_time interval.
 */
void color_timer(int value) {
    for (Particle& p : particles) {
        p.updateColor(randomColors);
    }
    glutTimerFunc(color_time, color_timer, 0);
} // animate_timer

/* This function determines if each particle should be assigned a random speed
 * or if they should have their speeds reset back to the default speed.
 */
void randomizeSpeed(void) {
    if (randomSpeed) {
        for (Particle& p : particles) {
            p.setRandomSpeed();
        }
    }
    else {
        for (Particle& p : particles) {
            p.setDefaultSpeed();
        }
    }
} // randomizeSpeed

// This function causes all the particles to spin in random orientations.
void randomizeSpin(void) {
    for (Particle& p : particles) {
        p.toggleSpin();
    }
} // randomizeSpin

/* This function determines if each particle should be assigned a random size
 *or if their size should be set to the default size.
 */
void randomizeSize(void) {
    if (randomSize) {
        for (Particle& p : particles) {
            p.setRandomSize();
        }
    }
    else {
        for (Particle& p : particles) {
            p.setDefaultSize();
        }
    }
} // randomizeSpin

/* This function determines if friction should be enabled or
 * removed from all particles.
 */
void toggleFriction(void) {
    if (friction) {
        for (Particle& p : particles) {
            p.applyFriction();
        }
    }
    else {
        for (Particle& p : particles) {
            p.removeFriction();
        }
    }
} // addFriction

/* This function toggles the camera view.
 * There are 3 camera views.
 * 1) Resets the camera to the initial rotated view;
 * 2) The camera position has no rotation.
 * 3) The camera view has no rotation and is upside down.
 * 4) The camera is set to a birds-eye-view of the ground.
 * 5) The camera is set to a birds-eye-view from the opposite side.
 */
void toggleView() {
    switch (view) {
    case 0: // reset to initial rotated view.
        glLoadIdentity();
        glRotatef(30.0, 1.0, 0.0, 0.0);
        glRotatef(30.0, 0.0, 1.0, 0.0);
        break;
    case 1: // Remove Rotation
        glLoadIdentity();
        break;
    case 2: // Flip Upside down
        glLoadIdentity();
        glRotatef(180.0, 1.0, 0.0, 0.0);
        break;
    case 3: // Birds-eye-view
        glLoadIdentity();
        glRotatef(90, 1.0, 0.0, 0.0);
        break;
    case 4: // Birds-eye-view from opposite side.
        glLoadIdentity();
        glRotatef(-90, 1.0, 0.0, 0.0);
        break;
    }
} // toggleView

 /* This function determines if each particle will change into a random shape
  * or if each particle should transform back into its original shape.
  */
void toggleRandomShapes(void) {
    if (changeShapes) {
        for (Particle& p : particles) {
            p.changeShape();
        }
    }
    else {
        for (Particle& p : particles) {
            p.setOriginalShape();
        }
    }
} // randomizeSpin

// This is the main function that runs the whole program.
int main(int argc, char** argv) {
    srand(0);

    //Temp for testing 1 particle
    //Particle p;
    //float vx = ((float)rand() / (RAND_MAX)) -0.1;
    //float vz = ((float)rand() / (RAND_MAX)) -0.1;
    //p.initializeDirection(vx, vz);
    //particles.push_back(p);

    userintro();
    glutInit(&argc, argv);
    glutInitWindowSize(700, 700);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Paritcle Fountain");
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    glutDisplayFunc(drawGround);
    glutIdleFunc(drawGround);

    /* Perform matrix operations on projection matrix stack
     * This matrix is used to transform the objects in the scene relative to the camera
     * This is wehere we apply transofmrations to move, rotate, and scale your objects
     */
    glMatrixMode(GL_PROJECTION);
    glOrtho(-50.0, 50.0, -50.0, 50.0, -500.0, 500.0);

    // Perform matrix operations on movelview matrix stack
    // This matrix is used to transform the scene relative to the camera
    glMatrixMode(GL_MODELVIEW);

    // Initial Rotation of cube
    glRotatef(30.0, 1.0, 0.0, 0.0);
    glRotatef(30.0, 0.0, 1.0, 0.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    animate_timer(0);
    add_particle_timer(0);

    glutMainLoop();
} // main