/* Authors: Tyvon Factor-Gaymon (tf18wj - 6580310) & Peter Fung(pf18np - 6509830)
*/


#pragma once
#if !defined(Linux)
#include <windows.h>           //Not Linux must be windows
#else
#include <stdlib.h>
#endif
#include <GL/gl.h>
#include <glut.h>
#include <random>


/*
* This class represents a single particle.
*/
class Particle {
private:
	// position
	float x = 0.0;
	float y = 1.0;
	float z = 0.0;

	// direction / velocity 
	float vx = 0.0;
	float vy = 1.0;
	float vz = 0.0;

	// rotation
	float ax = 0.0;
	float ay = 1.0;
	float az = 0.0;

	// spin increments
	float dax = 0.0;
	float day = 0.0;
	float daz = 0.0;

	float speed = 0.015; // The speed of the particle.
	float angle = 25.0; // The rotation angle of the particle.
	bool randomSpin = false;

	// Control Particle color
	bool randomColor = false;
	float r1;
	float r2;
	float r3;

	int shape = 0; // Determines what shpae the particle will be.
	int originalShape = 0; // Stores the original shape in case shape is changed.
	float size = 1.0; // The size of the particle.
	float friction = -1;
	int bounces = 0; // Keeps track of how many times a particle has bounced.
	float age = 0.0;  // Used to keep track of particle's lifespan.
	int id; //id of a particle

public:
	// This method sets the direction values.
	void initializeDirection(float xVal, float zVal) {
		vx = xVal;
		vz = zVal;
	} // initializeDirection

	/* This method updates the position of the particle.
	 * It also takes into account whether the particle is spinning.
	 */
	void updatePos() {
		if (bounces < 3) {
			x += vx;
			y += vy;
			z += vz;
		}
		// decrement vy to simulate gravity
		vy -= speed;

		if (randomSpin) {
			if (angle == 360) {
				angle = 0;
			}
			ax += dax; //0.64
			ay += day; //0.07
			az += daz; //0.27
			// Reverse orientation so we stay within bounds (-1,1)
			if (ax > 1) {
				dax *= -1;
			}
			if (ay > 1) {
				day *= -1;
			}
			if (az > 1) {
				daz *= -1;
			}
			//glRotatef(angle, ax, ay, az);
		}
		else { // stop spinning
			dax = 0;
			day = 0;
			daz = 0;
			angle = 0;
		}
	} // updatePos

	// This method updates the direction of the particle.
	void updateDirection(float yVal) {
		//vx += xVal;
		vy += yVal;
		//vz += zVal;
	} // updatePos


	/* This method is responsibel for drawing the particle.
	* It will move the particle, determine whether it should be rotated,
	* and it will also determine the shape of the particle.
	*/
	void draw() {
		// save current matrix to stack, so later transformations
		// can be applied without affecting its previous state
		glPushMatrix(); // IMPORTANT
		// Move move view matrix so that we draw according to where we've moved it
		glTranslatef(x, y, z);
		if (randomSpin) {
			glRotatef(angle, ax, ay, az);
		}

		if (randomColor) {
			setShapeColor();
		}
		else {
			determineShape();
		}
		// Pop previous stack from stack to restore it.
		// This undoes any transformations that were applied since the last glpushmatrix call
		// With that said, the smaller matrix will still be drawn at the location we drew it at
		// Subsequent commands will now be drawn relative to the initial position of (0,0,0)
		glPopMatrix(); // IMPORTANT
	}

	// This method sets the default shape and color of the particle.
	void determineShape() {
		switch (shape) { // with original color
		case 0:
			glColor3f(0.0, 1.0, 1.0);
			glutSolidCube(size); // draw solid cube with 1x1x1 dimensions glutSolidCube();
			break;
		case 1:
			glColor3f(1.0, 1.0, 0.0);
			glutSolidSphere(size, 25, 25);
			break;
		case 2:
			glColor3f(1.0, 1.0, 1.0);
			glutSolidTorus(size * 0.3, size, 30, 30);
			break;
		case 3:
			glColor3f(1.0, 0.0, 0.0);
			glutSolidCone(size, size * 2, 20, 20);
			break;
		case 4:
			glColor3f(0.0, 1.0, 0.0);
			glScalef(size * 2.0, size * 2.0, size * 2.0);
			glutSolidOctahedron();
			break;
		}
	}

	// This method sets the default shape and random color of the particle.
	void setShapeColor() {
		switch (shape) { // with original color
		case 0:
			glColor3f(r1, r2, r3);
			glutSolidCube(size); // draw solid cube with 1x1x1 dimensions glutSolidCube();
			break;
		case 1:
			glColor3f(r1, r2, r3);
			glutSolidSphere(size, 25, 25);
			break;
		case 2:
			glColor3f(r1, r2, r3);
			glutSolidTorus(size * 0.3, size, 30, 30);
			break;
		case 3:
			glColor3f(r1, r2, r3);
			glutSolidCone(size, size * 2, 20, 20);
			break;
		case 4:
			glColor3f(r1, r2, r3);
			glScalef(size * 2.0, size * 2.0, size * 2.0);
			glutSolidOctahedron();
			break;
		}
	}

