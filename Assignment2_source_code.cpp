#define FREEGLUT_STATIC 
#include <GL/freeglut.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include<math.h> 
#include "vector" 
using namespace std;

const double PI = acos(-1.0);

int intWinWidth = 1050;    //Default window width 
int intWinHeight = 750;	   //Default window heigth

float fltBuildingOffset = 900.0;   //Annimation offsets for buildings
float fltOffset = 750.0;

float fltFOV = 70;      //Field Of View 
float fltX0 = 300.0;      //camara position 
float fltY0 = 0.0;
float fltZ0 = 0.0;
float fltXRef = 0.0;     //Look At reference point 
float fltYRef = 0.0;
float fltZRef = 0.0;
float fltXUp = 0.0;     //Up vector 
float fltYUp = 1.0;
float fltZUp = 0.0;
float angle = 0.0;        //rotating angle
int getoff_button = 0;    //control mouse interaction

//variables for texture
GLint imagewidth0, imagewidth1;  //image width for mapping as 'TV' 
GLint imageheight0, imageheight1;  //image height
GLint pixellength0, pixellength1;  //image file length
vector<GLubyte*>p;
GLuint texture[2];
int photo_num = 0;  //setting index for each image

GLint adwidth0, adwidth1, adwidth2;   //image width for mapping as 'advertisement' 
GLint adheight0, adheight1, adheight2;  //image height
GLint adlength0, adlength1, adlength2;   //file length
GLuint ad_texture[3];
int ad_number = 0;                    //setting index for each image

GLint grdwidth, paperwidth, jhwidth, otwallwidth, doorwidth;   //setting image width for walls or floor
GLint grdheight, paperheight, jhheight, otwallheight, doorheight;	//image height
GLint grdlength, paperlength, jhlength, otlength, doorlength;       //image length
GLuint mapping_texture[6];

// Some global points needed to draw the fireworks
//Declare a structure to save x,y.
typedef struct { GLfloat x, y; } point;
point p_right = { 100,400 };  //right
point p_left = { 100,400 };   //left
point p_up = { 100,400 };     //up
point p_down = { 100,400 };   //down
point p_ne = { 100,400 };  //the line pointing to northeast
point p_nw = { 100,400 };  //the line pointing to northwest
point p_se = { 100,400 };  //southeast
point p_sw = { 100,400 };  //southwest

//step for fireworks 
GLfloat step = 10.0;     //length of each line
GLfloat pace = 8;        //moving length of each line

GLfloat ambient_l1[] = { 0.6,0.6,0.5,0.0 };    //ambient lighting setting for light 1 
GLfloat ambient_l2[] = { 0.8,0.8,0.8,1.0 };   //ambient lighting setting for light 2

//draw fireworks
void draw_firework() {
	//line_right
	glBegin(GL_LINES);
	glVertex2f(p_right.x, p_right.y);
	glVertex2f(p_right.x + step, p_right.y);
	glEnd();

	//line_left
	glBegin(GL_LINES);
	glVertex2f(p_left.x, p_left.y);
	glVertex2f(p_left.x - step, p_left.y);
	glEnd();

	//line_up
	glBegin(GL_LINES);
	glVertex2f(p_up.x, p_up.y);
	glVertex2f(p_up.x, p_up.y + step);
	glEnd();

	//line_down
	glBegin(GL_LINES);
	glVertex2f(p_down.x, p_down.y);
	glVertex2f(p_down.x, p_down.y - step);
	glEnd();

	//line_northeast
	glBegin(GL_LINES);
	glVertex2f(p_ne.x, p_ne.y);
	glVertex2f(p_ne.x + step, p_ne.y + step);
	glEnd();

	//line_sortheast
	glBegin(GL_LINES);
	glVertex2f(p_se.x, p_se.y);
	glVertex2f(p_se.x + step, p_se.y - step);
	glEnd();

	//line_northwest
	glBegin(GL_LINES);
	glVertex2f(p_nw.x, p_nw.y);
	glVertex2f(p_nw.x - step, p_nw.y + step);
	glEnd();

	//line_sorthwest
	glBegin(GL_LINES);
	glVertex2f(p_sw.x, p_sw.y);
	glVertex2f(p_sw.x - step, p_sw.y - step);
	glEnd();
}

