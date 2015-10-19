#include <math.h>
#include <algorithm>
#include <iostream>

using namespace std;

class Vector
{
    public:
        int x, y, z,xPos,yPos,zPos;

        Vector(int x1=0, int y1=0, int z1=0, int xPos1=0, int yPos1=0, int zPos1=0)
        {

          x = x1;
          y = y1;
          z = z1;
          xPos = xPos1;
          yPos = yPos1;
          zPos = zPos1;
        }
        ~Vector(){}

        void set(int x1,int y1,int z1)
        {
          x = x1;
          y = y1;
          z = z1;
        }

        double length()
        {
          return sqrt(x*x + y*y + z*z);
        }

        double dotProduct(Vector &other)
        {
          return x*other.x + y*other.y + z*other.z;
        }

        Vector crossProduct(Vector &other)
        {
            return Vector(y*other.z - z*other.y,
                            z*other.x - x*other.z,
                            x*other.y - y* other.x);

        }

        void translate(int x1,int y1,int z1)
        {
            xPos += x1;
            yPos += y;
            zPos += z;
        }

        void bindZ()
        {
            double temp = 1.0 - x * x-y*y;
            z = max(temp, 0.0);
        }

        void normalize(double scale = 1.0)
        {
            double len = length();
            //cout << len << endl;
            if (len == 0)
                return;
            if (scale == 0)
                scale = 0.0001;

            x /= (len / scale);
            y /= (len / scale);
            z /= (len / scale);

            //cout << "end normalize " << length() << endl;
        }

        void scaleOne()
        {
            //cout << "scale len: " << length() << endl;
            if (length() > 1.0)
                normalize();
        }


        Vector operator + (Vector &other)
        {
            Vector ret(x+other.x, y + other.y, z + other.z);
            return ret;
        }



        void operator +=(Vector &other)
        {
            x += other.x;
            y +=other.y;
            z += other.z;
        }

        Vector operator - (Vector& other)
        {
            Vector ret(x - other.x, y - other.y, z - other.z);
            return ret;
        }


        void operator -=(Vector &other)
        {
            x -= other.x;
            y -=other.y;
            z -= other.z;
        }

        Vector operator *(int multiplier)
        {
            Vector ret(x * multiplier, y * multiplier, z * multiplier);
            return ret;
        }

        void operator *=(int multiplier)
        {
            x *= multiplier;
            y *= multiplier;
            z *= multiplier;
        }

        bool operator ==(Vector &other)
        {
            if (x == other.x &&
                y == other.y &&
                xPos == other.xPos &&
                yPos == other.yPos &&
                zPos == other.zPos)
                return true;
            else
                return false;
        }

};

