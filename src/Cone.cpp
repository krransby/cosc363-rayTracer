/*----------------------------------------------------------
*  COSC363 2020
*  Assignment 2: Ray Tracer
*  Cone class
*
*  Kayle Ransby
*  34043590
*-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>


/**
* Cone's intersection method.  The input is a ray.
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 d)
{
    // Intersection equation: (lecture 08, slide 45)
    float a = (d.x * d.x) + (d.z * d.z) - (((radius / height) * (radius / height)) * (d.y * d.y));
    float b = (2 * ((p0.x - center.x) * d.x)) + (2 * ((p0.z - center.z) * d.z)) + (2 * (((radius / height) * (radius / height)) * (height - p0.y + center.y) * d.y));
    float c = ((p0.x - center.x) * (p0.x - center.x)) + ((p0.z - center.z) * (p0.z - center.z)) - (((radius / height) * (radius / height)) * ((height - p0.y + center.y) * (height - p0.y + center.y)));

    // Quadratic equation:
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

    if ((p >= center.y) and (p <= center.y + height)) // Render front
    {
        return t;
    }
    else if ((p < center.y) && isCapped) // Render cap
    {
        if (((p0.y + t1 * d.y) < center.y) && ((p0.y + t2 * d.y) >= center.y))
        {
            return t2 - t1;
        }

    }
    else
    {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    float alpha = glm::atan((p.x - center.x) / (p.z - center.z));

    float theta = halfAngle();
    
    glm::vec3 normal(glm::sin(alpha) * glm::cos(theta), glm::sin(theta), glm::cos(alpha) * glm::cos(theta));

    return normal;
}

float Cone::halfAngle(void) 
{

    glm::vec3 centerVec = glm::normalize(center - (center + glm::vec3(0.0, height, 0.0)));
    //glm::vec3 angleVec = glm::normalize((center - radius) - (center + glm::vec3(0.0, height, 0.0)));
    glm::vec3 angleVec = glm::normalize(glm::vec3(center.x - (radius / 2), center.y, center.z - (radius / 2)) - (center + glm::vec3(0.0, height, 0.0)));

    float halfAngle = glm::acos(glm::dot(centerVec, angleVec));

    return halfAngle;
}

void Cone::getTexCoord(glm::vec3 p, float& texcoords, float& texcoordt)
{
    //TODO
}