//read file
void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength) {
	GLubyte* pixeldata;
	FILE* pfile;
	fopen_s(&pfile, path, "rb");
	if (pfile == 0) exit(0);

	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);

	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0)pixellength++;
	pixellength *= imageheight;

	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0) exit(0);

	fseek(pfile, 54, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	p.push_back(pixeldata);  // Similar to glDrawPixels for program 3 in Week 11 
	fclose(pfile);
}

//setting light1, the initial one
void lighting() {
	//setting light1
	GLfloat light_position[] = { 1000,20000,1000 ,0.0 };
	GLfloat light_specular[] = { 1.0,1.0,1.0,1.0 };
	GLfloat light_diffuse[] = { 1.0,1.0,1.0,1.0 };
	GLfloat light_ambient[] = { 0.6,0.6,0.5,1.0 };
	GLfloat mat_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_specular[] = { 0,0,0.0,1.0 };
	GLfloat mat_shininess[] = { 10.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_l1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING); //start lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

//setting light2
void lighting2() {
	//reset the light
	GLfloat light_position[] = { 1000,20000,1000,0.0 };
	GLfloat light_specular[] = { 0.2,0.2,0.2,0.0 };
	GLfloat light_diffuse[] = { 0.2,0.2,0.2,0.0 };
	GLfloat light_ambient[] = { 0.8,0.8,0.8,0.0 };
	GLfloat mat_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_specular[] = { 0,0,0.0,1.0 };
	GLfloat mat_shininess[] = { 10.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_l2);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING); //start lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

//setting texture parameters
void mapping_setting() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//initialize mapping function
void mapping() {
	lighting();
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_FLAT);

	//setting images of home TV
	ReadImage("cw2.bmp", imagewidth0, imageheight0, pixellength0);
	ReadImage("cw1.bmp", imagewidth1, imageheight1, pixellength1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // set pixel storage modes (in the memory) 
	glGenTextures(2, &texture[0]);  // number of texture names to be generated and a pointer to the first element in the array of texture names 
	glBindTexture(GL_TEXTURE_2D, texture[0]);  // target to which texture is bound and the name of the texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth0, imageheight0, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[0]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth1, imageheight1, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[1]);
	mapping_setting();

	//setting images of advertisements
	ReadImage("ad_coca.bmp", adwidth0, adheight0, adlength0);
	ReadImage("ad_spring.bmp", adwidth1, adheight1, adlength1);
	ReadImage("ad_car.bmp", adwidth2, adheight2, adlength2);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(3, &ad_texture[0]);
	glBindTexture(GL_TEXTURE_2D, ad_texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, adwidth0, adheight0, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[2]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, ad_texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, adwidth1, adheight1, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[3]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, ad_texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, adwidth2, adheight2, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[4]);
	mapping_setting();

	//setting images used for walls and floor
	ReadImage("door.bmp", doorwidth, doorheight, doorlength);
	ReadImage("wall_flower.bmp", paperwidth, paperheight, paperlength);
	ReadImage("floor.bmp", grdwidth, grdheight, grdlength);
	ReadImage("jhwsx.bmp", jhwidth, jhheight, jhlength);
	ReadImage("outer_wall.bmp", otwallwidth, otwallheight, otlength);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(6, &mapping_texture[0]);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, doorwidth, doorheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[5]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, paperwidth, paperheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[6]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, mapping_texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, grdwidth, grdheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[7]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, mapping_texture[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, jhwidth, jhheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[8]);
	mapping_setting();
	glBindTexture(GL_TEXTURE_2D, mapping_texture[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, otwallwidth, otwallheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[9]);
	mapping_setting();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void when_in_mainloop() {
	glutPostRedisplay();
}

//a timer for image change
void onTimer(int value) {
	//automatically changing different TV images 
	if (photo_num == 0)
		photo_num = 1;
	else
		photo_num = 0;

	//automatically changing advertisement images
	if (ad_number < 2)
		ad_number++;
	else
		ad_number = 0;

	glutTimerFunc(2000, onTimer, 1);
}

//a timer for firework
void onTimer_firework(int value) {
	//setting fireworks position 
	p_right.x += 10;  //every time interval, the point controling right line is added a pace   
	p_left.x -= 10;
	p_up.y += 10;
	p_down.y -= 10;

	p_ne.x += pace;
	p_ne.y += pace;
	p_nw.x -= pace;
	p_nw.y += pace;
	p_se.x += pace;
	p_se.y -= pace;
	p_sw.x -= pace;
	p_sw.y -= pace;

	//setting the size of firework 
	if (p_right.x > 150) {
		p_right.x = 100;
		p_left.x = 100;
		p_up.y = 400;
		p_down.y = 400;

		p_ne.x = 100;
		p_ne.y = 400;

		p_se.x = 100;
		p_se.y = 400;

		p_nw.x = 100;
		p_nw.y = 400;

		p_sw.x = 100;
		p_sw.y = 400;
	}

	glutTimerFunc(250, onTimer_firework, 2);
}

//setting font
void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

//setting text
void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) {
		isFirstCall = 0;
		lists = glGenLists(128);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, lists);
	}

	for (; *str != '\0'; ++str) {
		glCallList(lists + *str);
	}
}

