#include <GL/glew.h>//This header file also includes the correct OpenGL headers like GL/gl.h
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <soil.h>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

//MACROS
#define START_FRAME 0
int END_FRAME = 0;//Changed we will dynamically decide how many frames to take
#define epsilon 0.001
int FPS = 60.0f;
int TOTAL_FRAMES = 10.0f;
//Total frames to be displayed in the linear interpolation performed during the interval between consecutive key-frames

//structure for parameters of the different joints
struct model_parameters {
	float hand_rotation;
	float elbow_rotation;
	float shoulderZ_rotation;
	float foot_rotation;
	float leg3_rotation;
	float leg2_rotation;
	float leg1Y_rotation;
	float leg1Z_rotation;
	float translateX;
	float translateY;
};

vector<model_parameters> keyFrames;

//parameters for rotation of arm
static int hand = -85; //for rotation w.r.t. the hand joint
static int elbow = -45;//for rotation w.r.t. the elbow joint  
static int shoulderZ = -40;//this should be rotation angle around z-axis

//parameters for rotation of leg
static int foot = 55;
static int leg3 = 100;
static int leg2 = -145;
static int leg1X = 90;//constant during jump
static int leg1Y = -55;
static int leg1Z = -10;

//torso parameters
static int torso = 0;//for rotation of torso
static float posX = 0;//Translate
static float posY = 0;

//multiple jumps
static float posX_initial = -25.0f;
//static int multipleJumps = 0;
bool increasePosX = false;

//global variables
int startTime;
int elapsedTime;
int prevTime;
int currentFrame;
float t_interpolation;


GLuint textureId; //The id of the skin texture
GLuint eyeTextureId; //The id of eye texture
GLUquadric *quad; //quadric object to handle textute coordinates for glu primitives

//camera and movement parameters:-
GLint windowWidth = 1280;                    // Width of our window
GLint windowHeight = 720;                    // Height of our window
GLint midWindowX = windowWidth / 2;         // Middle of the window horizontally
GLint midWindowY = windowHeight / 2;         // Middle of the window vertically

// Camera rotation
GLfloat camYRot = 0.0f;

// Camera position
GLfloat camXPos = 0.0f;
GLfloat camZPos = 5.0f;

// Camera movement speed
GLfloat camXSpeed = 0.0f;
GLfloat camZSpeed = 0.0f;
GLfloat movementSpeedFactor = 0.2f;// How fast we move (higher values mean we move and strafe faster)

// Hoding any keys down?
bool holdingForward = false;
bool holdingBackward = false;
bool holdingLeftStrafe = false;
bool holdingRightStrafe = false;
//cursor position:-
int xpos, ypos;



//function declarations
void animate();
void fillKeyFrames();
float lerp(float x, float y, float t);
void readKeyFrames();

// Function to convert degrees to radians
float toRads(const float &theAngleInDegrees)
{
	return theAngleInDegrees * 3.141592654f / 180.0f;
}

// Function to move the camera the amount we've calculated in the calculateCameraMovement function
void moveCamera()
{
	camXPos += camXSpeed;
	camZPos += camZSpeed;
}

void update(int x, int y) {
	xpos = x;
	ypos = y;
}

// Function to deal with mouse position changes, called whenever the mouse cursor moves
void handleMouseMove()
{
	GLfloat mouseSensitivity = 10.0f;
	int movement = xpos - midWindowX;
	camYRot += movement / mouseSensitivity;
	// Looking left and right. Keep the angles in the range -180.0f (anticlockwise turn looking behind) to 180.0f (clockwise turn looking behind)
	if (camYRot < -180.0f)
		camYRot += 360.0f;
	if (camYRot > 180.0f)
		camYRot -= 360.0f;
	// Reset the mouse position to the centre of the window each frame
	glutWarpPointer(midWindowX, midWindowY);
}

