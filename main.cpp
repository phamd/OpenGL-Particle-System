#include <GL/glut.h>
#include <list>
#include <stdio.h>
#include "containers.h"
#include "particlesystem.h"
#include "calculations.h"

Vector3 particlePos = { -5, 40, -10 }; // particle origin
Vector3 camPos = { -25, 38, 30 };
int totalTime = 0;
ParticleSystem particleSystem;

void createParticle(void) {
	Particle p = Particle();
	p.size = randFloat(0.01, 1);
	p.speed = randFloat(0.1, 3);
	p.position = particlePos;
	p.position.x += randFloat(-0.5, 0.5);
	p.position.z += randFloat(-0.5, 0.5);
	p.direction = particlePos.directionTo(
		{ particlePos.x + randFloat(-0.5, 0.5), 
		  particlePos.y - 20,
		  particlePos.z + randFloat(-0.5, 0.5) });
	p.velocity = p.direction;
	p.velocity.scale(p.speed);
	p.color = randomColor();
	particleSystem.append(p); // Add to particle list
}

void drawParticleSystem(void) {
	for (auto it = particleSystem.begin(); it != particleSystem.end(); it++) {
		glPushMatrix();
		glColor3fv(it->color.v);
		glTranslatef(it->position.x, it->position.y, it->position.z);
		glutSolidCube(it->size);
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
	gluLookAt(camPos.x, camPos.y, camPos.z, 0, 0, 0, 0, 1, 0);
	glColor3f(1, 1, 1);

	drawBox({ 0, -1, 0 }, { 50, 2, 50 });
	drawParticleSystem();

	glutSwapBuffers();
}

void update(int value) {
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = elapsedTime - totalTime;
	totalTime = elapsedTime;

	particleSystem.update(deltaTime);

	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		createParticle();
		break;

	case 'e':
		for (int i = 0; i < 100; i++) createParticle();
		break;

	// note: move these to arrow keys
	case 'w': 
		camPos.z -= 1;
		break;
	case 'a':
		camPos.x -= 1;
		break;
	case 's':
		camPos.z += 1;
		break;
	case 'd':
		camPos.x += 1;
		break;

	case 'q':
		exit(0);
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

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Particle System");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, update, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0); // Background color

	glutMainLoop();
	return(0);
}