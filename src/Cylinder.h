/*----------------------------------------------------------
*  COSC363 2020
*  Assignment 2: Ray Tracer
*  Cylinder header
*
*  Kayle Ransby
*  34043590
*
-------------------------------------------------------------*/

#ifndef H_CYLINDER
#define H_CYLINDER
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple cylinder located at 'position'
 * with the specified radius and height
 */
class Cylinder : public SceneObject
{

private:
    glm::vec3 center = glm::vec3(0);
    float height = 1;
    float radius = 1;
    bool isCapped = false;

public:
    Cylinder() {};  //Default constructor creates a unit cyliner

    Cylinder(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) {}

    Cylinder(glm::vec3 c, float r, float h, bool cpd) : center(c), radius(r), height(h), isCapped(cpd) {}

    float intersect(glm::vec3 p0, glm::vec3 d);

    void getTexCoord(glm::vec3 p, float& texcoords, float& texcoordt) override;

    glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER
