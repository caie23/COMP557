#pragma once
#ifndef WIRECUBE_H
#define WIRECUBE_H

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
 * This class handles wire cubes.
 * By Alexandre Mercier-Aubin
 */
class WireCube
{
public:
	WireCube(const GLuint aPosLocation);
	virtual ~WireCube();
	void draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV);
	GLuint textureID;
private:


	const GLfloat vertices[72] = { -1, -1,  1, 
									 1, -1,  1,
									 1, -1,  1,
									 1,  1,  1,
									 1,  1,  1,
									-1,  1,  1,
									-1,  1,  1,
									-1, -1,  1,
									-1, -1, -1,
									 1, -1, -1,
									 1, -1, -1,
									 1,  1, -1,
									 1,  1, -1,
									-1,  1, -1,
									-1,  1, -1,
									-1, -1, -1,
									-1, -1,  1,
									-1, -1, -1,
									 1, -1,  1,
									 1, -1, -1,
									 1,  1,  1,
									 1,  1, -1,
									-1,  1,  1,
									-1,  1, -1
					};

	GLuint posBufWireID;
	GLuint wireVAO;
};

#endif
