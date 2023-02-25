/*==================================================================================
* COSC 363 Computer Graphics (2020)
* Assignment 2: Ray Tracer
*
* Kayle Ransby
* 34043590
*===================================================================================*/

#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Light.h"
#include "SceneObject.h"
#include "TextureBMP.h"
#include "Ray.h"
#include <GL/freeglut.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int SSAA = 2; // 1 = off, 2x, 4x, max = 8x
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

const int numTextures = 3;
const char* texture_list[numTextures] = { "assets/table.bmp", "assets/earth.bmp", "assets/missing.bmp" };

vector<SceneObject*> sceneObjects;
vector<TextureBMP*> textures;
vector<Light*> lights;

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0.0);					// Background colour
	glm::vec3 color(0);								// Pixel color

	int fog_range[2] = { 40, -296 };				// z1, z2

	SceneObject* obj;

    ray.closestPt(sceneObjects);					// Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		// no intersection
	obj = sceneObjects[ray.index];					// object on which the closest point of intersection is found
	

	// Textures
	if (obj->isTextured()) 
	{
		// Add code for texture mapping here
		float texcoords;
		float texcoordt;
		glm::vec3 txColor;

		obj->getTexCoord(ray.hit, texcoords, texcoordt);
		
		if(texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1)
		{
			txColor = textures[obj->getTextureIndex()]->getColorAt(texcoords, texcoordt);
			obj->setColor(txColor);
		}
	}

	// Chequered floor
	if (ray.index == 0)
	{
		int boxSize = 5;
		glm::vec3 chequColor;

		int iz = (ray.hit.z / boxSize);
		int ix = (ray.hit.x / boxSize);

		if (ray.hit.x < 0) 
		{
			if ((iz + ix) % 2 == 0) chequColor = glm::vec3(0.937, 0.937, 0.231); // yellow
			else chequColor = glm::vec3(0.815, 0.235, 0.031); // red
		}
		else 
		{
			if ((iz + ix) % 2 == 0) chequColor = glm::vec3(0.815, 0.235, 0.031); // red
			else chequColor = glm::vec3(0.937, 0.937, 0.231); // yellow
		}

		obj->setColor(chequColor);
	}

	// Lighting
	float ambientTerm = 0.2;		// Ambient color of light
	float shininess = 50.0;			// Shininess amount
	float specularTerm = 0;			// Specular

	glm::vec3 normalVec = sceneObjects[ray.index]->normal(ray.hit); // Calculate normal vector

	// For each light in the scene
	for (int i = 0; i < lights.size(); i++) 
	{	
		// Variables for base colour
		glm::vec3 lightVec(lights[i]->getPosition() - ray.hit);
		lightVec = glm::normalize(lightVec);
		float lDotn = glm::dot(lightVec, normalVec);
		if (lDotn < 0) lDotn = 0.0;

		
		if (obj->isSpecular())
		{	
			// Variables for specular reflections
			glm::vec3 reflVector = glm::reflect(-lightVec, normalVec);
			reflVector = glm::normalize(reflVector);
			float rDotv = glm::dot(reflVector, -ray.dir);
			if (rDotv > 0) specularTerm = pow(rDotv, shininess);
		}
		

		// Create shadow ray in direction of current light
		Ray shadow(ray.hit, lightVec);
		shadow.closestPt(sceneObjects);

		// Check if the current light is a spotlight
		if (lights[i]->isSpotLight())
		{
			// Check if the current shadow direction is in the spotlight's cone
			if (lights[i]->inSpotLight(shadow.dir) && ray.hit.y >= -10) 
			{
				// Check that we are in the current light's shadow
				if (shadow.index >= 0 && shadow.dist < glm::distance(lights[i]->getPosition(), ray.hit)) 
				{
					// Check if the shadow should be lighter:
					bool lighterShadow = sceneObjects[shadow.index]->isRefractive() || sceneObjects[shadow.index]->isTransparent();

					// Make sure to not add shadow to transparent or refractive objects (causes a lot of noise!)
					bool notLighterShadow = sceneObjects[ray.index]->isRefractive() || sceneObjects[ray.index]->isTransparent();

					if (lighterShadow && !notLighterShadow)
					{
						ambientTerm = 0.5; // Shadow for transparent / refractive object
					}
					else
					{
						ambientTerm = 0.2; // shadow for regualr object
					}

					color += ambientTerm * obj->getColor();
				}
				else 
				{
					color += ambientTerm * obj->getColor() + (lDotn * 0.4f) * obj->getColor() + specularTerm * glm::vec3(1); // * 0.4f
				}
			}
		}
		else // Light is not a spotlight
		{
			// Check that we are in the current light's shadow
			if (shadow.index >= 0 && shadow.dist < glm::distance(lights[i]->getPosition(), ray.hit)) 
			{
				// Check if the shadow should be lighter:
				bool lighterShadow = sceneObjects[shadow.index]->isRefractive() || sceneObjects[shadow.index]->isTransparent();

				// Make sure to not add shadow to transparent or refractive objects (causes a lot of noise!)
				bool notLighterShadow = sceneObjects[ray.index]->isRefractive() || sceneObjects[ray.index]->isTransparent();

				if (lighterShadow && !notLighterShadow)
				{
					ambientTerm = 0.5; // Shadow for transparent / refractive object
				}
				else
				{
					ambientTerm = 0.2; // shadow for regualr object
				}

				color += ambientTerm * obj->getColor();
			}
			else 
			{
				color += ambientTerm * obj->getColor() + lDotn * obj->getColor() + specularTerm * glm::vec3(1);
			}
		}
	}

	
	// Transparency
	if (obj->isTransparent() && step < MAX_STEPS)
	{

		// Internal ray
		Ray internalRay(ray.hit, ray.dir);
		internalRay.closestPt(sceneObjects);

		glm::vec3 internalCol = trace(internalRay, step + 1);

		// External ray
		Ray externalRay(internalRay.hit, ray.dir);
		externalRay.closestPt(sceneObjects);
		glm::vec3 externalCol = trace(externalRay, step + 1);

		color = (color * (1 - obj->getTransparencyCoeff())) + (externalCol * obj->getTransparencyCoeff());
	}

	// Refractions
	if (obj->isRefractive() && step < MAX_STEPS)
	{
		float eta = 1.0f / obj->getRefractiveIndex();

		// Internal refeaction
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 refractedDir = glm::refract(ray.dir, normalVec, eta);
		Ray refrRay(ray.hit, refractedDir);
		refrRay.closestPt(sceneObjects);

		// External refraction
		glm::vec3 refractedNormalVec = obj->normal(refrRay.hit);
		glm::vec3 secondRefractedDir = glm::refract(refractedDir, -refractedNormalVec, 1.0f / eta);

		Ray refrRay2(refrRay.hit, secondRefractedDir);

		glm::vec3 refractedColor = trace(refrRay2, step + 1);
		color = (color * (1 - obj->getRefractionCoeff())) +  (refractedColor * obj->getRefractionCoeff());
	}

	// Reflections
	if (obj->isReflective() && step < MAX_STEPS) 
	{ 
		float rho = obj->getReflectionCoeff(); 
		glm::vec3 normalVec = obj->normal(ray.hit); 
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec); 
		Ray reflectedRay(ray.hit, reflectedDir); 
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor); 
	}

	// Fog
	float t = (ray.hit.z + fog_range[0]) / (fog_range[1] + fog_range[0]);
	color = (glm::vec3(1 - t) * color) + glm::dot(glm::vec3(t), backgroundCol);

	return color;
}


