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

/*

Keys to use:
w : move forward
s : move backward
a : turn left
d : turn right
i : pitch down
k : pitch up
j : roll left
l : rool right
up : move up
down : move down
left : strafe left
right : strafe right

*/

#include <windows.h>            // after upgrading to GLUT 3.7.6, had to add this!
#include <GL/glut.h>
#include <SFML/Graphics.hpp>    // using for loading textures
#include "camera.h"

#define NEARPLANE    0
#define FARPLANE     1000

CCamera Camera;

GLuint textures[2+5];

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { .5f, .5f, .5f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = {0, 5, -50, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

bool* keyStates = new bool[256]; // Create an array of boolean values of leng

float speed = 0;

void LoadTextures()
{
    sf::Image img;
    sf::Image img2;
    sf::Image img3, img4, img5, img6, img7;
    if(!img.LoadFromFile("c:\\Grass.bmp"))
        throw 1;
    if(!img2.LoadFromFile("c:\\asphalt.bmp"))
        throw 1;


    if(!img3.LoadFromFile("c:\\backw3.bmp"))
        throw 1;
    if(!img4.LoadFromFile("c:\\frontw3.bmp"))
        throw 1;
    if(!img5.LoadFromFile("c:\\leftw3.bmp"))
        throw 1;
    if(!img6.LoadFromFile("c:\\rightw3.bmp"))
        throw 1;
    if(!img7.LoadFromFile("c:\\topw3.bmp"))
        throw 1;

    glGenTextures(7, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img2.GetWidth(), img2.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.GetPixelsPtr());


    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img3.GetWidth(), img3.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img3.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img4.GetWidth(), img4.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img4.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img5.GetWidth(), img5.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img5.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img6.GetWidth(), img6.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img6.GetPixelsPtr());

    glBindTexture(GL_TEXTURE_2D, textures[6]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img7.GetWidth(), img7.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img7.GetPixelsPtr());

}

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(90,ar,NEARPLANE,FARPLANE);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

#define SPEED 7

void keyOperations (void)
{
    if (keyStates['q'])
        exit(0);
    if (keyStates['j'])
    {
        Camera.RotateZ(1 * SPEED);
    }

    if (keyStates['l'])
    {
        Camera.RotateZ(-1 * SPEED);
    }

    if (keyStates['a'])
    {
        Camera.RotateY(0.5 * SPEED);
    }

    if (keyStates['d'])
    {
        Camera.RotateY(-0.5 * SPEED);
    }

    if (keyStates['w'])
    {
        if(speed > -1)
            speed -= 0.01;
    }

    if (keyStates['s'])
    {
        if(speed < 0)
            speed += 0.01;
    }

    if (keyStates['i'])
    {
        Camera.RotateX(-0.5 * SPEED);
    }
    if (keyStates['k'])
    {
        Camera.RotateX(0.5* SPEED);
    }

    glutPostRedisplay();
}

static void drawskybox()
{
    // Draw a skybox
    // Store the current matrix
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);

    // Render the front quad
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  1000.0f, 550.0f, 0.0f );
        glTexCoord2f(1, 0); glVertex3f( -1000.0f, 550.0f, 0.0f );
        glTexCoord2f(1, 1); glVertex3f( -1000.0f,  -450.0f, 0.0f );
        glTexCoord2f(0, 1); glVertex3f(  1000.5f,  -450.0f, 0.0f );
    glEnd();


    // Render the left quad
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  -1000.0f, 550.0f,  0.0f );
        glTexCoord2f(1, 0); glVertex3f( -1000.0f, 550.0f, -FARPLANE );
        glTexCoord2f(1, 1); glVertex3f(  -1000.0f,  -450.0f, -FARPLANE );
        glTexCoord2f(0, 1); glVertex3f(  -1000.0f,  -450.0f,  0.0f );
    glEnd();

    // Render the back quad
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -1000.0f, 550.0f,  -FARPLANE );
        glTexCoord2f(1, 0); glVertex3f(  1000.0f, 550.0f,  -FARPLANE );
        glTexCoord2f(1, 1); glVertex3f(  1000.0f,  -450.0f,  -FARPLANE );
        glTexCoord2f(0, 1); glVertex3f( -1000.0f,  -450.0f,  -FARPLANE );

    glEnd();

    // Render the right quad
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( 1000.0f, 550.0f, -FARPLANE );
        glTexCoord2f(1, 0); glVertex3f( 1000.0f, 550.0f,  -0.0f );
        glTexCoord2f(1, 1); glVertex3f( 1000.0f,  -450.0f,  -0.0f );
        glTexCoord2f(0, 1); glVertex3f( 1000.0f,  -450.0f, -FARPLANE );
    glEnd();

    // Render the top quad
    glBindTexture(GL_TEXTURE_2D, textures[6]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -1000.0f,  550.0f, -FARPLANE );
        glTexCoord2f(1, 0); glVertex3f( 1000.0f,  550.0f,  -FARPLANE );
        glTexCoord2f(1, 1); glVertex3f(  1000.0f,  450.0f,  0.0f );
        glTexCoord2f(0, 1); glVertex3f(  -1000.0f,  450.0f, 0.0f );
    glEnd();


    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);



    glLoadIdentity();

	Camera.Render();



    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
	
	// give us some visual orientation
	drawskybox();

    // draw the grass   -   this really needs to be fixed
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
        glTexCoord2f (0, 0);
        glVertex3f(-1000,-1,0);
        glTexCoord2f (1, 0);
        glVertex3f(1000,-1,0);
        glTexCoord2f (1, 1);
        glVertex3f(1000,-1,-FARPLANE);
        glTexCoord2f (0, 1);
        glVertex3f(-1000,-1,-FARPLANE);
    glEnd();

    // draw the runway
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(-2, -1, 0);
        glTexCoord2f(1, 0);
        glVertex3f(2, -1, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-2, -1, -FARPLANE);
        glTexCoord2f(1, 0);
        glVertex3f(-2, -1, -FARPLANE);
        glTexCoord2f(0, 1);
        glVertex3f(2, -1, 0);
        glTexCoord2f(1, 1);
        glVertex3f(2, -1, -FARPLANE);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    // draw yellow lines
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1,1,0);
    float z;
    for(z = -2; z > -FARPLANE; z-=1.5)
    {
        glBegin(GL_QUADS);
            glVertex3f(-0.125f, -1, z);
            glVertex3f(0.125f, -1, z);
            glVertex3f(0.125f, -1, z-1);
            glVertex3f(-0.125f, -1, z-1);
        glEnd();
    }

    // make a simple line of trees
    for(z = -25; z >= -FARPLANE + 75; z-=25)
    {
        glPushMatrix(); // save what we've done so FARPLANE
        glColor3f(1,1,0);
        glTranslatef(-2.5,-1,z);
        glRotated(90, -1, 0, 0);
        glutSolidCone(0.25, 2, 50, 50);    // base radius, height, slices, stack
        glPopMatrix();

        glPushMatrix(); // save what we've done so FARPLANE
        glColor3f(0,1,0);
        glTranslatef(-2.5,0.5,z);
        glutSolidSphere(1,50,50);
        glPopMatrix();
    }

    // make a simple line of trees
    for(z = -12.5; z >= -FARPLANE + 62.5; z-=25)
    {
        glPushMatrix(); // save what we've done so FARPLANE
        glColor3f(1,1,0);
        glTranslatef(2.5,-1,z);
        glRotated(90, -1, 0, 0);
        glutSolidCone(0.25, 2, 50, 50);    // base radius, height, slices, stack
        glPopMatrix();

        glPushMatrix(); // save what we've done so FARPLANE
        glColor3f(0,1,0);
        glTranslatef(2.5,0.5,z);
        glutSolidSphere(1,50,50);
        glPopMatrix();
    }



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
        Camera.MoveUpward(0.3);
    else
    if(key == GLUT_KEY_DOWN)
        Camera.MoveUpward(-0.3);
    else
    if(key == GLUT_KEY_LEFT)
        Camera.StrafeRight(-1.0);
	else
	if(key == GLUT_KEY_RIGHT)
	    Camera.StrafeRight(1.0);
}

static void idle(void)
{
    keyOperations();
    Camera.MoveForward(speed * SPEED);
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
