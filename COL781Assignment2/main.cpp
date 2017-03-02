#include <GL/glew.h>//This header file also includes the correct OpenGL headers like GL/gl.h
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <soil.h>

using namespace std;

//MACROS
#define START_FRAME 0
#define END_FRAME 30
#define epsilon 0.001
#define FPS 30.0f
#define TOTAL_FRAMES 10.0f
//Total frames to be displayed in the linear interpolation performed during the interval between consecutive key-frames

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

//parameters for rotation of leg
static int foot = 60;
static int leg3 = 90;
static int leg2 = -130;
static int leg1X = 90;//constant during jump
static int leg1Y = 0;//constant during jump
static int leg1Z = -20;

//global variables
int startTime;
int elapsedTime;
int currentFrame;
float t_interpolation;

GLuint textureId; //The id of the skin texture
GLuint eyeTextureId; //The id of eye texture
GLUquadric *quad; //quadric object to handle textute coordinates for glu primitives

//function declarations
void animate();
void fillKeyFrames();
float lerp(float x, float y, float t);

//for loading skin and eye textures from image files
void loadTexture() {
	//Eye texture:-
	glGenTextures(1, &eyeTextureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, eyeTextureId); //Tell OpenGL which texture to edit
	int width, height;
	unsigned char* image = SOIL_load_image("eye.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	//Skin Texture:-
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture:-
	image = SOIL_load_image("tex1.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	cout << width << " " << height << endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

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
			glRotatef((GLfloat)theta, 0.0, 0.0, 1.0);
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

//for drawing the top part of leg
void drawLeg1() {
	glPushMatrix();
		glRotatef((GLfloat)90, 0.0, 1.0, 0.0);
		gluCylinder(quad, 1.125f, 1.125f, 6.0f, 30, 30);
		gluSphere(quad, 1.125f, 30, 30);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 6.0f);
			gluSphere(quad, 1.125f, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

//for drawing second tapered part of leg
void drawLeg2() {
	glPushMatrix();
		glRotatef((GLfloat)90, 0.0, 1.0, 0.0);
		gluCylinder(quad, 1.125f, 1.0f, 6.0f, 30, 30);
		gluSphere(quad, 1.125f, 30, 30);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 6.0f);
			gluSphere(quad, 1.0f, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

//for drawing last part of leg
void drawLeg3() {
	glPushMatrix();
		glRotatef((GLfloat)90, 0.0, 1.0, 0.0);
		gluCylinder(quad, 1.0f, 0.75f, 3.0f, 30, 30);
		gluSphere(quad, 1.0f, 30, 30);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 3.0f);
			gluSphere(quad, 0.75f, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

void drawLeg() {
	glRotatef((GLfloat)leg1Y, 0.0f, 1.0f, 0.0f);
	glRotatef((GLfloat)leg1Z, 0.0f, 0.0f, 1.0f);
	glRotatef((GLfloat)leg1X, 1.0f, 0.0f, 0.0f);
	drawLeg1();
	glPushMatrix();
		glTranslatef(6.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)leg2, 0.0f, 1.0f, 0.0f);
		drawLeg2();
		glPushMatrix();
			glTranslatef(6.0f, 0.0f, 0.0f);
			glRotatef((GLfloat)leg3, 0.0f, 1.0f, 0.0f);
			drawLeg3();
			glPushMatrix();//drawing the foot
				glTranslatef(3.0f, 0.0f, 0.0f);
				glRotatef((GLfloat)foot, 0.0f, 1.0f, 0.0f);
				glScalef(2.0f, 0.75f, 1.0f);//foot is represented as an elongated hand
				drawHand();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void drawTorso() {
	glRotatef((GLfloat)leg1Y, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(1.0f, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, eyeTextureId);
	glRotatef((GLfloat)-90, 1.0f, 0.0f, 0.0f);
	gluSphere(quad, 1.0f, 30, 30);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPopMatrix();
	glPushMatrix();
		//drawing the body:-
		glScalef(2.0f, 1.0f, 1.0f);
		glRotatef((GLfloat)180, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)90, 0.0f, 1.0f, 0.0f);
		gluSphere(quad, 1.0f, 10, 30);
	glPopMatrix();


}

void display() {
	//clear the current buffer (back buffer)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);//Specifies which matrix stack is the target for subsequent matrix operations.
	
	glEnable(GL_TEXTURE_2D);
	//Texture properties for eye texture:-
	glBindTexture(GL_TEXTURE_2D, eyeTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Texture properties for skin texture:-
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//To change the model from 
	//RGB = (diffuse + specular)* (base_texture)     and hence max value of each pixel is base_texture 
	//to 
	//RGB = (diffuse * base_texture) + specular      where max value can now increase for specular highlights
	glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	gluQuadricTexture(quad, 1);	//initializing the quadric object

							   
	//drawing the model
	glPushMatrix();//duplicate the top of the stack
	glScalef(0.5f, 0.5f, 0.5f);
	drawTorso();
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
	GLfloat gray[] = { 0.5, .5, 0.5, 1.0 };
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat direction[] = { 1.0, 1.0, 1.0, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gray);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, direction);

	glEnable(GL_LIGHTING);                // so the renderer considers light
	glEnable(GL_LIGHT0);                  // turn LIGHT0 on
	glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
	glEnable(GL_NORMALIZE);

	quad = gluNewQuadric();				   //initialize the quadric object
	loadTexture();			   //load texture from file and store its id
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	/*case 'a':
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
	case 32://Space bar
		//Animation
		currentFrame = START_FRAME;
		fillKeyFrames();//initialize the keyFrames vector
		glutIdleFunc(animate);//register idle callback
		break;*/
	case 'z':
		foot = (foot + 5) % 360;
		cout << "foot: " << foot << endl;
		glutPostRedisplay();
		break;
	case 'Z':
		foot = (foot - 5) % 360;
		cout << "foot: " << foot << endl;
		glutPostRedisplay();
		break;
	case 'x':
		leg3 = (leg3 + 5) % 360;
		cout << "leg3: " << leg3 << endl;
		glutPostRedisplay();
		break;
	case 'X':
		leg3 = (leg3 - 5) % 360;
		cout << "leg3: " << leg3 << endl;
		glutPostRedisplay();
		break;
	case 'c':
		leg2 = (leg2 + 5) % 360;
		cout << "leg2: " << leg2 << endl;
		glutPostRedisplay();
		break;
	case 'C':
		leg2 = (leg2 - 5) % 360;
		cout << "leg2: " << leg2 << endl;
		glutPostRedisplay();
		break;
	case 'v':
		leg1X = (leg1X + 5) % 360;
		cout << "leg1X: " << leg1X << endl;
		glutPostRedisplay();
		break;
	case 'V':
		leg1X = (leg1X - 5) % 360;
		cout << "leg1X: " << leg1X << endl;
		glutPostRedisplay();
		break;
	case 'b':
		leg1Y = (leg1Y + 5) % 360;
		cout << "leg1Y: " << leg1Y << endl;
		glutPostRedisplay();
		break;
	case 'B':
		leg1Y = (leg1Y - 5) % 360;
		cout << "leg1Y: " << leg1Y << endl;
		glutPostRedisplay();
		break;
	case 'n':
		leg1Z = (leg1Z + 5) % 360;
		cout << "leg1Z: " << leg1Z << endl;
		glutPostRedisplay();
		break;
	case 'N':
		leg1Z = (leg1Z - 5) % 360;
		cout << "leg1Z: " << leg1Z << endl;
		glutPostRedisplay();
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

	cout << "Press b for rotation about vertical.\nKey prompts for joints:z,x,c,n" << endl;
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
