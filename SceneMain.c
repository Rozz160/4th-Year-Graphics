#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_CLAMP_TO_EDGE 0x812F
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glut.h>

//Slava Feoktistov - 10336661

void init();
void handleResize(int w, int h);
void initTextures();
void display();
void makeRoom();
void makeCube(float scale_x, float scale_y, float scale_z, float move_x, float move_y, float move_z);
void drawTable();
void drawChair();
void drawDoor();
void drawLamp();
void drawFloorLamp();
void drawClock();
void drawBook();
void keyboardPressed(unsigned char key, int x, int y);
void keyboardUnpressed(unsigned char key, int x, int y);
void mouseMotion(int x, int y);
void timer(int value);
void flipPage();
void clockInc(int value);
void toggleLight();
void moveCamera(float dir);
void setCameraLimit();
void setWorldBounds(float x1, float x2, float z1, float z2);
GLuint loadTexture(const char* name);

double cam_x = 0, cam_y = 3, cam_z = 4;
double cam_pitch = -20, cam_yaw = 0;
int keys[256];
int bookPage[4] = {0,0,0,0};
int pageToggleLeft = 0, pageToggleRight = 0;
GLuint textures[21];
int lights = 0, toggle = 0;
double seconds = 0, minutes = 0, hours = 0;

GLfloat lightPos[] = {0.0f, 4.5f, 0.0f, 1.0f};
GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat diffuse[] = {1.0f, 0.95f, 0.87f ,1.0f};
GLfloat diffuseOff[] = {0.0f, 0.0f, 0.0f ,0.1f};
 
int main(int argc, char** argv){
	init(argc, argv);

	//Display
	glutDisplayFunc(display);
	glutReshapeFunc(handleResize);
	
	//Input
	glutKeyboardFunc(keyboardPressed);
	glutKeyboardUpFunc(keyboardUnpressed);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mouseMotion);
	
	glutTimerFunc(10, timer, 0);
	glutTimerFunc(1000, clockInc, 0);
	glutMainLoop();
	
	glDeleteTextures(21, &textures[0]);
	return 0;
}

void init(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(500,300);
	glutInitWindowSize(800,600);
	glutCreateWindow("Graphics");
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	
	glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	initTextures();
}

void handleResize(int w, int h) {
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45,ratio,0.001,1000);
	glMatrixMode(GL_MODELVIEW);
}

//DISPLAY-----------------------------------------------

void initTextures() {
	textures[0] = loadTexture("textures/wall1.bmp");
	textures[1] = loadTexture("textures/ceiling2.bmp");
	textures[2] = loadTexture("textures/floor2.bmp");
	textures[3] = loadTexture("textures/table.bmp");
	textures[4] = loadTexture("textures/chair.bmp");
	textures[5] = loadTexture("textures/door.bmp");
	textures[6] = loadTexture("textures/lamp.bmp");
	textures[7] = loadTexture("textures/lampwood.bmp");
	textures[8] = loadTexture("textures/lampcloth.bmp");
	textures[9] = loadTexture("textures/clock.bmp");
	textures[10] = loadTexture("textures/arrow.bmp");
	textures[11] = loadTexture("book/side.bmp");
	textures[12] = loadTexture("book/frontside.bmp");
	textures[13] = loadTexture("book/cover1.bmp");
	textures[14] = loadTexture("book/cover2.bmp");
	textures[15] = loadTexture("book/page1.bmp");
	textures[16] = loadTexture("book/page2.bmp");
	textures[17] = loadTexture("book/page3.bmp");
	textures[18] = loadTexture("book/page4.bmp");
	textures[19] = loadTexture("book/page5.bmp");
	textures[20] = loadTexture("book/page6.bmp");
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glRotatef(-cam_pitch, 1.0, 0.0, 0.0 );
	glRotatef(-cam_yaw, 0.0, 1.0, 0.0 );
	glTranslatef(-cam_x, -cam_y, -cam_z);

	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	makeRoom();
	drawTable();
	drawDoor();
	drawLamp();
	drawClock();
	drawBook();
	glPushMatrix();
		glTranslatef(4.2,0,4.2);
		drawFloorLamp();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(2.5,0,0);
		drawChair();
	glPopMatrix();
	glPushMatrix();
		glRotatef(180,0,1,0);
		glTranslatef(2.5,0,0);
		drawChair();
	glPopMatrix();
	glPushMatrix();
		glRotatef(90,0,1,0);
		glTranslatef(2,0,-1);
		drawChair();
	glPopMatrix();
	glPushMatrix();
		glRotatef(90,0,1,0);
		glTranslatef(2,0,1);
		drawChair();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(1,1.85,0);
		glutSolidTeapot(0.2);
	glPopMatrix();
	
	glFlush();
	glutSwapBuffers();
}

