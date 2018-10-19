/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
*=========================================================================
*/

//========================================================================
//BUILD COMMAND
//g++ -Wall -o "%e" "%f" Sphere.cpp SceneObject.cpp Ray.cpp Plane.cpp TextureBMP.cpp Cylinder.cpp Cone.cpp -lGL -lGLU -lglut//
//========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include "Cylinder.h"
#include "Cone.h"
#include <GL/glut.h>
#include <stdio.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 600;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float PHONG = 8;
const float REFRACTION_INDEX = 1/1.003;
float REFLECTION_POWER = 0.8; //This needs to be lowered if the reflection is in shadow
TextureBMP texture;
float CONTRAST_SCALER = 2.2; //2 is the mathematically 'correct' value, but it does not look good.


vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step, glm::vec3 light)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = light - ray.xpt;
    lightVector = glm::normalize(lightVector);
    float lDotn = glm::dot(lightVector, normalVector);
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    
    glm::vec3 specRef;
    
    //Colors and Texture
    if (ray.xindex == 9) //Floor
    { 
        // -40 -> 40 is planes width (80)
        //-40 -> -180 is planes length (140)
        int lon = ((ray.xpt.x - (-40)) / (40 - (-40))) * 7; // We want 7 divions across
        int lat = ((ray.xpt.z - (-40)) / (-180 - (-40))) * 12.25;  // 140/80 = 1.75, 1.75*7 = 12.25 for squares longways
        if ((lat % 2 == 0 && lon % 2 == 0) || (lat % 2 == 1 && lon % 2 == 1)) {
            materialCol = glm::vec3(0.8, 1, 0.8);
        }
        
    } 
    else if (ray.xindex == 2) //Earth
    { 
        float texcoords = 0.5 + (atan2(normalVector.x, normalVector.z)/(2*M_PI));
        float texcoordt = 0.5 + (asin(normalVector.y)/M_PI);
        materialCol = texture.getColorAt(texcoords, texcoordt);
        specRef = glm::vec3(0);
        
    } 
    else if (ray.xindex == 1)  //Fiery ball
    {
        float lon = ((0.5 + ((atan2(normalVector.z, normalVector.x)/(2*M_PI))*1.0)));
        float lat = ((0.5 - (asin(normalVector.y)/M_PI))*1.0);
        
        if (((lat - (0.06*(tan(20*lon)) + 0.5)) < 0))  //Fire 
        {
            float redCol = 0.6;
            materialCol = glm::vec3(redCol, 0, 0);
        }
        
        //materialCol = glm::vec3((lon+lat)/2, 0, 1);   //(1 - fabs(1/normalVector.x), 1 - fabs(1/normalVector.y), 1 - fabs(1/normalVector.z)); Rainbow
        
        //~ if ((lat % 2 == 0) || abs(lat - lon) % 4 == 0) { //Bricks
            //~ materialCol = glm::vec3(0.6, 0.0, 0.0);
        //~ }
        
        //~ if ((lat % 2 == 0 && lon % 2 == 0) || (lat % 2 == 1 && lon % 2 == 1)) { //Checkered
            //~ materialCol = glm::vec3(0.6, 0.0, 0.0);
        //~ }
        
        //Specular
        glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
        float rDotl = glm::dot(reflVector, lightVector);
        if(rDotl < 0) 
        {
            specRef = glm::vec3(0, 0, 0);
        } 
        else 
        {
            float specular = pow(rDotl, PHONG);
            specRef = specular*glm::vec3(1, 1, 1);
        }
    }
    else if (ray.xindex == 12)  //Spotted cone
    {
        float lon = ((0.5 + ((atan2(normalVector.z, normalVector.x)/(2*M_PI))*18.0)));
        float lat = fabs(ray.xpt.y); //Cannot use the nomral.y, because it is constant for the cone
        int lonInt = trunc(lon); //Remove the decimal points
        int latInt = trunc(lat);
        
        float s = (lon - lonInt) - 0.5; //Center of the squares (trunc-float gives us the decimal points, -0.5 for corner offset)
        float t = (lat - latInt) - 0.5;
        
        if (s*s + t*t <= 0.25 /*if point in cirlce*/ && ((lonInt % 2 == 0 && latInt % 2 == 0) || (lonInt % 2 == 1 && latInt % 2 == 1))) 
        {
            materialCol = glm::vec3(0.6, 0.6, 0.0);
        }
        
        //Specular
        glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
        float rDotl = glm::dot(reflVector, lightVector);
        if(rDotl < 0) 
        {
            specRef = glm::vec3(0, 0, 0);
        } 
        else 
        {
            float specular = pow(rDotl, PHONG);
            specRef = specular*glm::vec3(1, 1, 1);
        }
    } 
    else //Specular
    {
        glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
        float rDotl = glm::dot(reflVector, lightVector);
        
        if(rDotl < 0) 
        {
            specRef = glm::vec3(0, 0, 0);
        } 
        else 
        {
            float specular = pow(rDotl, PHONG);
            specRef = specular*glm::vec3(1, 1, 1);
        }
    }
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);
    float lightDist = sqrt(pow((light.x - ray.xpt.x), 2) + pow((light.y - ray.xpt.y), 2));

    glm::vec3 colorSum;
    
    
    //Transparent
    if(ray.xindex == 10 && step < 3) 
    {
        colorSum = ambientCol*materialCol + specRef;
        glm::vec3 g = glm::refract(ray.dir, normalVector, REFRACTION_INDEX);
        Ray refrRay1(ray.xpt, g);
        refrRay1.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
        glm::vec3 h = glm::refract(g, -m, 1.0f/REFRACTION_INDEX);
        Ray refrRay2(refrRay1.xpt, h);
        glm::vec3 refractedCol = trace(refrRay2, step+1, light);
        colorSum = colorSum + (refractedCol);
    
    } 
    else //Opaque
    { 
        
        if(shadow.xindex==10 && shadow.xdist < lightDist) //In shadow of transparent object
        { 
            colorSum = glm::vec3(0.7, 0.7, 0.5)*materialCol;            
        
        } 
        else if(lDotn < 0 || (shadow.xindex>-1 && shadow.xdist < lightDist)) //In shadow
        {
            colorSum = ambientCol*materialCol;
            REFLECTION_POWER = 0.3;
        
        }
        else if(ray.xindex == 11) //Fully reflective
        {
            REFLECTION_POWER = 1;
        
        } 
        else 
        {
            colorSum = (ambientCol*materialCol + lDotn*materialCol) + specRef;
            REFLECTION_POWER = 0.8;
        }
        
        //Reflection
        if((ray.xindex == 0 && step < MAX_STEPS)) //Blue sphere
        {
            glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
            Ray reflectedRay(ray.xpt, reflectedDir);
            glm::vec3 reflectedCol = trace(reflectedRay, step+1, light);
            colorSum = colorSum + (REFLECTION_POWER*reflectedCol);
        }
        if((ray.xindex == 11 && step < MAX_STEPS)) //Cylinder
        {
            glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector); 
            Ray reflectedRay(ray.xpt, reflectedDir);                                 
            glm::vec3 reflectedCol = trace(reflectedRay, step+1, light);
            colorSum = colorSum + (REFLECTION_POWER*reflectedCol);
        }
    }
    return (colorSum);
}

