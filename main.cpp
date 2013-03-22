/*
	gords-flights-sim
	Copyright 2013 Gordon Foster

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*

Keys to use:
w : increase speed
s : decrease speed
a : turn left
d : turn right
i : pitch down
k : pitch up
j : roll left
l : roll right
up : move up
down : move down
left : strafe left
right : strafe right

*/

// INCLUDES

#include <windows.h>            // after upgrading to GLUT 3.7.6, had to add this!
#include <GL/glut.h>
#include <SFML/Graphics.hpp>    // using for loading textures
#include <stdio.h>
#include <stdarg.h>   			//  To use functions with variables arguments
#include <stdlib.h>   			//  for malloc

//#include <al/al.h>
//#include <al/alc.h>
//#include <al/alut.h>

#include "camera.h"
#include "world.h"

using namespace std;

// DEFINES

// this will be set on Visual Studio only, so this code is added for all other compilers
#ifndef _MSC_VER
#define vsprintf_s(b,l,f,v) vsprintf(b,f,v);
#endif

#define NEARPLANE    0
#define FARPLANE     1
#define FOVY		35
#define NUM_TEXTURES	8
#define TEXTURE_GRASS	0
#define TEXTURE_ASPHALT	1
#define TEXTURE_BACK	2
#define TEXTURE_FRONT	3
#define TEXTURE_TOP		6
#define TEXTURE_LEFT	5
#define TEXTURE_RIGHT	4
#define TEXTURE_HOUSE	7
// GLOBALS

CCamera Camera;
CWorld World;

GLuint textures[2+5+1];  // grass, asphalt, skybox, house
GLuint airplane;  // actually an airplane

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { .5f, .5f, .5f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = {0, 5, -50, 0.0f };
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

bool* keyStates = new bool[256]; // Create an array of boolean values of leng

//  Just a pointer to a font style..
//  Fonts supported by GLUT are: GLUT_BITMAP_8_BY_13,
//  GLUT_BITMAP_9_BY_15, GLUT_BITMAP_TIMES_ROMAN_10,
//  GLUT_BITMAP_TIMES_ROMAN_24, GLUT_BITMAP_HELVETICA_10,
//  GLUT_BITMAP_HELVETICA_12, and GLUT_BITMAP_HELVETICA_18.
GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;

float moveSpeed = 0;
//float timeSinceLastUpdate, prevTime, moveFactor;
float timeSinceLastUpdate, prevTime;

// FUNCTION DEFINITIONS

void display();
void printw();
void LoadTextures();
void drawskybox();
void drawObjs();
void drawText();
void loadObj(char *fname);
static void keyOperations (void);
void resize(int width, int height);
void keyUp (unsigned char key, int x, int y);
void key(unsigned char key, int x, int y);
void specialkey(int key, int x, int y);
void idle();
void drawground();
void printw (float x, float y, float z, char* format, ...);
void reset();

// UTILITY FUNCTIONS


/*
* These are OpenAL "names" (or "objects"). They store and id of a buffer
* or a source object. Generally you would expect to see the implementation
* use values that scale up from '1', but don't count on it. The spec does
* not make this mandatory (as it is OpenGL). The id's can easily be memory
* pointers as well. It will depend on the implementation.
*/

// Buffers to hold sound data.
//ALuint Buffer;

// Sources are points of emitting sound.
//ALuint Source;


/*
* These are 3D cartesian vector coordinates. A structure or class would be
* a more flexible of handling these, but for the sake of simplicity we will
* just leave it as is.
*/

// Position of the source sound.
//ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
//ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };


// Position of the Listener.
//ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the Listener.
//ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
//ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };



/*
* ALboolean LoadALData()
*
*	This function will load our sample data from the disk using the Alut
*	utility and send the data into OpenAL as a buffer. A source is then
*	also created to play that buffer.
*/
/*
ALboolean LoadALData()
{
	// Variables to load into.

	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	// Load wav data into a buffer.

	alGenBuffers(1, &Buffer);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alutLoadWAVFile("wavdata/FancyPants.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffer, format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	// Bind the buffer with the source.

	alGenSources(1, &Source);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Source, AL_BUFFER,   Buffer   );
	alSourcef (Source, AL_PITCH,    1.0      );
	alSourcef (Source, AL_GAIN,     1.0      );
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_LOOPING,  loop     );

	// Do another error check and return.

	if(alGetError() == AL_NO_ERROR)
		return AL_TRUE;

	return AL_FALSE;
}
*/


/*
* void SetListenerValues()
*
*	We already defined certain values for the Listener, but we need
*	to tell OpenAL to use that data. This function does just that.
*/
/*
void SetListenerValues()
{
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}
*/


/*
* void KillALData()
*
*	We have allocated memory for our buffers and sources which needs
*	to be returned to the system. This function frees that memory.
*/
/*
void KillALData()
{
	alDeleteBuffers(1, &Buffer);
	alDeleteSources(1, &Source);
	alutExit();
}
*/

//-------------------------------------------------------------------------
//  Draws a string at the specified coordinates.
//-------------------------------------------------------------------------
void printw (float x, float y, float z, char* format, ...)
{
    va_list args;   //  Variable argument list
    int len;        // String length
    int i;          //  Iterator
    char* text;    // Text

    //  Initialize a variable argument list
    va_start(args, format);

    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    len = _vscprintf(format, args) + 1;

    //  Allocate memory for a string of the specified size
    text = (char*)malloc(len * sizeof(char));

    //  Write formatted output using a pointer to the list of arguments
    vsprintf_s(text, len, format, args);

    //  End using variable argument list
    va_end(args);

    //  Specify the raster position for pixel operations.
    glRasterPos3f (x, y, z);

    //  Draw the characters one by one
    for (i = 0; text[i] != '\0'; i++)
    glutBitmapCharacter(font_style, text[i]);

    //  Free the allocated memory for the string
    free(text);
}

void LoadTextures()
{
    sf::Image img;  //grass
    sf::Image img2; // asphalt
    sf::Image img3, img4, img5, img6, img7;  // skybox
    sf::Image img8;  // housee
    if(!img.LoadFromFile("Grass.bmp"))
        throw 1;
    if(!img2.LoadFromFile("asphalt.bmp"))
        throw 1;
    if(!img3.LoadFromFile("backw3.bmp"))
        throw 1;
    if(!img4.LoadFromFile("frontw3.bmp"))
        throw 1;
    if(!img5.LoadFromFile("leftw3.bmp"))
        throw 1;
    if(!img6.LoadFromFile("rightw3.bmp"))
        throw 1;
    if(!img7.LoadFromFile("topw3.bmp"))
        throw 1;
    if(!img8.LoadFromFile("home.bmp"))
		throw 1;

    glGenTextures(NUM_TEXTURES, textures);

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_GRASS]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ASPHALT]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img2.GetWidth(), img2.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BACK]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img3.GetWidth(), img3.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img3.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FRONT]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img4.GetWidth(), img4.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img4.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_RIGHT]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img5.GetWidth(), img5.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img5.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_LEFT]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img6.GetWidth(), img6.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img6.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_TOP]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img7.GetWidth(), img7.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img7.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_HOUSE]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img8.GetWidth(), img8.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img8.GetPixelsPtr());

}

