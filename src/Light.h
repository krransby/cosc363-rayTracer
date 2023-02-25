/*----------------------------------------------------------
*  COSC363 2020
*  Assignment 2: Ray Tracer
*  Light header
*
*  Kayle Ransby
*  34043590
*
-------------------------------------------------------------*/

#ifndef H_LIGHT
#define H_LIGHT
#include <glm/glm.hpp>

/**
 * Defines a simple light located at 'position'
 */
class Light
{
protected:
	glm::vec3 position = glm::vec3(1.0);  // Light's position
	
	// Spot light things
	bool isSpot = false;
	glm::vec3 direction = glm::vec3(0, -1, 0); // Default direction is down
	double cutoffAngle = 0.174533; // Default of 10 degrees

public:
	Light() {}
	
	Light(glm::vec3 pos) : position(pos) {} // constructor for standard light

	Light(glm::vec3 pos, glm::vec3 dir, double cutoff) : position(pos), direction(glm::normalize(dir)), cutoffAngle(glm::radians(cutoff)), isSpot(true) {} // constructor for spot light

	glm::vec3 getPosition();

	glm::vec3 getDirection();

	double getCutoffAngle();

	bool isSpotLight();

	bool inSpotLight(glm::vec3 shadowDirection);

};

#endif //!H_LIGHT