void makeRoom() {
	glEnable(GL_TEXTURE_2D);
	
	float difamb[] = {0.75,0.75,0.75,1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, difamb);
	
	//front
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_POLYGON);
		glNormal3f(0.0,0.0,1.0);
		glTexCoord2f(0,0);
		glVertex3f(5, 0, -5);
		glTexCoord2f(0,1);
		glVertex3f(5, 5, -5);
		glTexCoord2f(1,1);
		glVertex3f(-5, 5, -5);
		glTexCoord2f(1,0);
		glVertex3f(-5, 0, -5);
	glEnd();
	
	//back
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_POLYGON);
		glNormal3f(0.0,0.0,-1.0);
		glTexCoord2f(0,0);
		glVertex3f(5, 5, 5);
		glTexCoord2f(0,1);
		glVertex3f(5, 0, 5);
		glTexCoord2f(1,1);
		glVertex3f(-5, 0, 5);
		glTexCoord2f(1,0);
		glVertex3f(-5, 5, 5);
	glEnd();
	
	//left
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_POLYGON);
		glNormal3f(1.0,0.0,0.0);
		glTexCoord2f(0,0);
		glVertex3f(-5, 5, 5);
		glTexCoord2f(0,1);
		glVertex3f(-5, 0, 5);
		glTexCoord2f(1,1);
		glVertex3f(-5, 0, -5);
		glTexCoord2f(1,0);
		glVertex3f(-5, 5, -5);
	glEnd();
	
	//right
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_POLYGON);
		glNormal3f(-1.0,0.0,0.0);
		glTexCoord2f(0,0);
		glVertex3f(5, 5, -5);
		glTexCoord2f(0,1);
		glVertex3f(5, 0, -5);
		glTexCoord2f(1,1);
		glVertex3f(5, 0, 5);
		glTexCoord2f(1,0);
		glVertex3f(5, 5, 5);
	glEnd();
	
	//Ceiling
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_POLYGON);
		glNormal3f(0.0,-1.0,0.0);
		glTexCoord2f(0,0);
		glVertex3f(5, 5, -5);
		glTexCoord2f(1,0);
		glVertex3f(5, 5, 5);
		glTexCoord2f(1,1);
		glVertex3f(-5, 5, 5);
		glTexCoord2f(0,1);
		glVertex3f(-5, 5, -5);
	glEnd();
	
	//floor
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glBegin(GL_POLYGON);
		glNormal3f(0.0,1.0,0.0);
		glTexCoord2f(0,0);
		glVertex3f(5, 0, -5);
		glTexCoord2f(1,0);
		glVertex3f(5, 0, 5);
		glTexCoord2f(1,1);
		glVertex3f(-5, 0, 5);
		glTexCoord2f(0,1);
		glVertex3f(-5, 0, -5);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void makeCube(float scale_x, float scale_y, float scale_z, float move_x, float move_y, float move_z) {
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glTranslatef(move_x,move_y,move_z);
		glScalef(scale_x,scale_y,scale_z);
		
		float difamb[] = {0.5,0.5,0.5,1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, difamb);
		//front
		glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,-1.0);
			glTexCoord2f(0,0);
			glVertex3f(1, -1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, 1, -1);
			glTexCoord2f(1,1);
			glVertex3f(-1, 1, -1);
			glTexCoord2f(1,0);
			glVertex3f(-1, -1, -1);
		glEnd();
		
		//back
		glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, 1);
			glTexCoord2f(0,1);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(1,0);
			glVertex3f(-1, 1, 1);
		glEnd();
		
		//left
		glBegin(GL_POLYGON);
			glNormal3f(-1.0,0.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(-1, 1, 1);
			glTexCoord2f(0,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, -1);
			glTexCoord2f(1,0);
			glVertex3f(-1, 1, -1);
		glEnd();
		
		//right
		glBegin(GL_POLYGON);
			glNormal3f(1.0,0.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, -1, -1);
			glTexCoord2f(1,1);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,0);
			glVertex3f(1, 1, 1);
		glEnd();
		
		//Ceiling
		glBegin(GL_POLYGON);
			glNormal3f(0.0,1.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, -1);
			glTexCoord2f(1,0);
			glVertex3f(1, 1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, 1, 1);
			glTexCoord2f(0,1);
			glVertex3f(-1, 1, -1);
		glEnd();
		
		//floor
		glBegin(GL_POLYGON);
			glNormal3f(0.0,-1.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, -1, -1);
			glTexCoord2f(1,0);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(0,1);
			glVertex3f(-1, -1, -1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void tableLegs() {
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	makeCube(0.1,0.9,0.1,2,0.7,1.25);
	makeCube(0.1,0.9,0.1,-2,0.7,1.25);
	makeCube(0.1,0.9,0.1,2,0.7,-1.25);
	makeCube(0.1,0.9,0.1,-2,0.7,-1.25);
}

void tableTop() {
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	makeCube(2.25,0.1,1.75,0,1.6,0);
}

void drawTable() {
	tableTop();
	tableLegs();
}

void chairLegs() {
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	makeCube(0.07,0.5,0.07,0.5,0.5,0.5);
	makeCube(0.07,0.5,0.07,-0.5,0.5,0.5);
	makeCube(0.07,0.5,0.07,0.5,0.5,-0.5);
	makeCube(0.07,0.5,0.07,-0.5,0.5,-0.5);
}

void chairSeat() {
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	makeCube(0.6,0.07,0.6,0,0.95,0);
}

void chairBack() {
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	makeCube(0.07,0.85,0.07,0.5,1.85,0.5);
	makeCube(0.07,0.85,0.07,0.5,1.85,-0.5);
	makeCube(0.04,0.1,0.5,0.5,2.55,0);
	makeCube(0.04,0.1,0.5,0.5,2.1,0);
	makeCube(0.04,0.1,0.5,0.5,1.4,0);
}

void drawChair(){
	chairLegs();
	chairSeat();
	chairBack();
}

void drawDoor() {
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	makeCube(1.95,2.25,0.02,0,2.25,5);
}

void drawLamp() {
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	makeCube(0.5,0.05,0.5,0,4.95,0);
}

void lampBase() {
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	makeCube(0.5,0.05,0.5,0,0.05,0);
	makeCube(0.05,1.5,0.05,0,1.55,0);
}

void lampShade() {
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	makeCube(0.4,0.5,0.4,0,2.45,0);
}

void drawFloorLamp() {
	lampBase();
	lampShade();
}

void clockBase() {
	glBindTexture(GL_TEXTURE_2D, textures[9]);
	makeCube(0.7,0.7,0.02,0,3.5,-5);
}

void clockSecond() {
	double r;
	if(seconds == 0)
		r = 0;
	else
		r = (360/60) * seconds;
		
	glBindTexture(GL_TEXTURE_2D,textures[10]);
	glPushMatrix();
		glTranslatef(0,3.5,-4.98);
		glRotatef(r, 0.0, 0.0, -1.0);
		glTranslatef(0,0.3,0);
		makeCube(0.005,0.3,0.005,0,0,0);
	glPopMatrix();
}

void clockMinute() {
	double r;
	if(minutes == 0)
		r = 0;
	else
		r = (360/60) * minutes;
		
	glBindTexture(GL_TEXTURE_2D,textures[10]);
	glPushMatrix();
		glTranslatef(0,3.5,-4.97);
		glRotatef(r, 0.0, 0.0, -1.0);
		glTranslatef(0,0.27,0);
		makeCube(0.010,0.27,0.005,0,0,0);
	glPopMatrix();
}

void clockHour() {
	double r;
	if(hours == 0)
		r = 0;
	else
		r = (360/12) * hours;
		
	glBindTexture(GL_TEXTURE_2D,textures[10]);
	glPushMatrix();
		glTranslatef(0,3.5,-4.96);
		glRotatef(r, 0.0, 0.0, -1.0);
		glTranslatef(0,0.24,0);
		makeCube(0.012,0.24,0.005,0,0,0);
	glPopMatrix();
}

void clockArrows() {
	clockSecond();
	clockMinute();
	clockHour();
}

void drawClock() {
	clockBase();
	clockArrows();
}

void makeBook(GLuint page1, GLuint page2) {
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		
		float difamb[] = {0.5,0.5,0.5,1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, difamb);
		//front
		glBindTexture(GL_TEXTURE_2D, textures[11]);
		glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,-1.0);
			glTexCoord2f(0,0);
			glVertex3f(1, -1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, 1, -1);
			glTexCoord2f(1,1);
			glVertex3f(-1, 1, -1);
			glTexCoord2f(1,0);
			glVertex3f(-1, -1, -1);
		glEnd();
		
		//back
		glBindTexture(GL_TEXTURE_2D, textures[11]);
		glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, 1);
			glTexCoord2f(0,1);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(1,0);
			glVertex3f(-1, 1, 1);
		glEnd();
		
		//left
		glBindTexture(GL_TEXTURE_2D, textures[14]);
		glBegin(GL_POLYGON);
			glNormal3f(-1.0,0.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(-1, 1, 1);
			glTexCoord2f(0,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, -1);
			glTexCoord2f(1,0);
			glVertex3f(-1, 1, -1);
		glEnd();
		
		//right
		glBindTexture(GL_TEXTURE_2D, textures[12]);
		glBegin(GL_POLYGON);
			glNormal3f(1.0,0.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, -1, -1);
			glTexCoord2f(1,1);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,0);
			glVertex3f(1, 1, 1);
		glEnd();
		
		//Ceiling
		glBindTexture(GL_TEXTURE_2D, page1);
		glBegin(GL_POLYGON);
			glNormal3f(0.0,1.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, 1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, 1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, 1, 1);
			glTexCoord2f(1,0);
			glVertex3f(-1, 1, -1);
		glEnd();
		
		//floor
		glBindTexture(GL_TEXTURE_2D, page2);
		glBegin(GL_POLYGON);
			glNormal3f(0.0,-1.0,0.0);
			glTexCoord2f(0,0);
			glVertex3f(1, -1, -1);
			glTexCoord2f(0,1);
			glVertex3f(1, -1, 1);
			glTexCoord2f(1,1);
			glVertex3f(-1, -1, 1);
			glTexCoord2f(1,0);
			glVertex3f(-1, -1, -1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void bookSide1() {
	double x = 0;
	if(bookPage[0] == 180)
		x = 1.9;
	else if(bookPage[0] == 0)
		x = 1.93;
		
	glPushMatrix();
		glTranslatef(0,x,1);
		glRotatef(bookPage[0],0,0,1);
		glTranslatef(0.24,0,0);
		glScalef(0.24, 0.01, 0.35);
		
		makeBook(textures[13], textures[15]);
	glPopMatrix();
}

void bookSide2() {
	double x = 0;
	if(bookPage[1] == 180)
		x = 1.91;
	else if(bookPage[1] == 0)
		x = 1.92;
		
	glPushMatrix();
		glTranslatef(0,x,1);
		glRotatef(bookPage[1],0,0,1);
		glTranslatef(0.24,0,0);
		glScalef(0.24, 0.01, 0.35);
		
		makeBook(textures[16], textures[17]);
	glPopMatrix();
}

void bookSide3() {
	double x = 0;
	if(bookPage[2] == 180)
		x = 1.92;
	else if(bookPage[2] == 0)
		x = 1.91;
		
	glPushMatrix();
		glTranslatef(0,x,1);
		glRotatef(bookPage[2],0,0,1);
		glTranslatef(0.24,0,0);
		glScalef(0.24, 0.01, 0.35);
		
		makeBook(textures[18], textures[19]);
	glPopMatrix();
}

void bookSide4() {
	double x = 0;
	if(bookPage[3] == 180)
		x = 1.93;
	else if(bookPage[3] == 0)
		x = 1.9;
		
	glPushMatrix();
		glTranslatef(0,x,1);
		glRotatef(bookPage[3],0,0,1);
		glTranslatef(0.24,0,0);
		glScalef(0.24, 0.01, 0.35);
		
		makeBook(textures[20], textures[14]);
	glPopMatrix();
}

void drawBook() {
	bookSide1();
	bookSide2();
	bookSide3();
	bookSide4();
}

//CONTROLS-----------------------------------------------

void keyboardPressed(unsigned char key, int x, int y) {
	if(key == 27) 
        exit(0);
	keys[key] = 1;
}

void keyboardUnpressed(unsigned char key, int x, int y) {
    keys[key] = 0;
}

void mouseMotion(int x, int y) {
	int dx = x - glutGet(GLUT_WINDOW_WIDTH)/2;
	int dy = y - glutGet(GLUT_WINDOW_HEIGHT)/2;

	if(dx || dy) {
		if(dx) 
			cam_yaw -= 0.2*dx;
		if(dy ) {
			cam_pitch -= 0.2*dy;
			setCameraLimit();
		}
		glutPostRedisplay();
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	}
}

void timer(int value) {
	if(keys['w']) 
		moveCamera(0);
	else if(keys['s'])
		moveCamera(180);
		
	if(keys['a'])
		moveCamera(90);
	else if(keys['d'])
		moveCamera(270);
		
	if(keys['q'] && pageToggleLeft == 0) {
		flipPage(-1);
		pageToggleLeft = 1;
	} else if(!keys['q']) 
		pageToggleLeft = 0;
	
	if(keys['e'] && pageToggleRight == 0) {
		flipPage(1);
		pageToggleRight = 1;
	} else if(!keys['e']) 
		pageToggleRight = 0;

	if(keys[32] && toggle == 0) {
		lights = !lights;
		toggle = 1;
	}
	if(!keys[32])
		toggle = 0;
	
	//sets light on/off
	toggleLight();
	
	//World bounds arnt set to 5 due to camera clipping outside the room
	setWorldBounds(-4.95, 4.95, -4.95, 4.95);
	glutTimerFunc(10, timer, 0);
}

void flipPage(int dir) {
	if(dir == -1) {
		int i;
		for (i = 0; i < 4; i++) {
			if(bookPage[i] == 0) {
				bookPage[i] = 180;
				break;
			}
		}
	} else if(dir == 1) {
		int i;
		for (i = 3; i > -1; i--) {
			if(bookPage[i] == 180) {
				bookPage[i] = 0;
				break;
			}
		}
	}
	glutPostRedisplay();
}

void clockInc(int value) {
	time_t now;
    struct tm *tm;

    now = time(0);
    if ((tm = localtime (&now)) == NULL) {
        printf ("Error extracting time stuff\n");
    }
	seconds = tm->tm_sec;
	minutes = tm->tm_min;
	hours = tm->tm_hour;
	
	glutTimerFunc(1000, clockInc, 0);
}

void toggleLight() {
	if(lights == 1) 
		glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseOff);
	else if(lights == 0) 
		glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	
	glutPostRedisplay();
}