void loadObj(char *fname)
{
	FILE *fp;
	int read;
	GLfloat x, y, z;
	char ch;
	airplane = glGenLists(1);
	fp = fopen(fname, "r");

	if(!fp)
    {
       	printf("can't open file %s\n", fname);
	 	exit(1);
    }

	glPointSize(2.0);
	glNewList(airplane, GL_COMPILE);
	glPushMatrix();
	glBegin(GL_POINTS);
		while(!(feof(fp)))
		{
			read = fscanf(fp, "%c %f %f %f", &ch, &x, &y, &z);
			if(read == 4 && ch == 'v')
				glVertex3f(x,y,z);
		}
	glEnd();
	glPopMatrix();
	glEndList();

	fclose(fp);
}

static void keyOperations (void)
{
    if (keyStates['q'])
        exit(0);

    if (keyStates['j'])
        Camera.RotateZ(1);

    if (keyStates['l'])
        Camera.RotateZ(-1);

    if (keyStates['a'])
        Camera.RotateY(0.3f);

    if (keyStates['d'])
        Camera.RotateY(-0.3f);

    if (keyStates['w'])
        if(moveSpeed <1)
            moveSpeed += .001;

    if (keyStates['s'])
    {
        if(moveSpeed > 0)
            moveSpeed -= .001;
        if(moveSpeed < .001)
			moveSpeed=0;
    }

    if (keyStates['i'])
        Camera.RotateX(-0.5);

    if (keyStates['k'])
        Camera.RotateX(0.5);

    if (keyStates['r'])
    	reset();
}

void reset()
{
	moveSpeed = 0;
	Camera.Reset();
}

// CALLBACK FUNCTIONS

void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(FOVY,ar,NEARPLANE,FARPLANE);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void keyUp (unsigned char key, int x, int y)
{
    keyStates[key] = false; // Set the state of the current
}

void key(unsigned char key, int x, int y)
{
	if(key == 27)
		exit(0);
	else
		keyStates[key] = true; // Set the state of the current
}

