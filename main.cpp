#include <GL/glut.h>
#include <list>
#include <stdio.h>
#include "Vector3.h"
#include "ParticleSystem.h"
#include "calculations.h"
#include <string> // std::to_string // debug

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
 * 17. Wind
 */

Vector3 camPos = { -40, 38, 30 };
Vector3 cannonBase = { -5, 30, -10 }; // particle origin & cannon start position
Cannon cannon = Cannon(
	cannonBase,
	{ cannonBase.x, cannonBase.y - 1, cannonBase.z + 1 },
	{ cannonBase.x, cannonBase.y - 1, cannonBase.z + 1 },
	0.9);
ParticleSystem particleSystem;

int flowRate = 4;
bool flowEnabled = true;

// Particle cam
bool particleCamEnabled = false;
Particle* particleCam;
int particleCamAge;

// time
int totalTime = 0;
int gdeltaTime; // debug


void drawSentence(const char* line, float startX, float startY) // debug
{
	glRasterPos2f(startX, startY);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < std::strlen(line); ++i) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, line[i]);
	}
}

void drawDebug(void) // debug
{
			std::string debugText= "deltatime: " + std::to_string(gdeltaTime);

			for (int i = 0, Y = 20; i < 5; i++, Y += 20) {
				drawSentence(debugText.c_str(), 10, Y);
			}
}

void drawCannon(void)
{
	// I couldn't figure out quaternions to rotate a cylinder/cone in 3 axis
	// so my cannon is just two cubes, marking the shape of the cannon

	glPushMatrix(); // Draw Base
	glColor3f(1, 1, 1);
	glTranslatef (cannon.positionBase.x, cannon.positionBase.y, cannon.positionBase.z);
	glutSolidCube(1.5);
	glPopMatrix();

	glPushMatrix(); // Draw Spout
	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(cannon.direction.x, cannon.direction.y, cannon.direction.z);
	glutSolidCube(2);
	glPopMatrix();
}


void drawParticleSystem(void)
{
	for (auto it = particleSystem.begin(); it != particleSystem.end(); it++) {
		glPushMatrix();
			glColor3fv(it->color.v);
			glTranslatef(it->position.x, it->position.y, it->position.z);
			glRotatef(particleSystem.particleRotationAngle*it->speed, it->spin.x, it->spin.y, it->spin.z);
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
		{ 2, 6, 5, 3 }, // bottom
		{ 1, 2, 6, 7 }, // right
		{ 0, 4, 7, 1 }, // top
		{ 4, 5, 6, 7 }  // front
	};

	float normals[6][3] = {
		{ 0, 0, -1 },
		{ -1, 0, 0 }, // looking at this
		{ 0, -1, 0 },
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 }
	};

	float faceColors[6][3] = {
		{ 0.3, 0.9, 0.5 },
		{ 0.3, 0.9, 0.5 },
		{ 0, 1, 1 },
		{ 0.3, 0.9, 0.5 },
		{ 0, 0, 1 },
		{ 0.3, 0.9, 0.5 }
	};

	for (int i = 0; i < 6; i++) {
		glColor3fv(faceColors[i]);
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			glNormal3fv(normals[i]);
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
		if (particleCam == NULL || particleCam->age < particleCamAge)
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
	drawCannon();
	drawParticleSystem();
	drawDebug(); // debug

	glutSwapBuffers();
}

void update(int value)
{
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = elapsedTime - totalTime;
	totalTime = elapsedTime;
	gdeltaTime = deltaTime; // debug

	if (flowEnabled) {
		particleSystem.createParticle(flowRate, cannon);
	}

	particleSystem.update(deltaTime);

	if (deltaTime <= 300) { // skip frames if really lagging
		glutPostRedisplay();
	}

	glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		flowEnabled = !flowEnabled;
		break;
	case '=':
	case '+':
		flowRate += 1;
		break;
	case '-':
		flowRate -= (flowRate > 1) ? 1 : 0;
		break;

	case 'b':
		particleSystem.createParticle(100, cannon);
		break;
	case 'f':
		particleSystem.hasFriction = !particleSystem.hasFriction;
		break;
	case 'r':
		flowEnabled = false;
		particleSystem.clear();
		break;

	case 'y': // takes the last particle, if it exists
		particleCamEnabled = true;
		particleCamAge = 0;
		particleCam = (particleSystem.end()!=particleSystem.begin()) ? &*--particleSystem.end() : NULL;
		break;

	case 'g':
		particleSystem.hasGravity = !particleSystem.hasGravity;
		break;

	case 'i':
		particleSystem.hasWind = !particleSystem.hasWind;
		break;
	case 'u':
		particleSystem.wind += (particleSystem.wind < 1) ? 0.1 : 0;
		break;
	case 'j':
		particleSystem.wind += (particleSystem.wind > -1) ? -0.1 : 0;
		break;

	case 'w': 
		if (cannon.direction.z > cannon.positionBase.z - 2)
		cannon.direction.z -= 1;
		break;
	case 'a':
		if (cannon.direction.x > cannon.positionBase.x - 2)
		cannon.direction.x -= 1;
		break;
	case 's':
		if (cannon.direction.z < cannon.positionBase.z + 2)
		cannon.direction.z += 1;
		break;
	case 'd':
		if (cannon.direction.x < cannon.positionBase.x + 2)
		cannon.direction.x += 1;
		break;
	case 'c':
		if (cannon.direction.y > cannon.positionBase.y - 2)
			cannon.direction.y -= 1;
		break;
	case 'e':
		if (cannon.direction.y < cannon.positionBase.y + 2)
			cannon.direction.y += 1;
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
		camPos.y -= 10;
		break;
	case GLUT_KEY_RIGHT:
		camPos.y += 10;
		break;
	case GLUT_KEY_DOWN:
		camPos.x -= 10;
		break;
	case GLUT_KEY_UP:
		camPos.x += 10;
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
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
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
	printf(
		"Help: \
		Key        | Function \n\
		--------------------------------\n\
		Arrow Keys | Camera position \n\
		w,a,s,d    | Cannon angle left/right \n\
		e,c        | Cannon angle up/down \n\
		space      | Toggle particle flow \n\
		+,-        | +/- flow rate \n\
		f          | Toggle Friction \n\
		g          | Toggle gravity \n\
		i          | Toggle Wind \n\
		u,j        | +/- Wind Power \n\
		r          | Clear particles \n\
		y          | Particle Cam \n\
		b          | Burst 100 particles \n\
		\n\n\
		The two floating cubes denote the angle of the cannon. \n\
		");

	#ifdef _WIN32
		system("PAUSE"); // "Press any key to continue"
	#endif

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