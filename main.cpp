#include <GL/glut.h>
#include <list>
#include <stdio.h>
#include "Vector3.h"
#include "ParticleSystem.h"
#include "calculations.h"
#include <string> // std::to_string

/* Features
 * 1. Particle lifespan
 * 2. Particle spin
 * 3. Particle floor collision
 * 4. Friction mode
 * 5. Scene rotation
 * 6. Start/Stop Simulation
 * 7. Reset key
 * 8. Backface culling, double buffering, perspective projection
 * 12. Particle cam
 * 14. Lighting (no normal vectors)
 */

Vector3 particlePos = { -5, 30, -10 }; // particle origin
Vector3 cannonDirection = { particlePos.x, particlePos.y - 1, particlePos.z + 1 };
Vector3 camPos = { -25, 38, 30 };
ParticleSystem particleSystem;

int flowRate = 100;
bool flowEnabled = true;

// Particle cam
bool particleCamEnabled;
Particle* particleCam;
int particleCamAge;

// time
int totalTime = 0;
int leftoverTime = 0;
int timePerFrame = 100; // 10fps
int gdeltaTime; // debug


void drawSentence(const char* line, float startX, float startY)
{
	glRasterPos2f(startX, startY);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < std::strlen(line); ++i) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, line[i]);
	}
}

void drawDebug(void)
{
	glPushMatrix();
		glTranslatef(particlePos.x, particlePos.y, particlePos.z);
		glutSolidCube(2);
		glPopMatrix();

	glPushMatrix();
		glTranslatef(cannonDirection.x, cannonDirection.y, cannonDirection.z);
		glutSolidCube(2);
		glPopMatrix();

			std::string debugText= "deltatime: " + std::to_string(gdeltaTime);

			for (int i = 0, Y = 20; i < 5; i++, Y += 20) {
				drawSentence(debugText.c_str(), 10, Y);
			}
}

void createParticle(void)
{
	float variance = 0.9;
	Particle p = Particle();
	p.size = randFloat(0.01, 1);
	p.speed = randFloat(0.5, 2);
	p.position = particlePos;
	p.position.x += randFloat(-0.1, 0.1);
	p.position.z += randFloat(-0.1, 0.1);
	p.direction = particlePos.directionTo(
		{ cannonDirection.x * randFloat(-variance, variance), 
		  cannonDirection.y,
		  cannonDirection.z + randFloat(-variance, variance) }
		);
	p.velocity = p.direction;
	p.velocity.scale(p.size * p.speed); // bigger shapes bounce farther
	p.color.randomize();
	p.spin.randomize();
	p.shape = (randFloat(0, 1) < 0.50) ? Particle::Sphere : (randFloat(0, 1) < 0.80) ?
				Particle::Cube : Particle::Teapot; // 10% chance of teapot
	particleSystem.append(p); // Add to particle list
}

void drawParticleSystem(void)
{
	for (auto it = particleSystem.begin(); it != particleSystem.end(); it++) {
		glPushMatrix();
			glColor3fv(it->color.v);
			glTranslatef(it->position.x, it->position.y, it->position.z);
			glRotatef(particleSystem.particleRotationAngle, it->spin.x, it->spin.y, it->spin.z);
			if (it->shape == Particle::Cube)
				glutSolidCube(it->size);
			else if (it->shape == Particle::Teapot)
				glutSolidTeapot(it->size);
			else
				glutSolidSphere(it->size, 16, 16);
		glPopMatrix();
	}
}