void specialkey(int key, int w,int h)
{
    if(key == GLUT_KEY_UP)
        Camera.MoveUpward(0.3);
    else
    if(key == GLUT_KEY_DOWN)
        Camera.MoveUpward(-0.3);
    else
    if(key == GLUT_KEY_LEFT)
        Camera.StrafeRight(-0.2);
	else
	if(key == GLUT_KEY_RIGHT)
	    Camera.StrafeRight(0.2);
}

void idle(void)
{
	glutPostRedisplay();
}

void drawText()
{
	SF3dVector curv;		// current vector
    float x,y,z;

	glLoadIdentity();
	glColor3f(1.0f, 0.0f, 0.0f);
	curv = Camera.GetPosition();
	x = curv.x -5.5f;
	y = curv.y +3.5f;
	z = -1;

    printw(x, y, z, "Altitude: %f", curv.y);
    y -=0.5f;
    printw(x, y, z, "x: %f", curv.x);
    y -=0.5f;
    printw(x, y, z, "z: %f", curv.z);
    y -=0.5f;
    printw(x, y, z, "Speed: %f", moveSpeed);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

	//keyOperations();

    glLoadIdentity();
    //if(moveSpeed != 0)
    //{
    //	SF3dVector curv;		// current vector
	//	curv = Camera.GetPosition();
	//	if((curv.y > 0) && (moveSpeed < 0.2f))
	//		Camera.MoveUpward(-0.05f);
	//	curv = Camera.GetPosition();
	//	if(curv.y < 0)
	//		Camera.MoveUpward(-curv.y);
		//if((curv.y == 0) && (moveSpeed >= 0.2f))
		//	reset();
		timeSinceLastUpdate = clock() - prevTime;// = clock();
		prevTime = clock();
		Camera.MoveForward(moveSpeed * timeSinceLastUpdate);// * moveFactor);
		Camera.Render();
    //}

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);

	// draw the scene
	drawskybox();

	glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

glColor3f(1,1,1);
	drawground();



    drawObjs();  // makes GL_POINTS from .obj file
	drawText();

	glFlush();
	glutSwapBuffers();
	keyOperations();
}

// DRAWING FUNCTIONS

void drawground()
{
	// draw the grass   -   this really needs to be fixed
	int x, z2;

    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_GRASS]);
    glBegin(GL_QUADS);
    	for(x = -World.back; x < 0; x +=World.back)
    	{
			for(z2 = -World.back; z2 < 0; z2 +=World.back)
			{
				glTexCoord2f (0, 0);
				glVertex3f(x, -1, z2+World.back);
				glTexCoord2f (1, 0);
				glVertex3f(x+World.back, -1, z2+World.back);
				glTexCoord2f (1, 1);
				glVertex3f(x+World.back, -1, z2);
				glTexCoord2f (0, 1);
				glVertex3f(x, -1, z2);
			}
    	}
    	for(x = 0; x < World.back; x +=World.back)
    	{
			for(z2 = -World.back; z2 < 0; z2 +=World.back)
			{
				glTexCoord2f (0, 0);
				glVertex3f(x, -1, z2+World.back);
				glTexCoord2f (1, 0);
				glVertex3f(x+World.back, -1, z2+World.back);
				glTexCoord2f (1, 1);
				glVertex3f(x+World.back, -1, z2);
				glTexCoord2f (0, 1);
				glVertex3f(x, -1, z2);
			}
    	}
    glEnd();

    // draw the runway
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ASPHALT]);
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(-2, -1, 0);
        glTexCoord2f(1, 0);
        glVertex3f(2, -1, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-2, -1, -World.back);
        glTexCoord2f(1, 0);
        glVertex3f(-2, -1, -World.back);
        glTexCoord2f(0, 1);
        glVertex3f(2, -1, 0);
        glTexCoord2f(1, 1);
        glVertex3f(2, -1, -World.back);
    glEnd();

/*
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_HOUSE]);
    glBegin(GL_QUADS);
		glTexCoord2f (0, 0);
		glVertex3f(-10.0f, 2.0f, -50.0f);
		glTexCoord2f (1, 0);
		glVertex3f(-13.0f, 2.0f, -50.0f);
		glTexCoord2f (1, 1);
        glVertex3f(-13.0f, -1.0f, -50.0f);
        glTexCoord2f (0, 1);
        glVertex3f(-10.0f, -1.0f, -50.0f);

        glTexCoord2f (0, 0);
		glVertex3f(-10.0f, 2.0f, -50.0f);
		glTexCoord2f (1, 0);
		glVertex3f(-10.0f, 2.0f, -53.0f);
		glTexCoord2f (1, 1);
        glVertex3f(-10.0f, -1.0f, -53.0f);
        glTexCoord2f (0, 1);
        glVertex3f(-10.0f, -1.0f, -50.0f);

    glEnd();
    */
