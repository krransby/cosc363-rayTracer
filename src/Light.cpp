/*----------------------------------------------------------
*  COSC363 2020
*  Assignment 2: Ray Tracer
*  Light class
*
*  Kayle Ransby
*  34043590
*
-------------------------------------------------------------*/

#include "Light.h"

glm::vec3 Light::getPosition() 
{
	return position;
}

glm::vec3 Light::getDirection() 
{
	return direction;
}

double Light::getCutoffAngle() 
{
	return cutoffAngle;
}

bool Light::isSpotLight() 
{
	return isSpot;
}

bool Light::inSpotLight(glm::vec3 shadowDirection) 
{
	return (glm::acos(glm::dot(-shadowDirection, direction)) <= (float)cutoffAngle);
}