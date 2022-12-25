#pragma once
#ifndef QUADTEXTURED_H
#define QUADTEXTURED_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
/**
 * This class creates and draws a Quad object with texture mapping enabled.
 */
class QuadTextured
{
public:
	QuadTextured(const GLuint aPosLocation, const GLuint aNorLocation, const GLuint aTexLocation);
	virtual ~QuadTextured();

	//draws the quad
	void draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV);

	//texture to draw
	GLuint *textureID;
private:

	const GLfloat vertices[12] = { -1,-1, 0, 1,-1, 0,-1, 1, 0, 1, 1, 0 };
	const GLubyte indices[4] = { 0,1,2,3 };
	const GLfloat normals[12] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
	const GLfloat textureCoords[8] = { 0,0,1,0,0,1,1,1 };
	
	GLuint quadIBO;
	GLuint texBufQuadID;
	GLuint posBufQuadID;
	GLuint norBufQuadID;
	GLuint quadVAO;
};

#endif
