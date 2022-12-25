#pragma once
#ifndef AXIS_H
#define AXIS_H

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
 * This class handles axis
 * By Alexandre Mercier-Aubin
 */
class Axis
{
public:
	Axis(const GLuint aPosLocation);
	virtual ~Axis();
	void draw(const shared_ptr<Program> program, shared_ptr <MatrixStack> M);
private:

	const GLfloat vertices[6][3] = {
					{  0,  0,  0 },
					{  1,  0,  0 },
					{  0,  0,  0 },
					{  0,  1,  0 },
					{  0,  0,  0 },
					{  0,  0,  1 } };

	GLuint posBufID;
	GLuint frameVAO;
};

#endif
