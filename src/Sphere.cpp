/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>
#define EPSILON 0.01


/**
* Sphere's intersection method.  The input is a ray. 
*/
float Sphere::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
    float b = glm::dot(dir, vdif);
    float len = glm::length(vdif);
    float c = len*len - radius*radius;
    float delta = b*b - c;
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    if(fabs(t1) < EPSILON)
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < EPSILON) t2 = -1.0;

	return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    n = glm::normalize(n);
    return n;
}

void Sphere::getTexCoord(glm::vec3 p, float& texcoords, float& texcoordt)
{
    glm::vec3 dHat = glm::normalize(center - p);

    float u;
    float v;

    u = 0.5 + (glm::atan(dHat.x, dHat.z) / (2 * 3.1415));
    v = 0.5 - (glm::asin(dHat.y) / 3.1415);

    texcoords = u;
    texcoordt = v;
}