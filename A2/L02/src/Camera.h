#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "QuadTextured.h"
#include "WireCube.h"
#include "Axis.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
/**
 * By Alexandre Mercier-Aubin
 * This sets camera for instance the one used to render or the one to create the shadow maps
 */
class Camera
{
public:
	Camera(float aspect);
	virtual ~Camera();

	//updates the view matrix according
	void updateView();
	void draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV, Axis &axis);

	//sets the perspective of the camera
	void setPerspective(float fov, float aspect, float near, float far);

	//position of the camera in the world
	glm::vec3 position = glm::vec3(0, 1, 8);

	//where the camera is pointing at
	glm::vec3 lookAt = glm::vec3(0, 0, 0);

	//a vector that stores in which direction the sky is.
	glm::vec3 up = glm::vec3(0, 1, 0);

	//The camera's view matrix
	glm::mat4 V;

	//The camera's perspective matrix
	glm::mat4 P;

	//If the camera is drawn, then the depth buffer can be plotted on this quad
	shared_ptr<QuadTextured> debugDepthMapQuad;

	//cube made out of lines that can be used to plot the projection of the camera.
	shared_ptr<WireCube> debugWireCube;

	//Shader used to render the debug quad
	shared_ptr<Program> quadShader;

	glm::vec3 lightColor;

};

#endif