glm::vec3 SSAA_Filter(float xp, float yp, float cellX, float cellY, glm::vec3 eye, int current_steps)
{
	const int num_samples = SSAA * SSAA;

	int current_ray = 1;

	glm::vec3 col[num_samples] = {};

	glm::vec3 threshold(0.2);

	glm::vec3 average_col; // average colour vector

	// Generate rays
	while (current_ray <= num_samples)
	{
				
		float scale_factor = (0.5 / SSAA) * current_ray;

		// Generate a ray

		float xPos = xp + scale_factor * cellX;
		float yPos = yp + scale_factor * cellY;
		
		glm::vec3 dir(xPos, yPos, -EDIST);	//direction of the primary ray

		Ray ray = Ray(eye, dir);

		col[current_ray - 1] = trace(ray, 1); //Trace the primary ray and get the colour value

		// Set the current ray
		current_ray += 1;

	}

	// Loop through each cell division and get the average colour
	for (int i = 0; i < num_samples; i++)
	{
		glm::vec3 current_col = col[i];

		average_col += current_col;
	}

	average_col = average_col / glm::vec3(num_samples);

	if (current_steps < MAX_STEPS)
	{

		glm::vec3 upper_bound = average_col + threshold;
		glm::vec3 lower_bound = average_col - threshold;

		for (int i = 0; i < num_samples; i++)
		{

			bool filter_again = glm::any(glm::lessThan(col[i], lower_bound) && glm::greaterThan(col[i], upper_bound));

			if (filter_again)
			{
				float scale_factor = (0.5 / SSAA) * i;
				float xPos = xp + scale_factor * cellX;
				float yPos = yp + scale_factor * cellY;


				col[i] = SSAA_Filter(xPos, yPos, cellX, cellY, eye, current_steps++);

			}
		}

		average_col = glm::vec3(0.0);

		// Loop through each cell division and get the average colour
		for (int i = 0; i < num_samples; i++)
		{
			glm::vec3 current_col = col[i];

			average_col += current_col;
		}

		average_col = average_col / glm::vec3(num_samples);

	}
	

	return average_col;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  // grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  // cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  // cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);  // Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	// Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			
			yp = YMIN + j * cellY;

			glm::vec3 col;

			if (SSAA % 2 == 0 && SSAA <= 8) // supersampling
			{
				
				col = SSAA_Filter(xp, yp, cellX, cellY, eye, 1);

			}
			else // standard rendering
			{
				
				glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	// direction of the primary ray
				
				Ray ray = Ray(eye, dir);
				
				col = trace (ray, 1); // Trace the primary ray and get the colour value

			}
			
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				// Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

	glEnd();
	glFlush();
}