/* Draws a box centered at a given position.
*     0 - 1
*   /   / |
*  4 -37  2
*  |   | /
*  5 - 6
*/
void drawBox(Vector3 center, Vector3 size) // note: add a tilt option
{
	float vertices[8][3] = {
		{ center.x - size.x / 2, center.y + size.y / 2, center.z + size.z / 2 }, // back LT
		{ center.x + size.x / 2, center.y + size.y / 2, center.z + size.z / 2 }, // back RT
		{ center.x + size.x / 2, center.y - size.y / 2, center.z + size.z / 2 }, // back RB
		{ center.x - size.x / 2, center.y - size.y / 2, center.z + size.z / 2 }, // back LB
		{ center.x - size.x / 2, center.y + size.y / 2, center.z - size.z / 2 }, // front LT
		{ center.x - size.x / 2, center.y - size.y / 2, center.z - size.z / 2 }, // front LB
		{ center.x + size.x / 2, center.y - size.y / 2, center.z - size.z / 2 }, // front RB
		{ center.x + size.x / 2, center.y + size.y / 2, center.z - size.z / 2 }  // front RT
	};

	int faces[6][4] = {
		{ 1, 2, 3, 0 }, // back
		{ 0, 3, 5, 4 }, // left
		{ 2, 3, 5, 6 }, // bottom
		{ 7, 6, 2, 1 }, // right
		{ 0, 4, 7, 1 }, // top
		{ 4, 5, 6, 7 }  // front
	};

	float faceColors[6][3] = {
		{ 0.3, 0.9, 0.5 },
		{ 0.3, 0.9, 0.5 },
		{ 0, 0, 1 },
		{ 0.3, 0.9, 0.5 },
		{ 0, 0, 1 },
		{ 0.3, 0.9, 0.5 }
	};

	for (int i = 0; i < 6; i++) {
		glColor3fv(faceColors[i]);
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			glVertex3fv(vertices[faces[i][j]]);
		}
		glEnd();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (particleCamEnabled) {
		if (particleCam->age < particleCamAge)
			particleCamEnabled = false;
		else
			gluLookAt(particleCam->position.x, particleCam->position.y, 
			particleCam->position.z, particleCam->position.x + particleCam->direction.x, 
			particleCam->position.y + particleCam->direction.y, 
			particleCam->position.z + particleCam->direction.z, 0, 1, 0);
	}
	else {
		gluLookAt(camPos.x, camPos.y, camPos.z, 0, 0, 0, 0, 1, 0);
	}
	glColor3f(1, 1, 1);

	drawBox({ 0, -1, 0 }, { 100, 2, 100 });
	drawParticleSystem();
	drawDebug();

	glutSwapBuffers();
}

void update(int value)
{
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = elapsedTime - totalTime;
	totalTime = elapsedTime;
	gdeltaTime = deltaTime; // temp

	if (flowEnabled) {
		createParticle();
		createParticle();
		createParticle();
		createParticle();
	}

	particleSystem.update(deltaTime);

	leftoverTime += deltaTime;
	if (deltaTime <= 300) {
		glutPostRedisplay();
		leftoverTime -= glutGet(GLUT_ELAPSED_TIME) - totalTime;
	}

	glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		flowEnabled = !flowEnabled;
		break;

	case 'e':
		for (int i = 0; i < 100; i++) createParticle();
		break;
	case 'f':
		particleSystem.hasFriction = !particleSystem.hasFriction;
		break;
	case 'r':
		flowEnabled = false;
		particleSystem.clear();
		break;

	case 'c': // takes the last particle
		particleCamEnabled = true;
		particleCamAge = 0;
		particleCam = &*--particleSystem.end();
		break;

	case 'g':
		particleSystem.hasGravity = !particleSystem.hasGravity;
		break;

	case 'w': 
		if (cannonDirection.z > particlePos.z - 1)
		cannonDirection.z -= 1;
		break;
	case 'a':
		if (cannonDirection.x > particlePos.x - 1)
		cannonDirection.x -= 1;
		break;
	case 's':
		if (cannonDirection.z < particlePos.z + 1)
		cannonDirection.z += 1;
		break;
	case 'd':
		if (cannonDirection.x < particlePos.x + 1)
		cannonDirection.x += 1;
		break;

	case 'q':
		exit(0);
		break;
	}
}

void special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		camPos.y -= 1;
		break;
	case GLUT_KEY_RIGHT:
		camPos.y += 1;
		break;
	case GLUT_KEY_DOWN:
		camPos.x -= 1;
		break;
	case GLUT_KEY_UP:
		camPos.x += 1;
		break;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 1, 500);
}


void init(void)
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 0);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float position[4] = {30, 5, 0, 0};

	float amb[4] = { 1, 1, 1, 1 };
	float diff[4] = { 1, 1, 1, 1 };
	float spec[4] = { 1, 1, 1, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	
	glEnable(GL_COLOR_MATERIAL);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Particle System");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(16, update, 0);
	init();

	glutMainLoop();
	return(0);
}