/*----------------------------------------------------------
*  COSC363 2020 
*  Assignment 2: Ray Tracer
*  Cylinder class
*
*  Kayle Ransby
*  34043590  
*-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>


/**
* Cylinder's intersection method.  The input is a ray.
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 d)
{
    // Intersection equation: (lecture 08, slide 38)
    float a = (d.x * d.x) + (d.z * d.z);
    float b = 2 * (d.x * (p0.x - center.x) + d.z * (p0.z - center.z));
    float c = ((p0.x - center.x) * (p0.x - center.x)) + ((p0.z - center.z) * (p0.z - center.z)) - (radius * radius);

    // quadratic equation:
    float delta = (b * b) - 4 * (a * c);
    if (fabs(delta) < 0.001) return -1.0;
    if (delta < 0.0) return -1.0;

    // Solve quadratic equation
    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float t;

    if (t1 < t2) t = t1;
    else t = t2;

    float p = p0.y + t * d.y;

    if ((p >= center.y) && (p <= center.y + height)) // Render front
    {
        return t1;
    }
    else if ((p > center.y + height)) // Render back
    {
        if ((p0.y + t2 * d.y) <= center.y + height)
        {
            if (isCapped) return t2 - t1; // Render cap
            else return t2; // Render back
        }
    }
    else
    {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    //glm::vec3 normal((p.x - center.x) / radius, 0, (p.z - center.z) / radius); // causes weird noise
    glm::vec3 normal((p.x - center.x), 0, (p.z - center.z));
    normal = glm::normalize(normal);
    return normal;
}

void Cylinder::getTexCoord(glm::vec3 p, float& texcoords, float& texcoordt)
{
    glm::vec3 dHat = glm::normalize(center - p);

    float u;
    float t;

    u = 0.5 + (glm::atan(dHat.x, dHat.z) / (2 * 3.1415));
    t = (p.y - center.y) / height;

    texcoords = u;
    texcoordt = t;
}