//moving trees and road lamps
void animateTracks()
{
	fltOffset--;
	if (fltOffset <= -400.0)
		fltOffset = 0.0;
	else if (fltOffset >= 0.0)
		fltOffset = -400.0;

	fltBuildingOffset--;
	if (fltBuildingOffset <= -2500.0)
		fltBuildingOffset = 2500.0;
	else if (fltBuildingOffset >= 2500.0)
		fltBuildingOffset = -2500.0;

	glutPostRedisplay();
}

//draw the scene of "the house"
void myHome() {
	//====sofa====
	glPushMatrix();
	glColor3f(150 / 255.0, 150 / 255.0, 150 / 255.0);
	glTranslatef(350.0, 15.0, 500.0);
	glScalef(100.0, 30.0, 200.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(160 / 255.0, 160 / 255.0, 160 / 255.0);
	glTranslatef(390.0, 40, 500);
	glScalef(30.0, 80.0, 200.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(170 / 255.0, 170 / 255.0, 170 / 255.0);
	glTranslatef(350.0, 25, 400);
	glScalef(120.0, 51, 20.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(170 / 255.0, 170 / 255.0, 170 / 255.0);
	glTranslatef(350.0, 25, 590);
	glScalef(120.0, 51, 20.0);
	glutSolidCube(1);
	glPopMatrix();

	//====TV cabinet====
	glPushMatrix();
	glColor3f(150 / 255.0, 150 / 255.0, 145 / 255.0);
	glTranslatef(30.0, 25, 500);
	glScalef(60.0, 51, 180.0);
	glutSolidCube(1);
	glPopMatrix();

	//====TV====
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[photo_num]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.2, 70, 550);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.2, 140, 550);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.2, 140, 450);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.2, 70, 450);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//====Dining table====
	glPushMatrix();
	glColor3f(230 / 255.0, 230 / 255.0, 150 / 255.0);
	glTranslatef(340.0, 148, 840);
	glutSolidTeapot(16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(158 / 255.0, 113 / 255.0, 132 / 255.0);
	glTranslatef(340.0, 120, 840);
	glScalef(120.0, 30, 120.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(158 / 255.0, 113 / 255.0, 132 / 255.0);
	glTranslatef(340.0, 60, 840);
	glScalef(20.0, 120, 20.0);
	glutSolidCube(1);
	glPopMatrix();

	//====Human====
	glPushMatrix();
	glTranslatef(50, 0, 0);
	glPushMatrix();
	glColor3f(240 / 255.0, 240 / 255.0, 192 / 255.0);
	glTranslatef(40.0, 150, 850);
	glutSolidSphere(13, 20, 20);
	glPopMatrix();

	//body
	glPushMatrix();
	glColor3f(225 / 255.0, 11 / 255.0, 43 / 255.0);
	glTranslatef(40.0, 90, 850);
	glScalef(40.0, 60, 40.0);
	glutSolidCube(1);
	glPopMatrix();

	//two arms
	glPushMatrix();
	glColor3f(210 / 255.0, 11 / 255.0, 43 / 255.0);
	glTranslatef(17.0, 120, 850);
	glRotatef(-30, 0, 1, 0);
	glScalef(5, 50, 5.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(210 / 255.0, 11 / 255.0, 43 / 255.0);
	glTranslatef(63.0, 120, 850);
	glRotatef(30, 0, 1, 0);
	glScalef(5.0, 50, 5.0);
	glutSolidCube(1);
	glPopMatrix();

	//two legs
	glPushMatrix();
	glColor3f(210 / 255.0, 11 / 255.0, 43 / 255.0);
	glTranslatef(30.0, 30, 850);
	glScalef(10, 60, 10.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(210 / 255.0, 11 / 255.0, 43 / 255.0);
	glTranslatef(50.0, 30, 850);
	glScalef(10.0, 60, 10.0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	//====Floor====
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(409, 0.5, 900);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.5, 900);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.5, 300);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(409, 0.5, 300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//four walls
	// ====Front wall====
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.5, 300);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 400.5, 300);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 400.5, 650);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.5, 650);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.5, 700);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 400.5, 700);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 400.5, 900);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.5, 900);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//====Spring Festival Couplet====
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.2, 0.5, 500);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.2, 300.5, 500);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1.2, 300.5, 700);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-1.2, 0.5, 700);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.5, 200, 605);
	glScalef(1.0, 400.0, 605.0);
	glColor3f(223 / 255.0, 193 / 255.0, 141 / 255.0);
	glutSolidCube(1);
	glPopMatrix();

	//back wall
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(409, 0.5, 300);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(409, 0.5, 900);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(409, 400.5, 900);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(409, 400.5, 300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//the scroll
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(408.9, 200, 400);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(408.9, 250, 400);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(408.9, 250, 600);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(408.9, 200, 600);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(410.0, 200, 605);
	glScalef(1.0, 410.0, 610.0);
	glColor3f(223 / 255.0, 193 / 255.0, 141 / 255.0);
	glutSolidCube(1);
	glPopMatrix();

	//left wall
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(409, 400.5, 301);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 400.5, 301);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.5, 301);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(409, 0.5, 301);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(200.0, 200, 300);
	glScalef(410.0, 410.0, 1.0);
	glColor3f(223 / 255.0, 193 / 255.0, 141 / 255.0);
	glutSolidCube(1);
	glPopMatrix();

	//right wall
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(409, 400.5, 899);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 400.5, 899);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.5, 899);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(409, 0.5, 899);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(200.0, 200, 900);
	glScalef(410.0, 410.0, 1.0);
	glColor3f(223 / 255.0, 193 / 255.0, 141 / 255.0);
	glutSolidCube(1);
	glPopMatrix();

	//roof
	glPushMatrix();
	glTranslatef(200.0, 400, 605);
	glScalef(409.0, 1.0, 600.0);
	glColor3f(223 / 255.0, 193 / 255.0, 141 / 255.0);
	glutSolidCube(1);
	glPopMatrix();
}