// Function to calculate which direction we need to move the camera and by what amount
void calculateCameraMovement()
{
	// Break up our movement into components along the X, Y and Z axis
	float camMovementXComponent = 0.0f;
	float camMovementZComponent = 0.0f;

	if (holdingForward){
		camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot))));
		camMovementZComponent -= (movementSpeedFactor * float(cos(toRads(camYRot))));
	}
	if (holdingBackward){
		camMovementXComponent -= (movementSpeedFactor * float(sin(toRads(camYRot))));
		camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot))));
	}
	if (holdingLeftStrafe){
		float yRotRad = toRads(camYRot);
		camMovementXComponent += -movementSpeedFactor * float(cos(yRotRad));
		camMovementZComponent += -movementSpeedFactor * float(sin(yRotRad));
	}
	if (holdingRightStrafe){
		float yRotRad = toRads(camYRot);
		camMovementXComponent += movementSpeedFactor * float(cos(yRotRad));
		camMovementZComponent += movementSpeedFactor * float(sin(yRotRad));
	}

	camXSpeed = camMovementXComponent;
	camZSpeed = camMovementZComponent;

	// Cap the speeds to our movementSpeedFactor:-
	if (camXSpeed > movementSpeedFactor)
		camXSpeed = movementSpeedFactor;
	if (camXSpeed < -movementSpeedFactor)
		camXSpeed = -movementSpeedFactor;
	if (camZSpeed > movementSpeedFactor)
		camZSpeed = movementSpeedFactor;
	if (camZSpeed < -movementSpeedFactor)
		camZSpeed = -movementSpeedFactor;
}

// Function to set flags according to which keys are pressed or released
void handleKeypressUp(unsigned char theKey, int x, int y)
{
		switch (theKey)
		{
		case 'w':
		case 'W':
			holdingForward = false;
			break;
		case 's':
		case 'S':
			holdingBackward = false;
			break;
		case 'a':
		case 'A':
			holdingLeftStrafe = false;
			break;
		case 'd':
		case 'D':
			holdingRightStrafe = false;
			break;
		case 32://Space bar
				//Animation
			currentFrame = START_FRAME;
			//END_FRAME--;
			//fillKeyFrames();//initialize the keyFrames vector
			readKeyFrames();
			prevTime = glutGet(GLUT_ELAPSED_TIME);
			startTime = glutGet(GLUT_ELAPSED_TIME);
			TOTAL_FRAMES = 10.0f;
			increasePosX = true;
			glutIdleFunc(animate);//register idle callback
			break;
		default:
			break;
		}
}

void handleKeypress(unsigned char theKey, int x, int y) {
	switch (theKey)
	{
	case 'w':
	case 'W':
		holdingForward = true;
		break;
	case 's':
	case 'S':
		holdingBackward = true;
		break;
	case 'a':
	case 'A':
		holdingLeftStrafe = true;
		break;
	case 'd':
	case 'D':
		holdingRightStrafe = true;
		break;
	default:
		break;
	}
}

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

void drawArm(int shoulderX,int shoulderY,int hand) {
	glRotatef((GLfloat)shoulderY, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)shoulderZ, 0.0, 0.0, 1.0);
	glRotatef((GLfloat)shoulderX, 1.0, 0.0, 0.0);
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

