#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include "utils.h"
#include "models.h"
//#include "3DVector.h"
#include <list>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/any.hpp>

using namespace std;


///Globals
int globalWidth = 600;
int globalHeight = 600;
int fps = 60;
bool reset = false;

GLuint selectBuf[512];
GLdouble mvMatrix[16], projMatrix[16];
GLint viewport[4];

double lastWorldX=0, lastWorldY=0, lastWorldZ=0;

//object containers
deque<Model*>   modelsDeque;
deque<Model*>   obstacleDeque;

deque<Peg*>     pegDeque;
deque<VerticalWall*>    wallDeque;
deque<Disc*>    discDeque;
deque<Disc*>   picked;
GameBoard* board;
HorizontalWall * bottomWall;

//camera stuff
double thetaResolution = 272.0;
double phiResolution = 0.0;

Vector camera = Vector(0.0, 1.0, 10.0);
Vector view   = Vector(0.0, 1.0, 9.0);
Vector up     = Vector(0.0, 1.0, 0.0);

float xPos = 0;
float yPos = -0.5;
float lastMouseX =0.0;
float lastMouseY = 0.0;
float lastMouseZ = 0.0;

//Lighting
float lightPositon[4]  = { 1.0, 20.0, 1.5, 1.0 };
float lightAmbient[4]  = { 1.0, 1.0, 1.0, 1.0 };
float lightDiffuse[4]  = { 0.6, 0.6, 0.6, 1.0 };
float lightSpecular[4] = { 0.8, 0.8, 0.8, 1.0};



//functions
void processHits(GLint hits, GLuint buffer[]);



#define WorldW 2 *(float)globalWidth / (float)globalHeight
#define WorldH 2.0


inline float p2w_x(float w) {
   //float WorldW = 2 * (float) GW / (float) GH;
   return WorldW / globalWidth  * w - WorldW / 2;
}

inline float p2w_y(float h) {
   return -1 * (WorldH / globalHeight * h - WorldH / 2);
}



void init()
{
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
}

void drawObjects()
{

    glPushMatrix();
    int id = 0;

    for (deque<Disc*>::iterator it = discDeque.begin(); it != discDeque.end(); it++)
    {
        glLoadName(id);
        id++;
        (*it)->draw();
    }


    glLoadName(id);
    id++;
    board->draw();

    glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);


	glPushMatrix();
	{
	    gluLookAt(camera.x, camera.y, camera.z, view.x, view.y, view.z, up.x, up.y, up.z);
	    glLightf(GL_LIGHT0, GL_POSITION, *lightPositon);

	    glPushMatrix();
	    {
	        glTranslatef(xPos, yPos, 0);
	    }
		glPopMatrix();


		drawObjects();

	}
	glPopMatrix();

	glutSwapBuffers();
}

 void list_hits(GLint hits, GLuint *names)
 {
 	int i;

 	/*allocate 4 bytes for each hit:
    1. Number of hits (always 1)
    2. Min Z (broken)
    3. max Z (broken)
    4. Name of the object hit (via glLoadName)
 	*/

 	//printf("%d hits:\n", hits);

 	for (i = 0; i < hits; i++)
 	{
        lastMouseZ = names[i*4+1];
        int id = (GLubyte)names[i * 4 + 3];
        if (id != 4 && discDeque[id]->grab())
        {
            picked.push_back(discDeque[id]);
        }
        //cout << id << endl;
 	}


 	//printf("\n");
 }

void Pick(int x, int y)
{
    GLint hits, Viewport[4];
    unsigned int buffer[64];

    glSelectBuffer(64, buffer);
    //init();
    glGetIntegerv(GL_VIEWPORT, Viewport);
    glRenderMode(GL_SELECT);
    glInitNames();

    glPushName(0xfffffff);      //fill stack with one element (or glLoadName will generate an error)


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        glLoadIdentity();
        gluPickMatrix(x, Viewport[3]-y, 1,1, Viewport);
        gluPerspective(90, (float)globalWidth/(float)globalHeight, 1, 15);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            gluLookAt(camera.x, camera.y, camera.z, view.x, view.y, view.z, up.x, up.y, up.z);
            drawObjects();
        }
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    }
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glFlush();

    hits = glRenderMode(GL_RENDER);
    //printf("number of hits %i\n", hits);



    if(hits != 0)
    {
        //lastMouseZ = buffer[1];
        list_hits(hits, buffer);
        //printf("min_depth: %f", lastMouseZ);
        //processHits(hits, buffer);
    }







}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            printf("mouse clicked at %d %d (%f, %f)\n", x, y, p2w_x(x), p2w_y(y) );
            Pick(x,y);

            if (not picked.empty())
            {
                //init();
                float winX= (float)x;
                float winY= (float)y;
                GLdouble posX=0, posY=0, posZ=0;
                GLfloat z_cursor;

                glReadPixels(winX, winY, 1 ,1, GL_DEPTH_COMPONENT, GL_FLOAT,&z_cursor);

                //cout << z_cursor << endl;

                lastMouseZ = z_cursor;
                gluUnProject(winX,winY,z_cursor, mvMatrix,
                            projMatrix, viewport, &posX, &posY, &posZ);
                lastWorldX = posX;
                lastWorldY = posY;
                lastWorldZ = posZ;
                printf ("clicked %f, %f, %f", lastWorldX, lastWorldY, lastWorldZ);
            }


            //move = true;

        }
        if(state == GLUT_UP)
        {
            if (not picked.empty())
            {
                picked[0]->release();
                picked.clear();
            }
        }
    }
}