//draw tress on the both sides of the road
void tree() {

	for (int fltTreeNumber = -25; fltTreeNumber < 25; fltTreeNumber += 4.0)
	{
		//leaves
		glPushMatrix();
		glTranslatef(-150.0, 100.0, (fltTreeNumber * 100) + fltOffset - 125);
		glPushMatrix();
		glTranslatef(0, 150, -50);
		glScalef(25.0, 25.0, 25.0);
		glColor3f(77.0 / 255.0, 152.0 / 255.0, 127 / 255.0);
		glutSolidDodecahedron();
		glColor3f(1.0, 1.0, 1.0);
		glutWireDodecahedron();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 150, 50);
		glScalef(25.0, 25.0, 25.0);
		glColor3f(77.0 / 255.0, 152.0 / 255.0, 127 / 255.0);
		glutSolidDodecahedron();
		glColor3f(1.0, 1.0, 1.0);
		glutWireDodecahedron();
		glPopMatrix();

		//tree trunk
		glPushMatrix();
		glScalef(15.0, 200.0, 15.0);
		glColor3f(109.0 / 255.0, 50.0 / 255.0, 0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 90.0, 15.0);
		glRotatef(30, 1.0, 0.0, 0.0);
		glScalef(15.0, 60.0, 15.0);
		glColor3f(109.0 / 255.0, 50.0 / 255.0, 0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 90.0, -15.0);
		glRotatef(-30, 1.0, 0.0, 0.0);
		glScalef(15.0, 60.0, 15.0);
		glColor3f(109.0 / 255.0, 50.0 / 255.0, 0);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();
	}
}