void drawLeg(int leg1Y) {
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
	//Drawing the body of the frog:-
	glPushMatrix();
		glScalef(3.0f, 1.5f, 2.0f);
		glRotatef((GLfloat)180, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)90, 0.0f, 1.0f, 0.0f);
		gluSphere(quad, 1.0f, 30, 30);
	glPopMatrix();

	//Drawing the Head of the frog:-
	glPushMatrix();
		glTranslatef(3.5f, -0.5f, 0.0f);
		glPushMatrix();
			//Neck area
			glTranslatef(-0.5f, 0.5f, 0.0f);
			glRotatef((GLfloat)-90, 0.0f, 1.0f, 0.0f);
			glScalef(1.0f, 0.5f, 1.0f);
			gluCylinder(quad, 2.0f, 1.5f, 2.0f, 30, 30);
			glPushMatrix();
				glRotatef((GLfloat)180, 0.0f, 1.0f, 0.0f);
				gluCylinder(quad, 2.0f, 0.0f, 1.0f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			//Top of head
			glTranslatef(0.0f, 0.5f, 0.0f);
			glScalef(2.0f, 1.0f, 2.5f);
			glRotatef((GLfloat)90, 0.0f, 1.0f, 0.0f);
			gluSphere(quad, 1.0f, 3, 30);
		glPopMatrix();
		glPushMatrix();
			//Mouth of frog
			glScalef(2.0f, 0.5f, 2.0f);
			glRotatef((GLfloat)90, 0.0f, 1.0f, 0.0f);
			gluSphere(quad, 1.0f, 30, 30);
		glPopMatrix();
		glPushMatrix();
			//Eye Sockets
			glTranslatef(0.5f, 1.0f, -1.0f);
			gluCylinder(quad, 0.4f, 0.4f, 2.0f, 20, 20);
		glPopMatrix();
		//Eyes of frog:-
		glBindTexture(GL_TEXTURE_2D, eyeTextureId);
		glPushMatrix();
			glTranslatef(0.5f, 1.0f, 1.0f);
			glRotatef((GLfloat)-90, 1.0f, 0.0f, 0.0f);
			gluSphere(quad, 0.4f, 10, 10);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.5f, 1.0f, -1.0f);
			glRotatef((GLfloat)90, 1.0f, 0.0f, 0.0f);
			gluSphere(quad, 0.4f, 10, 10);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, textureId);
	glPopMatrix();
	


}

//To draw the entire frog model:-
void drawModel() {
	glTranslatef(-4.0f, 0.0f, 0.0f);
	glScalef(0.15f, 0.15f, 0.15f);
	//translate
	glTranslatef((GLfloat)(posX + posX_initial),(GLfloat)posY,0.0f);
	//
	glRotatef((GLfloat)torso, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
		glPushMatrix();
			glRotatef((GLfloat)20, 0.0f, 0.0f, 1.0f);
			drawTorso();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.5f, 0.0f, 1.4f);
			drawArm(90,-90,hand);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.5f, 0.0f, -1.4f);
			drawArm(90, 90,-hand);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, -0.5f, 1.0f);
			glScalef(0.4f, 0.4f, 0.4f);
			drawLeg(leg1Y);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, -0.5f, -1.0f);
			glScalef(0.4f, 0.4f, 0.4f);
			drawLeg(-leg1Y);
		glPopMatrix();
	glPopMatrix();
}

void display() {
	handleMouseMove();
	calculateCameraMovement();
	moveCamera();
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

	glLoadIdentity();
	gluLookAt(camXPos,1.0f,camZPos,camXPos+sin(toRads(camYRot)),1.0f,camZPos-cos(toRads(camYRot)),0.0f,1.0f,0.0f);
	//cout << camXPos << " " << camZPos << " " << camXPos + sin(toRads(camYRot)) << " " << camZPos - cos(toRads(camYRot)) << endl;	//drawing the model
	glPushMatrix();//duplicate the top of the stack
	drawModel();
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
		gluPerspective(45.0 / 2.0f, h/ w, 1.0, 1500.0f);
	}
	else {
		// height is smaller, so stretch out the width
		gluPerspective(45.0 / 2.0f, w / h, 1.0, 1500.0f);
	}
}