void moveCamera(float dir) {
    float rad=(cam_yaw+dir)*(M_PI/180.0);
	cam_x-=sin(rad)*0.05;
	cam_z-=cos(rad)*0.05;
	
	glutPostRedisplay();
}

void setCameraLimit() {
	if(cam_pitch<-90)
		cam_pitch=-90;
	else if(cam_pitch>90)
		cam_pitch=90;
}

void setWorldBounds(float x1, float x2, float z1, float z2) {
	if(cam_x < x1)
		cam_x = x1;
	else if(cam_x > x2)
		cam_x = x2;
		
	if(cam_z < z1)
		cam_z = z1;
	else if(cam_z > z2)
		cam_z = z2;
}

//TEXTURE LOADING------------------------------------------

GLuint loadTexture(const char* name) {
	SDL_Surface* img=IMG_Load(name);
	if(img == NULL)
		return -1;
	
	SDL_PixelFormat form={NULL,32,4,0,0,0,0,8,8,8,8,0xff000000,0x00ff0000,0x0000ff00,0x000000ff,0,255};
	SDL_Surface* img2=SDL_ConvertSurface(img,&form,SDL_SWSURFACE);
	if(img2 == NULL)
		return -1;
		
	GLuint texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img2->w,img2->h,0,GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,img2->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	SDL_FreeSurface(img);
	SDL_FreeSurface(img2);
	return texture;
}