//The street light on the right side of the road
void lamp_right() {
	for (int fltPoleNumber = -25; fltPoleNumber < 25; fltPoleNumber += 4.0)
	{
		glPushMatrix();
		glTranslatef(-200.0, 0.0, ((fltPoleNumber * 100) + fltOffset));
		glPushMatrix();
		glTranslatef(0.0, 400.0, 0.0);
		glScalef(80.0, 10.0, 10.0);
		glColor3f(155.0 / 255.0, 155.0 / 255.0, 155.0 / 255.0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-20, 200.0, 0.0);
		glScalef(7.0, 400.0, 7.0);
		glColor3f(155.0 / 255.0, 155.0 / 255.0, 155.0 / 255.0);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();
	}
}

//the street light on the left side
void lamp_left() {
	for (int fltPoleNumber = -25; fltPoleNumber < 25; fltPoleNumber += 4.0)
	{
		glPushMatrix();
		glTranslatef(-200.0, 0.0, ((fltPoleNumber * 100) + fltOffset));
		glPushMatrix();
		glTranslatef(0.0, 400.0, 0.0);
		glScalef(80.0, 10.0, 10.0);
		glColor3f(155.0 / 255.0, 155.0 / 255.0, 155.0 / 255.0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20, 200.0, 0.0);
		glScalef(7.0, 400.0, 7.0);
		glColor3f(155.0 / 255.0, 155.0 / 255.0, 155.0 / 255.0);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();
	}
}