// Performs application specific initialization.  It defines lighting
// parameters for light source GL_LIGHT0: black for ambient, yellow for
// diffuse, white for specular, and makes it a directional source
// shining along <-1, -1, -1>.  It also sets a couple material properties
// to make cyan colored objects with a fairly low shininess value.  Lighting
// and depth buffer hidden surface removal are enabled here.
void init() {

	glutSetCursor(GLUT_CURSOR_NONE);//hide mouse cursor
	glutWarpPointer(midWindowX, midWindowY);
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
	case 'q':
		shoulderZ = (shoulderZ + 5) % 360;
		cout << shoulderZ << endl;
		glutPostRedisplay();
		break;
	case 'Q':
		shoulderZ = (shoulderZ - 5) % 360;
		cout << shoulderZ << endl;
		glutPostRedisplay();
		break;
	case 'e':
		elbow = (elbow + 5) % 360;
		cout << elbow << endl;
		glutPostRedisplay();
		break;
	case 'E':
		elbow = (elbow - 5) % 360;
		cout << elbow << endl;
		glutPostRedisplay();
		break;
	case 'h':
		hand = (hand + 5) % 360;
		cout << hand << endl;
		glutPostRedisplay();
		break;
	case 'H':
		hand = (hand - 5) % 360;
		cout << hand << endl;
		glutPostRedisplay();
		break;
	case 32://Space bar
		//Animation
		currentFrame = START_FRAME;
		//END_FRAME--;
		//fillKeyFrames();//initialize the keyFrames vector
		readKeyFrames();
		startTime = glutGet(GLUT_ELAPSED_TIME);
		TOTAL_FRAMES = 10.0f;
		//posX_initial = multipleJumps*25.0f;
		glutIdleFunc(animate);//register idle callback
		break;
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
	case 't':
		torso = (torso+ 5) % 360;
		glutPostRedisplay();
		break;
	case 'T':
		torso = (torso - 5) % 360;
		glutPostRedisplay();
		break;
	case 'j' :
		posX += 0.1f;
		glutPostRedisplay();
		break;
	case 'J':
		posX -= 0.1f;
		glutPostRedisplay();
		break;
	case 'k':
		posY += 0.1f;
		glutPostRedisplay();
		break;
	case 'K':
		posY -= 0.1f;
		glutPostRedisplay();
		break;
	case 'l'://Capture the current frame into the keyFrames
		keyFrames.push_back(model_parameters());
		keyFrames[END_FRAME].hand_rotation = hand;
		keyFrames[END_FRAME].elbow_rotation = elbow;
		keyFrames[END_FRAME].shoulderZ_rotation = shoulderZ;
		keyFrames[END_FRAME].foot_rotation = foot;
		keyFrames[END_FRAME].leg1Y_rotation = leg1Y;
		keyFrames[END_FRAME].leg1Z_rotation = leg1Z;
		keyFrames[END_FRAME].leg2_rotation = leg2;
		keyFrames[END_FRAME].leg3_rotation = leg3;
		keyFrames[END_FRAME].translateX = posX;
		keyFrames[END_FRAME].translateY = posY;
		END_FRAME++;//Increment the end frame number.
		break;
	case 'p'://save the parameters in a text file (space separated)
	{
		ofstream myfile("parameters.txt");
		if (myfile.is_open())
		{
			myfile << END_FRAME << endl;
			for (int i = 0; i < END_FRAME; i++) {
				myfile << keyFrames[i].hand_rotation << " "
					<< keyFrames[i].elbow_rotation << " "
					<< keyFrames[i].shoulderZ_rotation << " "
					<< keyFrames[i].foot_rotation << " "
					<< keyFrames[i].leg1Y_rotation << " "
					<< keyFrames[i].leg1Z_rotation << " "
					<< keyFrames[i].leg2_rotation << " "
					<< keyFrames[i].leg3_rotation << " "
					<< keyFrames[i].translateX << " "
					<< keyFrames[i].translateY << endl;
			}
		}
		else cout << "Unable to open file";
		myfile.close();
	}
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
	glutInitWindowSize(windowWidth, windowHeight);//prototype - void glutInitWindowSize(int width, int height)
	//create Window
	glutCreateWindow("Frog Jump");

	//register callbacks
	glutReshapeFunc(reshape);//register reshape callback function
	glutDisplayFunc(display);//register display callback function
	glutKeyboardUpFunc(handleKeypressUp);//register keyboard press callback function
	glutKeyboardFunc(handleKeypress);

	//initializing the light sources and enabling the hidden surface removal
	init();

	cout << "Use q,e,h to move the joints" << endl;
	// enter GLUT event processing cycle
	glutPassiveMotionFunc(update);
	glutMainLoop();//enter the event loop

	return 0;//unreachable return statement :P
}