void pyrimid(glm::vec3 pos, glm::vec3 colour, int height, int size)
{

	float scale = size / 2;

	glm::vec3 pointA(pos.x - scale, pos.y, pos.z + scale);
	glm::vec3 pointB(pos.x + scale, pos.y, pos.z + scale);
	glm::vec3 pointC(pos.x + scale, pos.y, pos.z - scale);
	glm::vec3 pointD(pos.x - scale, pos.y, pos.z - scale);
	glm::vec3 pointE(pos.x, pos.y + height, pos.z);

	// Pyrimid
	// Bottom
	Plane* pyrimid_bottom = new Plane(pointA,	//Point A
									pointB,		//Point B
									pointC,		//Point C
									pointD);	//Point D
	pyrimid_bottom->setColor(colour);
	sceneObjects.push_back(pyrimid_bottom);

	// Back
	Plane* pyrimid_back = new Plane(pointC,		//Point C
									pointD,		//Point D
									pointE);	//Point E
	pyrimid_back->setColor(colour);
	sceneObjects.push_back(pyrimid_back);

	// Front
	Plane* pyrimid_front = new Plane(pointA,	//Point A
									pointB,		//Point B
									pointE);    //Point E
	pyrimid_front->setColor(colour);
	sceneObjects.push_back(pyrimid_front);

	// Left
	Plane* pyrimid_left = new Plane(pointD,		//Point D
									pointA,		//Point A
									pointE);	//Point E
	pyrimid_left->setColor(colour);
	sceneObjects.push_back(pyrimid_left);

	// Right
	Plane* pyrimid_right = new Plane(pointB,	//Point B
									pointC,		//Point C
									pointE);    //Point E
	pyrimid_right->setColor(colour);
	sceneObjects.push_back(pyrimid_right);
}

void table(glm::vec3 pos, glm::vec3 colour, float size, float height)
{

	float scale = size / 2;

	glm::vec3 pointA(pos.x - scale, pos.y, pos.z + scale);
	glm::vec3 pointB(pos.x + scale, pos.y, pos.z + scale);
	glm::vec3 pointC(pos.x + scale, pos.y, pos.z - scale);
	glm::vec3 pointD(pos.x - scale, pos.y, pos.z - scale);
	glm::vec3 heightADJ(0.0, height, 0.0);

	// left front
	Cylinder* cylinder1 = new Cylinder(pointA, 0.25, height);
	cylinder1->setColor(colour);
	sceneObjects.push_back(cylinder1);

	// right front
	Cylinder* cylinder2 = new Cylinder(pointB, 0.25, height);
	cylinder2->setColor(colour);
	sceneObjects.push_back(cylinder2);

	// left back
	Cylinder* cylinder3 = new Cylinder(pointC, 0.25, height);
	cylinder3->setColor(colour);
	sceneObjects.push_back(cylinder3);

	// right back
	Cylinder* cylinder4 = new Cylinder(pointD, 0.25, height);
	cylinder4->setColor(colour);
	sceneObjects.push_back(cylinder4);

	// Table trim front
	Plane* table_trimF = new Plane(pointA + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point A'
		pointB + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point B'					
		pointB + heightADJ,								//Point B
		pointA + heightADJ);							//Point A
	table_trimF->setColor(colour);
	sceneObjects.push_back(table_trimF);

	// Table trim back
	Plane* table_trimB = new Plane(pointB + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point B'
		pointC + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point C'					
		pointC + heightADJ,								//Point C
		pointB + heightADJ);							//Point B
	table_trimB->setColor(colour);
	sceneObjects.push_back(table_trimB);

	// Table trim left
	Plane* table_trimL = new Plane(pointC + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point C'
		pointD + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point D'					
		pointD + heightADJ,								//Point D
		pointC + heightADJ);							//Point C
	table_trimL->setColor(colour);
	sceneObjects.push_back(table_trimL);

	// Table trim right
	Plane* table_trimR = new Plane(pointD + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point D'
		pointA + heightADJ - glm::vec3(0.0, 1.0, 0.0),	//Point A'					
		pointA + heightADJ,								//Point A
		pointD + heightADJ);							//Point D
	table_trimR->setColor(colour);
	sceneObjects.push_back(table_trimR);


	// Table Top
	Plane* table_top = new Plane(pointA + heightADJ - glm::vec3(0.5, 0.0, -0.5),	//Point A
								pointB + heightADJ - glm::vec3(-0.5, 0.0, -0.5),		//Point B
								pointC + heightADJ - glm::vec3(-0.5, 0.0, 0.5),		//Point C
								pointD + heightADJ - glm::vec3(0.5, 0.0, 0.5));	//Point D
	table_top->setTexture(true, 0);
	sceneObjects.push_back(table_top);

	
}

