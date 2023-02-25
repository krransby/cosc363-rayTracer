/*----------------------------------------------------------
*  COSC363 2020
*  Assignment 2: Ray Tracer
*  Cone header
*
*  Kayle Ransby
*  34043590
*
-------------------------------------------------------------*/

#ifndef CONE_H
#define CONE_H
#include <glm/glm.hpp>
#include "SceneObject.h"


class Cone : public SceneObject {

private:
    glm::vec3 center = glm::vec3(0);
    float radius = 1;
    float height = 1;
    bool isCapped = false;

public:

    Cone() {}; // Default constructor creates a unit cone

    Cone(glm::vec3 c, float r, float h) : radius(r), height(h), center(c) {};

    Cone(glm::vec3 c, float r, float h, bool cap) : radius(r), height(h), center(c), isCapped(cap) {};

    float intersect(glm::vec3 p0, glm::vec3 d);

    glm::vec3 normal(glm::vec3 pt);

    float halfAngle(void);

    void getTexCoord(glm::vec3 p, float& texcoords, float& texcoordt) override;

};

#endif //CONE_H