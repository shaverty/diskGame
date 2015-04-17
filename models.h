#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <map>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include "3DVector.h"
#include <list>
#include <deque>

using namespace std;

//class Disc();


#define SOLID_CLOSED_CYLINDER(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS) \
gluCylinder(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS); \
glRotatef(180, 1,0,0); \
gluDisk(QUAD, 0.0f, BASE, SLICES, 1); \
glRotatef(180, 1,0,0); \
glTranslatef(0.0f, 0.0f, HEIGHT); \
gluDisk(QUAD, 0.0f, TOP, SLICES, 1); \
glTranslatef(0.0f, 0.0f, -HEIGHT);


const float flatblack[4][4]
{
    {0.1, 0.1, 0.1, 1.0},
    {0.1, 0.1, 0.1, 1.0},
    {0.1, 0.1, 0.1, 1.0},
    {0.0}
};
const float flatred[4][4]
{
    { 0.3, 0.0, 0.0, 1.0 },
    {0.9, 0.0, 0.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    {0.0}
};
const float shinygreen[4][4]
{
    {0.0, 0.3, 0.0, 1.0},
    {0.0, 0.9, 0.0, 1.0},
    {0.2, 1.0, 0.2, 1.0},
    {8.0}
};
const float flatblue[4][4]
{
    {0.0, 0.0, 0.5, 1.0},
    {0.0, 0.0, 0.8, 1.0},
    {0.2, 0.2, 1.0, 1.0},
    {0.0}
};
const float flatteal[4][4]
{
    {0.0, 0.5, 0.5, 1.0},
    {0.0, 0.0, 0.8, 1.0},
    {0.2, 0.2, 1.0, 1.0},
    {0.0}
};
const float flatyellow[4][4]
{
    {0.9, 1.0, 0.5, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    {0.2, 0.2, 0.0, 1.0},
    {0.0}
};



void setMaterial(const float material[4][4])
{
    const float *ambient;
    const float *diffuse;
    const float *specular;
    const float *shininess;
    ambient = material[0];
    diffuse = material[1];
    specular = material[2];
    shininess= &material[3][0];

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

class Model
{
    public:
    Vector position;
    bool flag =true;

    Model(Vector pos = Vector())
    {
        position = pos;
        flag = true;
    }
    ~Model(){};

    void translate()
    {
        glTranslatef(position.x,position.y,position.z);
    }

    void move(Vector vec)
    {
        position += vec;
    }

    void draw()
    {
        throw 20;
    }

    template <class T> void gravity(deque<T*> objects)
    {
        return;
    }
    bool contains(int worldX, int worldY)
    {
        return false;
    }

    bool grab(){
        return false;
    }
    bool release(){
        return false;
    }

};


class Peg : public Model
{
    public:

    double radius = 0.10;
    double height = 0.10;
    int slices = 20;
    int stacks = 20;

    Peg(Vector posit = Vector() ) : Model(posit)
    {

    }

    void draw()
    {
        glPushMatrix();
        translate();
        setMaterial(shinygreen);
        GLUquadricObj *qobj= gluNewQuadric();
        gluQuadricDrawStyle(qobj, GLU_FILL);
        SOLID_CLOSED_CYLINDER(qobj, radius, radius, height,slices, stacks);
        //gluCylinder(qobj, radius, radius, height, stacks, slices);
        glPopMatrix();

    }

    bool contact(double rad, Vector pos)
    {
        Vector distance = position - pos;

        if (distance.length() < (rad + radius) )
            return true;
        else
            return false;
    }

    double collide(double rad, Vector pos, Vector &velo, double speed)
    {
        Vector difference = pos - position;
        //cout << pos.x << " " << position.x<< endl;

        if (difference.x ==0)
        {
            if (rand() %2)
                difference.x = 0.05;
            else
                difference.x = -0.05;
        }
            difference.normalize();
            //cout << difference.x << " "<<difference.y << " "<<difference.z << endl;
            velo += difference;

            return speed * 0.90;


    }
};


class Wall : public Model
{
    public:
    double width, height, depth;

    Wall(Vector pos = Vector(), double width = 0.5, double height = 10, double depth = 0.5)
    {
        this->position = pos;
        this->width = width;
        this->height = height;
        this->depth = depth;
    }

    void draw()
    {
        glPushMatrix();
        translate();
        setMaterial(flatblue);
        glScalef(width, height, depth);
        glutSolidCube(1);
        glPopMatrix();
    }
};


class VerticalWall : public Wall
{
   public:
    VerticalWall(Vector pos = Vector(), double width=0.5, double height=10, double depth=0.5): Wall(pos, width, height, depth)
    {

    }

    bool contact(double rad, Vector pos)
    {
        if ( (position.y + height) / 2 > pos.y > (position.y-height) /2 )
        {
            double distance = fabs(position.x - pos.x);

            ///issues
            if (distance < rad + width / 2)
                return true;
        }
        return false;
    }

    double collide(Vector &velo, double speed)
    {
        velo.x = -1.0*velo.x;
        return speed *0.9;
    }
};

class HorizontalWall : public Wall
{
    public:

    HorizontalWall(Vector pos = Vector(), double width=0.5, double height=10, double depth=0.5): Wall(pos, width, height, depth)
    {

    }

    bool contact(double rad, Vector pos)
    {
        if ( (position.x + width) /2 > pos.x > (position.x - width) /2 )
        {
            double distance = fabs(position.y - pos.y);

            if (distance < rad + height /2)
                return true;
        }

        return false;
    }

    double collide(bool &flag, Vector &velo)
    {
        velo.set(0.0,0.0,0.0);
        flag = true;
        return 0;
    }

    /*double collide(Disc &disc)
    {
        disc.velocity.set(0,0,0);
        disc.flag = true;

        return 0;
    }*/
};



class Disc : public Model
{
    public:
    bool flag = true;
    double radius =.35;
    double height = .1;
    int slices = 20;
    int stacks = 20;
    Vector velocity;

    Disc(Vector posit = Vector()) : Model(posit)
    {
        velocity = Vector();
    }

    void draw()
    {
        glPushMatrix();
        translate();
        setMaterial(flatred);
        GLUquadricObj *qobj= gluNewQuadric();
        gluQuadricDrawStyle(qobj, GLU_FILL);
        gluQuadricOrientation(qobj, GLU_INSIDE);
        SOLID_CLOSED_CYLINDER(qobj, radius, radius, height,slices, stacks);
        //gluCylinder(qobj, radius, radius, height, stacks, slices);
        //glutSolidCylinder(radius, height, slices, stacks);

        glPopMatrix();
    }

    ///issues issues issues
    void gravity(deque<Peg*> pegs, deque<VerticalWall*> walls, HorizontalWall* bottomWall)
    {
        if (!flag)
        {
            float speed = 0.05;
            for(deque<Peg*>::iterator it = pegs.begin(); it != pegs.end(); it++)
            {
                if ((*it)->contact(radius, position))
                {
                    speed = (*it)->collide(radius, position, velocity, speed);

                }

            }
            for(deque<VerticalWall*>::iterator it = walls.begin(); it != walls.end(); it++)
            {
                if ((*it)->contact(radius, position))
                    speed = (*it)->collide(velocity, speed);
            }

            if (bottomWall->contact(radius, position))
                speed = bottomWall->collide(flag,velocity);

            Vector temp = Vector(0, -0.025, 0);
            velocity += temp;
            velocity.normalize(speed);
            position += velocity;

            //cout << "Disc new position ("<< position.x << ", " << position.y<< ", " << position.z << ")"<< endl;
        }

    }

    bool contains(int worldX, int worldY)
    {
        Vector distance = Vector(worldX, worldY, 0);
        distance = position - distance;
        distance.z = 0;

        if (distance.length() < radius)
            return true;
        else
            return false;

    }

    bool grab()
    {
        flag = true;
        return flag;
    }

    bool release()
    {
        flag = false;
        return flag;
    }

    template <class T> bool contact(T* obstacle)
    {
        Vector distance = position - obstacle.position;
        if (distance.length() < (radius+obstacle.radius) )
            return true;
        else
            return false;
    }

};




class GameBoard : public Model
{
    public:
    deque<Peg*> pegs;
    deque<VerticalWall*> Vwalls;
    HorizontalWall *bottomWall;

    double bottom, top,left, right;
    GameBoard(Vector position = Vector(), deque<Peg*>pegsL = deque<Peg*>(),deque<VerticalWall*> wallsL = deque<VerticalWall*>(),
              double bottom = -5, double top = 5, double left =-3, double right =3 )
              {
                  this->position = position;
                  this->bottom = bottom;
                  this->top = top;
                  this->left = left;
                  this->right = right;

                  if (pegsL.empty())
                    generatePegs();
                  else
                    pegs = pegsL;


                  if (wallsL.empty())
                    generateWalls();
                  else
                    Vwalls = wallsL;
              }

    void generateWalls()
    {
        double height    = top-bottom;
        double yMid      = (bottom + top) / 2 + position.y;
        double width     = right-left +1.5;
        double xMid      = (left+right)/2 + position.x -0.5;

        Vector leftPos   = Vector(left-1.25+position.x,
                                    yMid, position.z);
        Vector rightPos  = Vector(right + 0.25 + position.x,
                                  yMid, position.z);
        Vector bottomPos = Vector(xMid, bottom-0.25, position.z);

        VerticalWall* leftWall      = new VerticalWall(leftPos, 0.5, height);
        VerticalWall* rightWall     = new VerticalWall(rightPos, 0.5, height);
        bottomWall  = new HorizontalWall( bottomPos, width, 0.5 );

        Vwalls.push_back(leftWall);
        Vwalls.push_back(rightWall);
        //walls.push_back(bottomWall);

    }

    void generatePegs()
    {
        double offsetX = position.x;
        double offsetY = position.y;
        double z = position.z;

        for (int row = bottom; row < top; row++)
        {
            if ((int)row % 2 == 0)
            {
                for (double column = left-1; column < right; column++)
                {
                    Peg *p1 = new Peg(Vector(column + offsetX +.5, row + offsetY, z));
                    pegs.push_back(p1);
                }
            }
            else
            {
                for (double column = left; column < right; column ++)
                {
                    Peg *p2 = new Peg(Vector(column + offsetX, row + offsetY, z));
                    pegs.push_back(p2);
                }
            }
        }
    }


    void draw()
    {
        for (deque<Peg*>::iterator it = pegs.begin(); it != pegs.end(); it++)
            (*it)->draw();

        for (deque<VerticalWall*>::iterator it = Vwalls.begin(); it != Vwalls.end(); it++)
            (*it)->draw();

        bottomWall->draw();
    }

    deque<Peg*> getPegs()
    {
        return pegs;
    }

    deque<VerticalWall*> getVWalls()
    {
        return Vwalls;
    }

    HorizontalWall* getHWall()
    {
        return bottomWall;
    }

};






