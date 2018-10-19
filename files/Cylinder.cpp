/*-------------------------------------------------------------
* COSC363 Assignment
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float Ex = posn.x-center.x; //X distance from eye to center of cylinder
    float Ez = posn.z-center.z;
    
    float a = (dir.x*dir.x) + (dir.z*dir.z);
    float b = 2*(dir.x*Ex + dir.z*Ez);
    float c = Ex*Ex + Ez*Ez - radius*radius;
    float delta = b*b - 4*a*c;
    
    if(fabs(delta) < 0.001 || delta < 0.0) return -1.0;
    
    float t1 = (-b + sqrt(delta))/(2*a);
    float t2 = (-b - sqrt(delta))/(2*a);
    float tmin;
    float tmax;

    
    if(t1 < t2) { 
        tmin = t1;
        tmax = t2;
    }
    else {
        tmin = t2;
        tmax = t1;
    }
    
    if(fabs(t1) < 0.001 || fabs(t2) <0.001)
    {
        return -1.0;
    }
    
    
    float ymin = posn.y + dir.y*tmin; //y value of the smaller root
    float ymax = posn.y + dir.y*tmax; //y value of the larger root
    
    
    if (ymin <= center.y +height && ymin >= center.y) { //Value is withing the cones specified height
        
        return tmin;
        
        
    } else if (ymax <= center.y + height && ymax >= center.y) { //If the ray hits the back of the cylinder
        //Calculate circle intersection. 
        //We do not need to check it becuase we know it intersects due the the previous 'if' statements
        float t3 = (height - (posn.y-center.y))/dir.y; 
        return t3;                                    
        
    } else { //It does not intersect
        
        return -1.0;
    
    }
}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
    glm::vec3 n = glm::vec3(d.x, 0, d.z);
    n = glm::normalize(n);
    return n;
}
