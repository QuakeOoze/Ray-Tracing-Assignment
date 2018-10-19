/*-------------------------------------------------------------
* COSC363 Assignment
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float Ex = posn.x-center.x; //X distance from eye to center of cylinder
    float Ez = posn.z-center.z;
    float Ey = posn.y-center.y;
    
    float k = (radius/height)*(radius/height);
    
    float a = (dir.x*dir.x) + (dir.z*dir.z) - k*(dir.y*dir.y);
    float b = 2*(dir.x*Ex + dir.z*Ez - k*(dir.y*Ey));
    float c = Ex*Ex + Ez*Ez - k*(Ey*Ey);
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
    
    
    float ymin = posn.y + dir.y*tmin + height;
    float ymax = posn.y + dir.y*tmax + height;
    
    if (ymin <= center.y + height && ymin >= center.y) {
        
        return tmin;
        
        
    } else if (ymax <= center.y + height && ymax >= center.y) {

        float t3 = (height - (posn.y-center.y))/dir.y;
        return t3;                                    
        
    } else {
        
        return -1.0;
    
    }
}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    float theta = atan(radius/height);
    
    glm::vec3 d = p - center;
    float alpha = atan(d.x/d.z);
    
    glm::vec3 n(sin(alpha)*cos(theta), sin(theta), cos(alpha)*cos(theta));
    n = glm::normalize(n);
    return n;
}