void mouseMove(int x, int y)
{
    if (not picked.empty() )
    {
        float winX= (float)x;
        float winY= (float)y;
        GLdouble posX=0, posY=0, posZ=0;
        GLfloat z_cursor;

        glReadPixels(winX, winY, 1 ,1, GL_DEPTH_COMPONENT, GL_FLOAT,&z_cursor);

        //cout << z_cursor << endl;


        gluUnProject(winX,winY,z_cursor, mvMatrix,
             projMatrix, viewport, &posX, &posY, &posZ);

        double xMove = lastWorldX - posX;
        double yMove = lastWorldY - posY;
        lastWorldX = posX;
        lastWorldY = posY;
        lastWorldZ = posZ;

        picked[0]->move(Vector(-6.9*xMove, 6.9*yMove, 0));

        glutPostRedisplay();
    }
}


void reshape(int w, int h) {
  globalWidth= w;
  globalHeight = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90, (float)w/(float)h, 1, 15);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);

  glutPostRedisplay();

}

void init_lighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_MODELVIEW);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

void init_objects()
{
    for (int i = 0; i <4; i++)
    {
        Disc* disc= new Disc(Vector(-6, i, 3));
        modelsDeque.push_back(disc);
        discDeque.push_back(disc);
    }

    board = new GameBoard(Vector(0,1,3));

    pegDeque = board->getPegs();
    deque<Model*>::iterator it = obstacleDeque.end();
    obstacleDeque.insert(it, pegDeque.begin(), pegDeque.end());


    wallDeque = board->getVWalls();
    bottomWall = board->getHWall();
    deque<Model*>::iterator itr = obstacleDeque.end();
    obstacleDeque.insert(itr, wallDeque.begin(), wallDeque.end());

    modelsDeque.push_back(board);
}


void timer(int data)
{
    if (not reset)
    {
        for (deque<Disc*>::iterator it = discDeque.begin(); it != discDeque.end(); it++)
            (*it)->gravity(pegDeque, wallDeque, bottomWall);
        glutTimerFunc(data, timer, data);
        glutPostRedisplay();
    }
}


/*
void test()
{
    Vector vec2(8,9,10);
    Disc d1(vec2);

    using boost::any_cast;
    std::deque<boost::any> d_walls;

    VerticalWall *leftWall      = new VerticalWall(vec2, 0.5, 2);
    VerticalWall *rightWall     = new VerticalWall(vec2, 0.5, 2);
    HorizontalWall *bottomWall  = new HorizontalWall(vec2, 3, 0.5 );


    //cout << leftWall->height << endl;
    d_walls.push_back(leftWall);
    d_walls.push_back(rightWall);
    d_walls.push_back(bottomWall);

    d1.velocity.set(1,1,1);
    //cout << d1.velocity.x << endl;
    //d_walls[0]->collide(d1);

    VerticalWall* lef;
    lef = d_walls[0];
    using namespace std;
    cout << d1.velocity.x << endl;
}
/* */


void cameraClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
            {
                lastMouseX = x;
                lastMouseY = y;
            }
    }
}

void cameraMove(int x, int y)
{
    float scale = .5;
    thetaResolution-= scale*(lastMouseX -x);
    phiResolution+= scale*(lastMouseY-y);

    phiResolution = min(phiResolution, 50.0);
    phiResolution = max(phiResolution, -50.0);

    thetaResolution = thetaResolution > 360 ? thetaResolution : thetaResolution-360.0;
    thetaResolution = thetaResolution < 360 ? thetaResolution : thetaResolution+360.0;


    view.x = camera.x+ cos(degreeRadian(phiResolution)* cos(degreeRadian(thetaResolution)));
    view.y = camera.y + sin(degreeRadian(phiResolution));
    view.z = camera.z + cos(degreeRadian(phiResolution)) * cos (90-degreeRadian(thetaResolution));

    lastMouseX = x;
    lastMouseY =y;
    glutPostRedisplay();

}

bool mode = true;
void keyboard(unsigned char key, int x, int y)
{
    float scale = 0.1;

    Vector zoom = view - camera;;
    zoom.scaleOne();

    Vector strafe = zoom.crossProduct(Vector(0,1,0));

    switch( key )
    {
        case 'w' : case 'W':
            view += zoom*scale;
            camera+= zoom*scale;
            break;
        case 'a' : case 'A':
            view -= strafe*scale;
            camera-= strafe*scale;
            break;
        case 's':
            view -=zoom*scale;
            camera-=zoom*scale;
            break;
        case 'd':
            view+=strafe*scale;
            camera += strafe*scale;
            break;
        case 'm' : case 'M':
            if (mode)
            {
                glutMouseFunc( cameraClick );
                glutMotionFunc( cameraMove );
            }
            else
            {
                glutMouseFunc(mouse);
                glutMotionFunc(mouseMove);
            }
            mode = !mode;
            break;
        case 'q' : case 'Q':
            exit(0);
    }

}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(globalHeight, globalWidth);
    glutInitWindowPosition(450, 350);
    glutCreateWindow("cis441 - Final Project: Plinko");
    //glClearColor(1.0, 1.0, 1.0, 1.0);

    glutReshapeFunc( reshape );
    glutDisplayFunc( display);

    init();
    glutMouseFunc( mouse );
    glutMotionFunc( mouseMove );
    glutKeyboardFunc( keyboard );

    glEnable(GL_DEPTH_TEST);

    init_lighting();

    init_objects();

    glutTimerFunc(1000/fps, timer, 1000/fps);
    glutMainLoop();

}