//---Traces a ray and returns a colour---------------------------------------------------------------
//  Traces through the specified pixel at dx (x-shift) and dy (y-shift) in the pixel and returns the colour found
//--------------------------------------------------------------------------------------------
glm::vec3 getDivisionColour(glm::vec3 eye, float xp, float yp, float cellX, float cellY, float dx, float dy)
{
    glm::vec3 dir(xp+dx*cellX, yp+dy*cellY, -EDIST);
    Ray ray = Ray(eye, dir);
    ray.normalize();	
    glm::vec3 light1(10, 40, -3);
    glm::vec3 light2(-60, 20, -30);
    glm::vec3 col1 = trace (ray, 1, light1);
    glm::vec3 col2 = trace (ray, 1, light2);
    glm::vec3 col = sqrt(((col1*col1) + (col2*col2))); //*((col1 - col2)*(col2 - col1));//Spooky bois
    return col;
}

//---Color averager of a pixel----------------------------------------------------------------
// Get the average color of a pixel at four points specified by two x and two y values
// This calls getDivisionColour for each point and returns an average color of the 4 of them
//--------------------------------------------------------------------------------------------
glm::vec3 getAverageColour(glm::vec3 eye, float xp, float yp, float cellX, float cellY, float shiftx1, float shiftx2, float shifty1, float shifty2)
{
    glm::vec3 botL = getDivisionColour(eye, xp, yp, cellX, cellY, shiftx1, shifty1);
    glm::vec3 botR = getDivisionColour(eye, xp, yp, cellX, cellY, shiftx2, shifty1);
    glm::vec3 topL = getDivisionColour(eye, xp, yp, cellX, cellY, shiftx2, shifty2);
    glm::vec3 topR = getDivisionColour(eye, xp, yp, cellX, cellY, shiftx1, shifty2);
    
    float tempAvgR = sqrt(((botL.r*botL.r) + (botR.r*botR.r) + (topR.r*topR.r) + (topL.r*topL.r)))/CONTRAST_SCALER;
    float tempAvgG = sqrt(((botL.g*botL.g) + (botR.g*botR.g) + (topR.g*topR.g) + (topL.g*topL.g)))/CONTRAST_SCALER;
    float tempAvgB = sqrt(((botL.b*botL.b) + (botR.b*botR.b) + (topR.b*topR.b) + (topL.b*topL.b)))/CONTRAST_SCALER;

    glm::vec3 avgCol = glm::vec3(tempAvgR, tempAvgG, tempAvgB);
    
    return avgCol;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height
    float diff = 0.04; //Colour difference must be bigger than this for pixel to be further divided (10.2 in RGB)

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
	    xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;
            
            //Get the first four colors of the pixel division (into 4)
            glm::vec3 botL = getDivisionColour(eye, xp, yp, cellX, cellY, 0.25, 0.25);
            glm::vec3 botR = getDivisionColour(eye, xp, yp, cellX, cellY, 0.75, 0.25);
            glm::vec3 topR = getDivisionColour(eye, xp, yp, cellX, cellY, 0.75, 0.75);
            glm::vec3 topL = getDivisionColour(eye, xp, yp, cellX, cellY, 0.25, 0.75);
            
            //ADAPTIVE SAMPLING STARTS
            //Average the red, green and blue values
            float tempAvgR = sqrt(((botL.r*botL.r) + (botR.r*botR.r) + (topR.r*topR.r) + (topL.r*topL.r)))/CONTRAST_SCALER;
            float tempAvgG = sqrt(((botL.g*botL.g) + (botR.g*botR.g) + (topR.g*topR.g) + (topL.g*topL.g)))/CONTRAST_SCALER;
            float tempAvgB = sqrt(((botL.b*botL.b) + (botR.b*botR.b) + (topR.b*topR.b) + (topL.b*topL.b)))/CONTRAST_SCALER;
            
            //Check each cells colors agains the average of all cells, and if it is too far out, divide that cell further
            if (abs(botL.r - tempAvgR) > diff || abs(botL.g - tempAvgG) > diff || abs(botL.b - tempAvgB) > diff) {
                botL = getAverageColour(eye, xp, yp, cellX, cellY, 0.125, 0.375, 0.125, 0.375);
            }
            
            if (abs(botR.r - tempAvgR) > diff || abs(botR.g - tempAvgG) > diff || abs(botR.b - tempAvgB) > diff) {
                botR = getAverageColour(eye, xp, yp, cellX, cellY, 0.625, 0.875, 0.125, 0.375);
                
            }
            
            if (abs(topR.r - tempAvgR) > diff || abs(topR.g - tempAvgG) > diff || abs(topR.b - tempAvgB) > diff) {
                topR = getAverageColour(eye, xp, yp, cellX, cellY, 0.625, 0.875, 0.625, 0.875);            
            }
            
            if (abs(topL.r - tempAvgR) > diff || abs(topL.g - tempAvgG) > diff || abs(topL.b - tempAvgB) > diff) {
                topL = getAverageColour(eye, xp, yp, cellX, cellY, 0.125, 0.375, 0.625, 0.875);               
            }
            //ADAPTIVE SAMPLING ENDS
            
            float avgR = (sqrt(((botL.r*botL.r) + (botR.r*botR.r) + (topR.r*topR.r) + (topL.r*topL.r)))/CONTRAST_SCALER);
            float avgG = (sqrt(((botL.g*botL.g) + (botR.g*botR.g) + (topR.g*topR.g) + (topL.g*topL.g)))/CONTRAST_SCALER);
            float avgB = (sqrt(((botL.b*botL.b) + (botR.b*botR.b) + (topR.b*topR.b) + (topL.b*topL.b)))/CONTRAST_SCALER);
            
            
            //glColor3f(botL.r ,botL.g, botL.b);

			glColor3f(avgR, avgG, avgB);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
    
        }
    }
    glEnd();
    glFlush();
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

	//-- Create a pointer to a sphere object
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 0, -120.0), 15.0, glm::vec3(0, 0, 1));
	Sphere *sphere2 = new Sphere(glm::vec3(5.0, 4.0, -100.0), 3.0, glm::vec3(1, 0.5, 0));
	Sphere *sphere3 = new Sphere(glm::vec3(4.0, -16.0, -99.0), 4.0, glm::vec3(0, 1, 0));

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1); //0
	sceneObjects.push_back(sphere2); //1
	sceneObjects.push_back(sphere3); //2
    
    //-- Floor
    Plane *floorPlane = new Plane (glm::vec3(-40., -20, -40),
                                   glm::vec3(40., -20, -40),
                                   glm::vec3(40., -20, -180),
                                   glm::vec3(-40, -20, -180),
                                   glm::vec3(1, 1, 1));
       
    //    5---6   
    //   /   /|   
    //  2---1 |   
    //  |   | 7   
    //  |   |/   
    //  3---4     
       
                                
    //-- Cube vertices
                                               
                                               
    glm::vec3 corner1(-4,     -16, -90);           
    glm::vec3 corner2(-7.464, -16, -92);       
    glm::vec3 corner3(-7.464, -20, -92);       
    glm::vec3 corner4(-4,     -20, -90);           
    
    glm::vec3 corner5(-5.464, -16, -95.464);
    glm::vec3 corner6(-2,     -16, -93.464);
    glm::vec3 corner7(-2,     -20, -93.464);
    glm::vec3 corner8(-5.464, -20, -95.464);
    glm::vec3 color1(1, 0, 1);
    
    Plane *face1234 = new Plane (corner1, corner2, corner3, corner4, color1);
    Plane *face6147 = new Plane (corner6, corner1, corner4, corner7, color1);
    Plane *face5678 = new Plane (corner5, corner6, corner7, corner8, color1);
    Plane *face2583 = new Plane (corner2, corner5, corner8, corner3, color1);
    Plane *face6521 = new Plane (corner6, corner5, corner2, corner1, color1);
    Plane *face4387 = new Plane (corner4, corner3, corner8, corner7, color1);
    
    sceneObjects.push_back(face1234); //3
    sceneObjects.push_back(face6147); //4
    sceneObjects.push_back(face5678); //5
    sceneObjects.push_back(face2583); //6
    sceneObjects.push_back(face6521); //7
    sceneObjects.push_back(face4387); //8
    
    sceneObjects.push_back(floorPlane); //9
    
    Sphere *sphere4 = new Sphere(glm::vec3(0, -11.5, -85.0), 2.0, glm::vec3(1, 1, 0.0));
    sceneObjects.push_back(sphere4); //10
    
    Cylinder *cylinder1 = new Cylinder (glm::vec3(-15, -20, -100), 5, 15.0, glm::vec3(1, 1, 1));
    sceneObjects.push_back(cylinder1); //11
    
    Cone *cone1 = new Cone (glm::vec3(17, -20, -117), 3.0, 9.0, glm::vec3(0, 0.6, 0.6));
    sceneObjects.push_back(cone1); //12

    char pathBuffer[50] = "Earth.bmp";
    char* path = pathBuffer;
    texture = TextureBMP(path);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