void load_textures()
{
	// Textures
	for (int i = 0; i < numTextures; i++)
	{
		TextureBMP* tmp = new TextureBMP(texture_list[i]);
		textures.push_back(tmp);
	}
}

void create_lights() 
{
	Light* light1 = new Light(glm::vec3(-90, 50, -3));
	lights.push_back(light1);

	Light* spotLight = new Light(glm::vec3(10.0, 10.0, -76.25), glm::vec3(-5.0, -8.0, 0), 15.0);
	lights.push_back(spotLight);
}

//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	// Textures
	load_textures();

	// Lights
	create_lights();

	// Scene objects

	// Floor Plane
	Plane* plane = new Plane(glm::vec3(-128, -15, -40),	//Point A
							glm::vec3(128, -15, -40),	//Point B
							glm::vec3(128, -15, -296),  //Point C
							glm::vec3(-128, -15, -296));//Point D
	plane->setSpecularity(false);
	plane->setReflectivity(true, 0.2);
	sceneObjects.push_back(plane);


	// Table
	table(glm::vec3(0, -15, -70), glm::vec3(0.88, 0.58, 0.3), 25.0, 7.0);

	// Pyrimid
	pyrimid(glm::vec3(-6.25, -8.0, -63.75), glm::vec3(1.0, 1.0, 0.0), 2.0, 2.0);

	// Cone
	Cone* cone1 = new Cone(glm::vec3(6.25, -8.0, -63.75), 1.0, 2.0);
	cone1->setColor(glm::vec3(1.0, 1.0, 0.0));
	sceneObjects.push_back(cone1);

	// Cylinder
	Cylinder* cylinder1 = new Cylinder(glm::vec3(0.0, -8.0, -76.25), 1.0, 2.0);
	cylinder1->setColor(glm::vec3(1.0, 1.0, 0.0));
	sceneObjects.push_back(cylinder1);

	// earth
	Sphere* sphere1 = new Sphere(glm::vec3(0.0, -4.0, -76.25), 2.0);
	sphere1->setTexture(true, 1);
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

	// Refractive
	Sphere* sphere2 = new Sphere(glm::vec3(-6.25, -4.0, -63.75), 2.0);
	sphere2->setColor(glm::vec3(0, 1, 0));   //Set colour to green
	sphere2->setRefractivity(true, 1.0f, 1.01f);
	sphere2->setReflectivity(true, 0.1);	 //Set reflectivity
	sceneObjects.push_back(sphere2);		 //Add sphere to scene objects

	// Transparent
	Sphere* sphere3 = new Sphere(glm::vec3(6.25, -4.0, -63.75), 2.0);
	sphere3->setColor(glm::vec3(0, 1, 0));   //Set colour to green
	sphere3->setTransparency(true, 0.6);
	sphere3->setReflectivity(true, 0.1);	 //Set reflectivity
	sceneObjects.push_back(sphere3);		 //Add sphere to scene objects

	// Cylinder
	Cylinder* cylinder2 = new Cylinder(glm::vec3(-9.5, -20.0, -95), 1.0, 20.0, true);
	cylinder2->setTexture(true, 2);
	sceneObjects.push_back(cylinder2);

	// Cylinder
	Cylinder* cylinder3 = new Cylinder(glm::vec3(9.5, -20.0, -95), 1.0, 20.0, true);
	cylinder3->setTexture(true, 2);
	sceneObjects.push_back(cylinder3);

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Kayle Ransby's Ray-Tracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
