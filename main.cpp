/*
	gords-flights-sim
	Copyright 2012 Gordon Foster
	
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

#include <windows.h>        // after upgrading to GLUT 3.7.6, had to add this!
#include <GL/glut.h>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <stdio.h>

struct Vector3
{
    float x;
    float y;
    float z;
    Vector3() : x(0), y(0), z(0) {}; // initialze when created
};

#define PIOVER180 3.14/180

GLuint textures[2];
float myar;                 // my copy of aspect ratio

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { .5f, .5f, .5f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = {0, 5, -50, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

bool* keyStates = new bool[256]; // Create an array of boolean values of leng

float IDENTITY_MATRIX[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

float modelview[16];
float angle[16];
float result[16];
float final[16];

float rotX = 0;
float rotY = 0;
float rotZ = 0;
float atX = 0;
float atY = 0;
float atZ = 0;




void copyMat(float dest[], float src[])
{
    int c;
    for(c = 0; c < 16; c++)
        dest[c] = src[c];
}

void translationMat(float *dest, float x, float y, float z)
{
    dest[0] = 1;
    dest[4] = 0;
    dest[8] = 0;
    dest[12]= x;
    dest[1] = 0;
    dest[5] = 1;
    dest[9] = 0;
    dest[13]= y;
    dest[2] = 0;
    dest[6] = 0;
    dest[10]= 1;
    dest[14]= z;
    dest[3] = 0;
    dest[7] = 0;
    dest[11]= 0;
    dest[15]= 1;
}

void rotMatX(float *dest, float a)
{
    copyMat(dest, IDENTITY_MATRIX);

    dest[5] = cos(a);
    dest[6] = -sin(a);
    dest[9] = sin(a);
    dest[10]= cos(a);
}

void rotMatY(float *dest, float a)
{
    copyMat(dest, IDENTITY_MATRIX);

    dest[0] = cos(a);
    dest[2] = sin(a);
    dest[8] = -sin(a);
    dest[10]= cos(a);
}

void rotMatZ(float * dest, float a)
{
    copyMat(dest, IDENTITY_MATRIX);

    dest[0] = cos(a);
    dest[1] = sin(a);
    dest[4] = -sin(a);
    dest[5]= cos(a);
}


void multMat(float *matDest, float *matA, float *matB)
{
    float dest[16];
    int c, d, e;

    for(c = 0; c < 4; c++)
        for(d = 0; d < 4; d++)
            for(e = 0, dest[c + d * 4] = 0; e < 4; e++)
                dest[c + d * 4] += matA[c + e * 4] * matB[e + d * 4];

    copyMat(matDest, dest);
}
void mycam()
{
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    rotMatX(angle, rotX*PIOVER180);
    multMat(result, modelview, angle);
    copyMat(final, result);

    rotMatY(angle, rotY*PIOVER180);
    multMat(result, modelview, angle);
    multMat(final, final, result);

    rotMatZ(angle, rotZ*PIOVER180);   // - right
    multMat(result, modelview, angle);
    multMat(final, final, angle);

    translationMat(angle, atX, atY, atZ);        // - move back
    multMat(result, modelview, angle);
    multMat(final, final, angle);

    glLoadMatrixf(final);
}
void LoadTextures()
{
    sf::Image img;
    sf::Image img2;
    if(!img.LoadFromFile("c:\\Grass.bmp"))
        throw 1;

    if(!img2.LoadFromFile("c:\\asphalt.bmp"))
        throw 1;

    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img2.GetWidth(), img2.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.GetPixelsPtr());

}

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    myar = ar;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);  // left, right, bottom, top, near, far
    gluPerspective(45,ar,0,100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

#define SPEED 1

void keyOperations (void)
{
    if (keyStates['q'])
        exit(0);
    if (keyStates['j'])
    {
        rotZ -= .1*SPEED;
        if(rotZ < 0)
            rotZ = 360;
    }

    if (keyStates['l'])
    {
        rotZ += .1*SPEED;
        if(rotZ > 360)
            rotZ = 0;
    }

    if (keyStates['a'])
    {
        rotY += .15*SPEED;
        if(rotY > 360)
            rotY = 0;
    }

    if (keyStates['d'])
    {
        rotY -= .15*SPEED;
        if(rotY < 0)
            rotY = 360;
    }

    if (keyStates['w'])
    {
        atZ += .1*SPEED;
    }


    if (keyStates['s'])
    {
        atZ -= .1*SPEED;
    }

    if (keyStates['i'])
    {
        rotX -= 0.1*SPEED;
        if(rotX < 0)
            rotX = 360;
    }
    if (keyStates['k'])
    {
        rotX += 0.1*SPEED;
        if(rotX > 360)
            rotX = 0;
    }

    if(keyStates['r'])
    {
        rotX = rotY = rotZ = 0;
        atX = atY = atZ = 0;
    }

    glutPostRedisplay();
}

bool temp = 0;
static void display(void)
{
    //const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    //const double a = t*90.0;
    keyOperations();    // handle keyboard input
    mycam();			// update camera view (math)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);

    // draw the grass
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    glTexCoord2f (0, 0);
    glVertex3f(-100,-1,0);
    glTexCoord2f (1, 0);
    glVertex3f(100,-1,0);
    glTexCoord2f (1, 1);
    glVertex3f(100,-1,-100);
    glTexCoord2f (0, 1);
    glVertex3f(-100,-1,-100);
    glEnd();

    // draw the runway
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);
    glVertex3f(-2, -1, 0);
    glTexCoord2f(1, 0);
    glVertex3f(2, -1, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-2, -1, -100);
    glTexCoord2f(1, 0);
    glVertex3f(-2, -1, -100);
    glTexCoord2f(0, 1);
    glVertex3f(2, -1, 0);
    glTexCoord2f(1, 1);
    glVertex3f(2, -1, -100);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // draw yellow lines
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1,1,0);
    float z;
    for(z = -2; z > -100; z-=1.5)
    {
        glBegin(GL_QUADS);
        glVertex3f(-0.125f, -1, z);
        glVertex3f(0.125f, -1, z);
        glVertex3f(0.125f, -1, z-1);
        glVertex3f(-0.125f, -1, z-1);
        glEnd();
    }

    // make a simple line of trees
    for(z = -25; z >= -75; z-=25)
    {
        glPushMatrix(); // save what we've done so far
        glColor3f(1,1,0);
        glTranslatef(-2.5,-1,z);
        glRotated(90, -1, 0, 0);
        glutSolidCone(0.25, 2, 50, 50);    // base radius, height, slices, stack
        glPopMatrix();

        glPushMatrix(); // save what we've done so far
        glColor3f(0,1,0);
        glTranslatef(-2.5,0.5,z);
        glutSolidSphere(1,50,50);
        glPopMatrix();
    }

    // make a simple line of trees
    for(z = -12.5; z >= -62.5; z-=25)
    {
        glPushMatrix(); // save what we've done so far
        glColor3f(1,1,0);
        glTranslatef(2.5,-1,z);
        glRotated(90, -1, 0, 0);
        glutSolidCone(0.25, 2, 50, 50);    // base radius, height, slices, stack
        glPopMatrix();

        glPushMatrix(); // save what we've done so far
        glColor3f(0,1,0);
        glTranslatef(2.5,0.5,z);
        glutSolidSphere(1,50,50);
        glPopMatrix();
    }

    // make a sun (cheaply, it isn't really far away)
    glColor3f(1.0,0.5,0.0);
    glPushMatrix();
    glTranslatef(0,100,-50);
    glutSolidSphere(1.0,50,50);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

static void keyUp (unsigned char key, int x, int y)
{
    keyStates[key] = false; // Set the state of the current
}

static void key(unsigned char key, int x, int y)
{
    keyStates[key] = true; // Set the state of the current
}

static void specialkey(int key, int x, int y)
{
    //keyStates[key] = true; // Set the state of the current
//    keyOperations();    // handle keyboard input

    if(key == GLUT_KEY_UP)
        atY -= 1;
    else
    if(key == GLUT_KEY_DOWN)
        atY += 1;
    else
    if(key == GLUT_KEY_LEFT)
	    atX += 1;
	else
	if(key == GLUT_KEY_RIGHT)
	    atX -= 1;
}

static void idle(void)
{
    keyOperations();
    glutPostRedisplay();
}




int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialkey);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);

    glClearColor(0.529,0.808,0.980,1);      // sky blue

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

    LoadTextures();


    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);


    glutMainLoop();

    return EXIT_SUCCESS;
}