void animate() {
	if (increasePosX) {
		posX_initial += 25.0f;
		increasePosX = false;
	}
	elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	Sleep(max(0,17-(elapsedTime-prevTime)));
	prevTime = elapsedTime;
	elapsedTime = glutGet(GLUT_ELAPSED_TIME);

	t_interpolation = ((elapsedTime - startTime)*(FPS))/(1000.0f * TOTAL_FRAMES);
	int next_frame = currentFrame + 1;
	//Interpolate all the parameters of the model
	hand = lerp(keyFrames[currentFrame].hand_rotation, keyFrames[next_frame].hand_rotation,t_interpolation);
	elbow = lerp(keyFrames[currentFrame].elbow_rotation, keyFrames[next_frame].elbow_rotation, t_interpolation);
	shoulderZ = lerp(keyFrames[currentFrame].shoulderZ_rotation, keyFrames[next_frame].shoulderZ_rotation, t_interpolation);
	
	foot = lerp(keyFrames[currentFrame].foot_rotation, keyFrames[next_frame].foot_rotation, t_interpolation);
	leg1Y = lerp(keyFrames[currentFrame].leg1Y_rotation, keyFrames[next_frame].leg1Y_rotation, t_interpolation);
	leg1Z = lerp(keyFrames[currentFrame].leg1Z_rotation, keyFrames[next_frame].leg1Z_rotation, t_interpolation);
	leg2 = lerp(keyFrames[currentFrame].leg2_rotation, keyFrames[next_frame].leg2_rotation, t_interpolation);
	leg3 = lerp(keyFrames[currentFrame].leg3_rotation, keyFrames[next_frame].leg3_rotation, t_interpolation);
	
	posX = lerp(keyFrames[currentFrame].translateX, keyFrames[next_frame].translateX, t_interpolation);
	posY = lerp(keyFrames[currentFrame].translateY, keyFrames[next_frame].translateY, t_interpolation);
	
	glutPostRedisplay();
	
	if (t_interpolation > 1.0f - epsilon) {
		currentFrame++;
		if (currentFrame == END_FRAME - 1) {
			glutIdleFunc(NULL);
			increasePosX = true;
			//added code
			currentFrame = START_FRAME;
			//END_FRAME--;
			//fillKeyFrames();//initialize the keyFrames vector
			readKeyFrames();
			startTime = glutGet(GLUT_ELAPSED_TIME);
			TOTAL_FRAMES = 10.0f;
			glutIdleFunc(animate);//register idle callback
			//end
		}
		else if (currentFrame == 7)
			TOTAL_FRAMES = 4.0f;
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

void readKeyFrames() {
	ifstream infile;
	infile.open("parameters.txt");
	keyFrames.clear();

	std::string line;
	//read number of key frames
	getline(infile, line);
	istringstream iss(line);
	iss >> END_FRAME;
	//END_FRAME--;
	for(int i = 0; i < END_FRAME; i++)
	{
		getline(infile,line);
		istringstream iss(line);
		keyFrames.push_back(model_parameters());
		
		iss >> keyFrames[i].hand_rotation
			>> keyFrames[i].elbow_rotation
			>> keyFrames[i].shoulderZ_rotation
			>> keyFrames[i].foot_rotation
			>> keyFrames[i].leg1Y_rotation
			>> keyFrames[i].leg1Z_rotation
			>> keyFrames[i].leg2_rotation
			>> keyFrames[i].leg3_rotation
			>> keyFrames[i].translateX
			>> keyFrames[i].translateY;
	}


	infile.close();
}

float lerp(float x, float y, float t) {
	if (t > 1 - epsilon)
		t = 1.0f;
	return x + t*(y - x);
}
