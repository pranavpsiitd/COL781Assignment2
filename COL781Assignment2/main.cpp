#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
using namespace std;

static int angle = 35, hand=65, elbow=-45, shoulder=90;

void drawHand() {
	glPushMatrix();
		glScalef(1.0f, 2.0f, 1.0f);
		glRotatef((GLfloat)90,0.0f,1.0f,0.0f);
		glutSolidCylinder(0.5f, 1.0f, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		glutSolidSphere(0.5, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(1.0, 0.0, 0.0);
		glScalef(1.0, 2.0, 1.0);
		glutSolidSphere(0.5, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glRotatef(330, 0.0, 0.0, 1.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glutSolidCone(0.4, 3.0, 30, 30);
		glPushMatrix();
			glTranslatef(0.0, 0.0, (GLfloat)2.4);
			glScalef(1.0, 1.0, 3.0);
			glutSolidSphere(0.2, 10, 10);
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glRotatef(10, 0.0, 0.0, 1.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glutSolidCone(0.3, 3.5, 30, 30);
		glPushMatrix();
			glTranslatef(0.0, 0.0, (GLfloat)2.9);
			glScalef(1.0, 1.0, 3.0);
			glutSolidSphere(0.2, 10, 10);
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glRotatef(350, 0.0, 0.0, 1.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glutSolidCone(0.3, 3.5, 30, 30);
		glPushMatrix();
			glTranslatef(0.0, 0.0, (GLfloat)2.9);
			glScalef(1.0, 1.0, 3.0);
			glutSolidSphere(0.2, 10, 10);
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glRotatef(30, 0.0, 0.0, 1.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glutSolidCone(0.4, 3.0, 30, 30);
		glPushMatrix();
			glTranslatef(0.0, 0.0, (GLfloat)2.4);
			glScalef(1.0, 1.0, 3.0);
			glutSolidSphere(0.2, 10, 10);
		glPopMatrix();
	glPopMatrix();
}

void drawArmPart() {
	glPushMatrix();
		glRotatef((GLfloat)90, 0.0, 1.0, 0.0);
		glutSolidCylinder(1.0f, 3.0f, 30, 30);
		glutSolidSphere(1.0f, 30, 30);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 3.0f);
			glutSolidSphere(1.0f, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

void drawArm() {
	glRotatef((GLfloat)angle, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)shoulder, 1.0, 0.0, 0.0);
	glScalef(0.5f, 0.5f, 0.5f);
	drawArmPart();
	glPushMatrix();
		glTranslatef(3.0, 0.0, 0.0);
		glRotatef((GLfloat)elbow, 0.0, 1.0, 0.0);
		drawArmPart();
		glPushMatrix();
			glTranslatef(3.5f, 0.0f, 0.0f);
			glRotatef((GLfloat)hand, 0.0, 0.0, 1.0);
			glRotatef((GLfloat)90, 1.0, 0.0, 0.0);
			drawHand();
		glPopMatrix();
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	drawArm();
	glPopMatrix();
	glFlush();
}

// We don't want the scene to get distorted when the window size changes, so
// we need a reshape callback.  We'll always maintain a range of -2.5..2.5 in
// the smaller of the width and height for our viewbox, and a range of -10..10
// for the viewbox depth.
void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	GLfloat aspect = GLfloat(w) / GLfloat(h);
	glLoadIdentity();
	if (w <= h) {
		// width is smaller, so stretch out the height
		glOrtho(-2.5, 2.5, -2.5 / aspect, 2.5 / aspect, -10.0, 10.0);
	}
	else {
		// height is smaller, so stretch out the width
		glOrtho(-2.5*aspect, 2.5*aspect, -2.5, 2.5, -10.0, 10.0);
	}
}

// Performs application specific initialization.  It defines lighting
// parameters for light source GL_LIGHT0: black for ambient, yellow for
// diffuse, white for specular, and makes it a directional source
// shining along <-1, -1, -1>.  It also sets a couple material properties
// to make cyan colored objects with a fairly low shininess value.  Lighting
// and depth buffer hidden surface removal are enabled here.
void init() {
	GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
	GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat direction[] = { 1.0, 1.0, 1.0, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, direction);

	glEnable(GL_LIGHTING);                // so the renderer considers light
	glEnable(GL_LIGHT0);                  // turn LIGHT0 on
	glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
	glEnable(GL_NORMALIZE);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		angle = (angle + 5) % 360;
		cout << angle << endl;
		glutPostRedisplay();
		break;
	case 'A':
		angle = (angle - 5) % 360;
		glutPostRedisplay();
		break;
	case 's':
		shoulder = (shoulder + 5) % 360;
		cout << shoulder << endl;
		glutPostRedisplay();
		break;
	case 'S':
		shoulder = (shoulder - 5) % 360;
		glutPostRedisplay();
		break;
	case 'e':
		elbow = (elbow + 5) % 360;
		cout << elbow << endl;
		glutPostRedisplay();
		break;
	case 'E':
		elbow = (elbow - 5) % 360;
		glutPostRedisplay();
		break;
	case 'h':
		hand = (hand + 5) % 360;
		cout << hand << endl;
		glutPostRedisplay();
		break;
	case 'H':
		hand = (hand - 5) % 360;
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

// The usual application statup code.
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Cyan Shapes in Yellow Light");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardUpFunc(keyboard);
	init();
	glutMainLoop();
}