//draw the outdoor map
void groundAndTracks()
{
	//===Moon===
	glPushMatrix();
	glColor3f(248.0 / 255.0, 245.0 / 255.0, 210.0 / 255.0);
	glTranslatef(-1000, 500, -100);
	glutSolidSphere(50, 10, 10);
	glPopMatrix();

	//========Road Lamp and Tree======== 
	glPushMatrix();
	lamp_right();
	tree();
	glTranslatef(500, 0, 0);
	tree();
	glTranslatef(20, 0, 0);
	lamp_left();
	glPopMatrix();

	//========Buildings======== 
	glPushMatrix();
	glTranslatef(-400.0, 0.0, fltBuildingOffset);
	glPushMatrix();
	glTranslatef(0.0, 75, 0.0);
	glScalef(200.0, 500.0, 300.0);
	glColor3f(91 / 255.0, 180 / 255.0, 201 / 255.0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-500.0, 0.0, fltBuildingOffset + 1000);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapping_texture[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(200.5, 0, 300);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(200.5, 800, 300);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(200.5, 800, -300);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(200.5, 0, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//====Advertisements====
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ad_texture[ad_number]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(201, 200, 300);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(201, 600, 300);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(201, 600, -300);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(201, 200, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 300, 0.0);
	glScalef(400.0, 1000.0, 600.0);
	glColor3f(110 / 255.0, 110 / 255.0, 110 / 255.0);
	glutSolidCube(1);
	glColor3f(1.0, 1.0, 1.0);
	glutWireCube(1);
	glPopMatrix();
	glPopMatrix();

	//====My Home====
	glPushMatrix();
	glTranslatef(800.0, 0.0, 0);
	myHome();
	glPopMatrix();

	//========Ground======== 
	glPushMatrix();
	glTranslatef(0.0, -5.0, 0.0);
	glPushMatrix();
	glScalef(5000.0, 10.0, 5000.0);
	glColor3f(210.0 / 255.0, 210.0 / 255.0, 210.0 / 255.0);
	glutSolidCube(1);
	glPopMatrix();

	//=====Road======
	glPushMatrix();
	glTranslatef(100.0, 5.1, 0.0);
	glColor3f(100 / 255.0, 100 / 255.0, 100 / 255.0);
	glBegin(GL_QUADS);
	glVertex3f(200, 0, 2500);
	glVertex3f(-200, 0, 2500);
	glVertex3f(-200, 0, -2500);
	glVertex3f(200, 0, -2500);
	glEnd();

	//yellow alerting line
	glColor3f(250 / 255.0, 203 / 255.0, 82 / 255.0);
	glBegin(GL_QUADS);
	glVertex3f(20, 0, 2500);
	glVertex3f(-20, 0, 2500);
	glVertex3f(-20, 0, -2500);
	glVertex3f(20, 0, -2500);
	glEnd();

	//footpath to my home
	glColor3f(65 / 255.0, 120 / 255.0, 10 / 255.0);
	glBegin(GL_QUADS);
	glVertex3f(200, 0, 550);
	glVertex3f(800, 0, 550);
	glVertex3f(800, 0, 650);
	glVertex3f(200, 0, 650);
	glEnd();
	glPopMatrix();

	//=====Firework=====
	glPushMatrix();
	glColor3f(255 / 255.0, 0 / 255.0, 0 / 255.0);
	glTranslatef(100, 0, 1500);
	draw_firework();
	glTranslatef(-200, 0, 0);
	draw_firework();
	glColor3f(22 / 255.0, 255 / 255.0, 143 / 255.0);
	glTranslatef(100, -50, 0);
	draw_firework();
	glColor3f(189 / 255.0, 166 / 255.0, 224 / 255.0);
	glTranslatef(-20, 60, 0);
	draw_firework();
	glColor3f(255 / 255.0, 10 / 255.0, 10 / 255.0);
	glTranslatef(40, 60, 0);
	draw_firework();
	glPopMatrix();
	glPopMatrix();
}

//simulate the bus window
void window() {
	glColor3f(220.0 / 255.0, 220.0 / 255.0, 220.0 / 255.0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 500);
	glVertex2f(30, 500);
	glVertex2f(30, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(30, 30);
	glVertex2f(700, 30);
	glVertex2f(700, 0);
	glVertex2f(30, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(700, 30);
	glVertex2f(700, 500);
	glVertex2f(670, 500);
	glVertex2f(670, 30);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(670, 500);
	glVertex2f(30, 500);
	glVertex2f(30, 470);
	glVertex2f(670, 470);
	glEnd();

	glPushMatrix();
	selectFont(25, ANSI_CHARSET, "Comic sans MS");
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(intWinWidth - 600, 50.0);	  //setting text starting position
	drawString("CLICK HERE TO GET OFF THE BUS ->");
	glPopMatrix();
}

void displayObject()
{
	glClearColor(6.0 / 255.0, 9.0 / 255.0, 102.0 / 255.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float ini_carheight = 100.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 700, 0, 500);

	//simulate the scene on the bus
	if (getoff_button == 0) {
		glPushMatrix();
		glClearColor(0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 0.0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 700, 0, 500);
		ini_carheight = 20.0;	//the initial camara height is 20
		window();
		glPopMatrix();
	}
	//setting text on the right cornor when freely moving outdoor
	if (getoff_button == 1) {
		glPushMatrix();
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(intWinWidth - 550, 50.0);	  //setting text starting position
		drawString("GO BACK HOME OFTEN !");
		glRasterPos2f(intWinWidth - 550, 20.0);
		drawString("Use'W/A/S/D'to move and rotate");
		glPopMatrix();
	}
	//setting projection and view position for whole scene
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fltFOV, 1, 0.1, 5000);
	gluLookAt(fltX0, fltY0 + ini_carheight, fltZ0, fltXRef, fltYRef, fltZRef, fltXUp,
		fltYUp, fltZUp);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	groundAndTracks();
	glutSwapBuffers();
}

void reshapeWindow(GLint intNewWidth, GLint intNewHeight) {
	glViewport(0, 0, intNewWidth, intNewHeight);
}

void adjustDisplay(unsigned char key, int x, int y)
{
	float gapx = abs(fltXRef - fltX0);
	float gapz = abs(fltZRef - fltZ0);
	float dis_r = sqrt(gapx * gapx + gapz * gapz);

	//Press 'a/A' to turn left
	if (key == 'a' || key == 'A') {
		angle += 5;
		fltXRef = fltX0 - dis_r * cos(angle / 360);
		fltZRef = fltZ0 + dis_r * sin(angle / 360);

	}

	//Press 'd/D' to turn right
	if (key == 'd' || key == 'D') {
		angle -= 5;
		fltXRef = fltX0 - dis_r * cos(angle / 360);
		fltZRef = fltZ0 + dis_r * sin(angle / 360);
	}

	//Press 'w/W' to move forward
	if (key == 'w' || key == 'W') {
		if (dis_r <= 300) {
			fltXRef = fltX0 - 5000 * cos(angle / 360);
			fltZRef = fltZ0 + 5000 * sin(angle / 360);
		}
		fltX0 = fltX0 - 5 * cos(angle / 360);
		fltZ0 = fltZ0 + 5 * sin(angle / 360);
	}

	//Press 's/S' to move backward
	if (key == 's' || key == 'S') {
		if (dis_r <= 300) {
			fltXRef = fltX0 - 5000 * cos(angle / 360);
			fltZRef = fltZ0 + 5000 * sin(angle / 360);
		}
		fltX0 = fltX0 + 5 * cos(angle / 360);
		fltZ0 = fltZ0 - 5 * sin(angle / 360);
	}

	//Press 'j/J' to move down
	if (key == 'J' || key == 'j')
		if (fltY0 > 35)
			fltY0 -= 10;

	//Press 'u/U' to move up
	if (key == 'u' || key == 'U')
		if (fltY0 < 1000)
			fltY0 += 10;

	glutPostRedisplay();
}

void mouse_input(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= 400 && y >= 400) {
		//click left button at specific position to stop animated scene and start keyboard interaction 
		if (getoff_button == 0) {
			getoff_button = 1;
			glutIdleFunc(NULL);
			glutKeyboardFunc(adjustDisplay);
			glutIdleFunc(when_in_mainloop);
			glutTimerFunc(2000, onTimer, 1);
			glutTimerFunc(250, onTimer_firework, 2);
		}
		lighting2();   //light 2 is brighter
	}
	//click right button to change the light setting
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		lighting();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(intWinWidth, intWinHeight);
	glutCreateWindow("Spring Festival");

	mapping();
	glutDisplayFunc(displayObject);
	glutReshapeFunc(reshapeWindow);
	glutKeyboardFunc(NULL);				//at the beginning, keyboard interaction is not allowed
	glutMouseFunc(mouse_input);
	glutIdleFunc(animateTracks);
	glutMainLoop();
}