	// This method increments the age of the particle.
	void increment_age(float time) {
		age += time;
	} // increment_age

	/* This method determines whether the particle is expired or not.
	 * If it has expired, then it will eventually be processed and removed
	 * from the particles vector, effectively destroying it.
	 * A particle will only last 12 seconds before it is destroyed.
	 */
	bool expired() {
		return (age > 12.0);
	} // expired

	/*This method checks if the particle is inside area of the hole.
	*/
	bool checkHole() {
		if (((x + vx >= -12) && (x + vx <= 12)) && ((z + vz >= 12) && (z + vz <= 22))) {
			return true; //particle is inside the hole area
		}
		else {
			return false;// particle is not inside the area of the hole
		}
	}// checkHole()

	/* This method allows the particle to bounce if it meets the conditions.
	 * The conditions being, that the particle can only bounce 3 times,
	 * and in order to bounce it must be within the bounds of the ground.
	 */
	void bounce(float xVal) {
		// make particle stationary on the ground
		if (bounces == 3) {
			vx = 0;
			vy = 0;
			vz = 0;
			randomSpin = false;
			dax = 0;
			day = 0;
			daz = 0;
		}
		if (onGround(xVal) && vy < 0) {
			if (!checkHole()) {
				bounces += 1;
				vy *= friction;// Apply Friction reduction of speed (particle's speed reduced to 60% of its current speed with each bounce)
				//engine->play2D()
				PlaySound(TEXT("mixkit-electronic-retro-block-hit-2185.wav"), NULL, SND_ASYNC);
			}
		}
	} // bounce

	// This method lets us know if the particle bounces on the ground or not.
	bool onGround(float xVal) {
		return (abs(x) < xVal && abs(z) < xVal && y <= 2 && y > -1);
	} // onGround

	// This method assigns a random value to the particles speed.
	void setRandomSpeed() {
		float randomNum = ((float)rand() / RAND_MAX) * 0.06;
		speed = randomNum;
	} // setRandomSpeed

	// This method resets the speed of a particle to the default value.
	void setDefaultSpeed() {
		speed = 0.015;
	} // setDefaultSpeed

	// This method determines how the particle will spin.
	void toggleSpin() {
		randomSpin = true;
		dax = ((float)rand() / RAND_MAX) * 0.05;
		day = ((float)rand() / RAND_MAX) * 0.05;
		daz = ((float)rand() / RAND_MAX) * 0.05;
	} // toggleSpin

	// This method determines the shape of the particle.
	void setShape(int val) {
		shape = val;
		originalShape = val;
	} // setShape

	// This method updates the angle for a particle, allowing it to spin.
	void updateAngle() {
		angle += 6;
	} // updateAngle

	// This method applies friction to a particle. 
	void applyFriction() {
		friction = -0.6;
	} // applyFriction

	// This method removes particle friction.
	void removeFriction() {
		friction = -1;
	} // removeFriction

	// This method sets the size of the particle to a random value.
	void setRandomSize() {
		size = ((float)rand() / (float)RAND_MAX) * 3.0f;
	} // setRandomSize

	// This method resets the size of the particle to the default size.
	void setDefaultSize() {
		size = 1.0;
	}//setDefaultSize

	// This method changes the shape of the particle to a random one.
	void changeShape() {
		int r = rand() % 5;
		while (true) { // This ensures that a shape other than the original one is chosen.
			if (r != shape) {
				break;
			}
			r = rand() % 5;
		}
		shape = r;
	} // changeShape

	// This method changes the shape back to its original shape.
	void setOriginalShape() {
		shape = originalShape;
	} // setOriginalShape

	// This method changes the color of the particle to a random color.
	void updateColor(bool val) {
		if (val) {
			r1 = (float)rand() / (float)RAND_MAX;
			r2 = (float)rand() / (float)RAND_MAX;
			r3 = (float)rand() / (float)RAND_MAX;
			randomColor = val;
		}
		else {
			randomColor = val;
		}
	} // updateColor

	// get the particle's ID
	int getID() {
		int i = id;
		return id;
	} // getID

	// set the particle's ID
	void setID(int id) {
		this->id = id;
	} // setID

	// get the particle's x value
	float getX() {
		return x;
	} // getX

	// get the particle's y value
	float getY() {
		return y;
	} // getY

	// get the particle's z value
	float getZ() {
		return z;
	} // getZ

	// get the particle's vx value
	float getVX() {
		return vx;
	} // getVX

	// get the particle's vy value
	float getVY() {
		return vy;
	} // getVY

	// get the particle's vz value
	float getVZ() {
		return vz;
	} // getVZ

	// set the particle's vx value
	void setVX(float vx) {
		this->vx = vx;
	} // setVY

	// set the particle's vy value
	void setVY(float vy) {
		this->vy = vy;
	} // setVY

	// set the particle's vz value
	void setVZ(float vz) {
		this->vz = vz;
	} // setVZ

	// get the particle's age
	float getAge() {
		return age;
	} // getAge

	// get the particle's size
	float getSize() {
		return size;
	} // getSize

}; // Particle