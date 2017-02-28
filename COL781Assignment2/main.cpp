#include <GL/glew.h>//This header file also includes the correct OpenGL headers like GL/gl.h
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <Windows.h>
#include <soil.h>
using namespace std;

//MACROS
#define START_FRAME 0
#define END_FRAME 30
#define epsilon 0.001
#define FPS 30.0f
#define TOTAL_FRAMES 10.0f
//Total frames to be done in the linear interpolation during the interval between consecutive key-frames

//structure for parameters of the different joints
struct model_parameters {
	float hand_rotation;
};

vector<model_parameters> keyFrames;

//parameters for rotation of arm
static float hand = 45.0f; //for rotation w.r.t. the hand joint
static int elbow = -45;//for rotation w.r.t. the elbow joint  
static int shoulder = 90;//for rotation of the segment from the shoulder joint around x-axis
static int angle = 0;//this should be rotation angle around z-axis

//global variables
int startTime;
int elapsedTime;
int currentFrame;
float t_interpolation;

GLuint textureId; //The id of the texture
GLUquadric *quad; //quadric object to handle textute coordinates for glu primitives

//function declarations
void animate();
void fillKeyFrames();
float lerp(float x, float y, float t);

//for loading texture from an image file and returning the texture id
GLuint loadTexture() {
	GLuint _textureId;
	glGenTextures(1, &_textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, _textureId); //Tell OpenGL which texture to edit

	//Map the image to the texture:-
	int width, height;
	unsigned char* image = SOIL_load_image("tex3.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	cout << width << " " << height << endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	return _textureId; //Returns the id of the texture

}

//for drawing hand of frog
void drawHand() {
	glPushMatrix();
		glScalef(1.0f, 2.0f, 1.0f);
		glRotatef((GLfloat)90,0.0f,1.0f,0.0f);
		gluCylinder(quad,0.5f,0.5f, 1.0f, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluSphere(quad,0.5, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(1.0, 0.0, 0.0);
		glScalef(1.0, 2.0, 1.0);
		gluSphere(quad,0.5, 30, 30);
	glPopMatrix();
	//drawing the fingers:-
	for (int theta = -30; theta <= 30; theta += 20) {
		bool flag = (abs(theta) == 10);//flag to differentiate b/w middle longer fingers v/s lateral shorter fingers
		glPushMatrix();
			glRotatef(theta, 0.0, 0.0, 1.0);
			glRotatef(90, 0.0, 1.0, 0.0);
			flag ? gluCylinder(quad, 0.3, 0.0, 3.5, 30, 30) : gluCylinder(quad, 0.4, 0.0, 3.0, 30, 30);
			glPushMatrix();
				flag ? glTranslatef(0.0, 0.0, (GLfloat)2.9) : glTranslatef(0.0, 0.0, (GLfloat)2.4);
				glScalef(1.0, 1.0, 3.0);
				gluSphere(quad, 0.2, 10, 10);
			glPopMatrix();
		glPopMatrix();
	}
}

//basically used for drawing both the arm segments
void drawArmPart() {
	glPushMatrix();
		glRotatef((GLfloat)90, 0.0, 1.0, 0.0);
		gluCylinder(quad,1.0f,1.0f, 3.0f, 30, 30);
		gluSphere(quad,1.0f, 30, 30);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 3.0f);
			gluSphere(quad,1.0f, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

void drawArm() {
	glRotatef((GLfloat)angle, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)shoulder, 1.0, 0.0, 0.0);
	glScalef(0.5f, 0.5f, 0.5f);
	drawArmPart();//drawing the shoulder
	glPushMatrix();
		glTranslatef(3.0, 0.0, 0.0);
		glRotatef((GLfloat)elbow, 0.0, 1.0, 0.0);
		drawArmPart();//drawing the elbow
		glPushMatrix();
			glTranslatef(3.5f, 0.0f, 0.0f);
			glRotatef((GLfloat)hand, 0.0, 0.0, 1.0);
			glRotatef((GLfloat)90, 1.0, 0.0, 0.0);
			drawHand();//drawing the hand
		glPopMatrix();
	glPopMatrix();
}

void display() {
	//clear the current buffer (back buffer)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);//Specifies which matrix stack is the target for subsequent matrix operations.
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluQuadricTexture(quad, 1);

							   
	//drawing the arm
	glPushMatrix();//duplicate the top of the stack
	drawArm();
	glPopMatrix();//remove the top of stack
	
	//swap the front and back buffers.
	glutSwapBuffers();
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
	glEnable(GL_COLOR_MATERIAL);		   //to allow or disallow merging of material color with texture

	quad = gluNewQuadric();				   //initialize the quadric object
	textureId = loadTexture();			   //load texture from file and store its id
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
	/*case 'h':
		hand = (hand + 5) % 360;
		cout << hand << endl;
		glutPostRedisplay();
		break;
	case 'H':
		hand = (hand - 5) % 360;
		glutPostRedisplay();
		break;*/
	case 32://Space bar
		//Animation
		currentFrame = START_FRAME;
		fillKeyFrames();//initialize the keyFrames vector
		glutIdleFunc(animate);//register idle callback
		break;
	case 27://ASCII code for Escape Key
		exit(0);
		break;
	default:
		break;
	}
}


// The usual application statup code.
int main(int argc, char** argv) {
	// init GLUT and create window
	glutInit(&argc, argv);
	//double buffer window, color model - RGB, depth buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(40, 20);// First we establish the window’s position, i.e. its top left corner.
	glutInitWindowSize(1280, 720);//prototype - void glutInitWindowSize(int width, int height)
	//create Window
	glutCreateWindow("Frog Jump");

	//register callbacks
	glutReshapeFunc(reshape);//register reshape callback function
	glutDisplayFunc(display);//register display callback function
	glutKeyboardUpFunc(keyboard);//register keyboard press callback function

	//initializing the light sources and enabling the hidden surface removal
	init();

	cout << "Press Space Bar for animation" << endl;
	// enter GLUT event processing cycle
	glutMainLoop();//enter the event loop

	return 0;//unreachable return statement :P
}

void animate() {
	elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	t_interpolation = ((elapsedTime - startTime)*(FPS))/(1000.0f * TOTAL_FRAMES);
	int next_frame = currentFrame + 1;
	
	hand = lerp(keyFrames[currentFrame].hand_rotation, keyFrames[next_frame].hand_rotation,t_interpolation);
	
	glutPostRedisplay();
	
	if (t_interpolation > 1.0f - epsilon) {
		currentFrame++;
		if (currentFrame == END_FRAME)
			glutIdleFunc(NULL);
		startTime = glutGet(GLUT_ELAPSED_TIME);
	}
}

void fillKeyFrames() {
	keyFrames.clear();
	keyFrames.push_back(model_parameters());
	keyFrames[START_FRAME].hand_rotation = hand;
	for (int i = 1; i <= END_FRAME; i++) {
		keyFrames.push_back(model_parameters());
		keyFrames[i].hand_rotation = keyFrames[i-1].hand_rotation - 3.0f;
	}
}

float lerp(float x, float y, float t) {
	if (t > 1 - epsilon)
		t = 1.0f;
	return x + t*(y - x);
}
