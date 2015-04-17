#include <math.h>
#define _USE_MATH_DEFINES



double degreeRadian(double degree)
{
    return M_PI * degree / 180.0;
}

double radianDegree(double radian)
{
    return radian * 180.0 / M_PI;
}

