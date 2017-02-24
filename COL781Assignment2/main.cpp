#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
using namespace std;

static int angle = 0;

void drawHand() {
	glRotatef((GLfloat)angle, 1.0f, 0.0f, 0.0f);
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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	/*glPushMatrix();

	// Rotate the scene so we can see the tops of the shapes.
	glRotatef(-20.0, 1.0, 0.0, 0.0);

	// Make a torus floating 0.5 above the x-z plane.  The standard torus in
	// the GLUT library is, perhaps surprisingly, a stack of circles which
	// encircle the z-axis, so we need to rotate it 90 degrees about x to
	// get it the way we want.
	glPushMatrix();
	glTranslatef(-0.75, 0.5, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutSolidTorus(0.275, 0.85, 16, 40);
	glPopMatrix();

	// Make a cone.  The standard cone "points" along z; we want it pointing
	// along y, hence the 270 degree rotation about x.
	glPushMatrix();
	glTranslatef(-0.75, -0.5, 0.0);
	glRotatef(270.0, 1.0, 0.0, 0.0);
	glutSolidCone(1.0, 2.0, 70, 12);
	glPopMatrix();

	// Add a sphere to the scene.
	glPushMatrix();
	glTranslatef(0.75, 0.0, -1.0);
	glutSolidSphere(1.0, 30, 30);
	glPopMatrix();

	glPopMatrix();*/
	glPushMatrix();
	drawHand();
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
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's':
		angle = (angle + 5) % 360;
		glutPostRedisplay();
		break;
	case 'S':
		angle = (angle - 5) % 360;
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