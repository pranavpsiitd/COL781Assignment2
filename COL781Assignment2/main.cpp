#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>

using namespace std;

void renderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);//clear red

	glutSwapBuffers();
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 500);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL First Window");

	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glutDisplayFunc(renderScene);

	//glutMainLoop();

	cout << glGetString(GL_RENDERER) << endl;
	cout << glewGetString(GLEW_VERSION) << endl;
	cout << glGetString(GL_VERSION) << endl;
	cout << glutGet(GLUT_VERSION) << endl;
	getchar();

	return 0;
}