glDisable(GL_TEXTURE_2D);



    // draw yellow lines
    //glDisable(GL_LIGHTING);
    glColor3f(1,1,0);
    float z;
    for(z = -2; z > -World.back; z-=1.5)
    {
        glBegin(GL_QUADS);
            glVertex3f(-0.125f, -1, z);
            glVertex3f(0.125f, -1, z);
            glVertex3f(0.125f, -1, z-1);
            glVertex3f(-0.125f, -1, z-1);
        glEnd();
    }

}

void drawskybox()
{
    // Draw a skybox
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);

    // Render the front quad
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FRONT]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  World.back, 550.0f, 0.0f );
        glTexCoord2f(1, 0); glVertex3f( -World.back, 550.0f, 0.0f );
        glTexCoord2f(1, 1); glVertex3f( -World.back,  -450.0f, 0.0f );
        glTexCoord2f(0, 1); glVertex3f(  World.back,  -450.0f, 0.0f );
    glEnd();

    // Render the left quad
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_LEFT]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  -World.back, 550.0f,  0.0f );
        glTexCoord2f(1, 0); glVertex3f( -World.back, 550.0f, -World.back );
        glTexCoord2f(1, 1); glVertex3f(  -World.back,  -450.0f, -World.back );
        glTexCoord2f(0, 1); glVertex3f(  -World.back,  -450.0f,  0.0f );
    glEnd();

    // Render the back quad
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BACK]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -World.back, 550.0f,  -World.back );
        glTexCoord2f(1, 0); glVertex3f(  World.back, 550.0f,  -World.back );
        glTexCoord2f(1, 1); glVertex3f(  World.back,  -450.0f,  -World.back );
        glTexCoord2f(0, 1); glVertex3f( -World.back,  -450.0f,  -World.back );
    glEnd();

    // Render the right quad
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_RIGHT]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( World.back, 550.0f, -World.back );
        glTexCoord2f(1, 0); glVertex3f( World.back, 550.0f,  -0.0f );
        glTexCoord2f(1, 1); glVertex3f( World.back,  -450.0f,  -0.0f );
        glTexCoord2f(0, 1); glVertex3f( World.back,  -450.0f, -World.back );
    glEnd();

    // Render the top quad
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_TOP]);
    glBegin(GL_QUADS);
    	glTexCoord2f(0, 0); glVertex3f( World.back,  550.0f, -World.back );
    	glTexCoord2f(1, 0); glVertex3f( -World.back,  550.0f,  -World.back );
    	glTexCoord2f(1, 1); glVertex3f(  -World.back, 550.0f,  0.0f );
    	glTexCoord2f(0, 1); glVertex3f(  World.back,  550.0f, 0.0f );
    glEnd();
}

// DRAW LOADED .obj FILES

void drawObjs()
{
 	//glPushMatrix();
 	glLoadIdentity();
 	glTranslatef(0.00, -5, -15.5);
 	glRotatef(110, 1, 0, 0);
 	glRotatef(180, 0, 1, 0);
 	//glColor3f(1,0.23,0.27);
 	glColor3f(0,0,0);
 	glCallList(airplane);
 	//glPopMatrix();
}

int main(int argc, char *argv[])
{

	// create main window
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("gord-flight-sim");

	// setup callbacks
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialkey);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);

	// background
    glClearColor(0.529,0.808,0.980,1);      // sky blue

	// lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	// bitmap files, etc
    LoadTextures();

	// wavefront obj files
    loadObj("test.obj");//replace porsche.obj with radar.obj or any other .obj to display it

	//alutInit(NULL, 0);
	//alGetError();
	//alutInit(&argc, argv);
	//MessageBox(NULL, alutGetErrorString(NULL), "error", MB_OK);


	// Initialize OpenAL and clear the error bit.
	//MessageBox(NULL, "1", "Error", MB_OK);

	//MessageBox(NULL, "2", "Error", MB_OK);
	//alGetError();
	//MessageBox(NULL, "3", "Error", MB_OK);

	// Load the wav data.

	//if(LoadALData() == AL_FALSE)
	//{
	    //printf("Error loading data.");
	//    MessageBox(NULL, "4", "Error", MB_OK);
	//	return 0;
	//}

	//SetListenerValues();
	//MessageBox(NULL, "5", "Error", MB_OK);
	// Setup an exit procedure.

	//atexit(KillALData);
	//MessageBox(NULL, "6", "Error", MB_OK);


	timeSinceLastUpdate = prevTime = clock();
	glutMainLoop();

    return EXIT_SUCCESS;
}

