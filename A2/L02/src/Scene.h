#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Camera.h"
#include "Program.h"
#include "QuadTextured.h"
#include "WireCube.h"
#include "Axis.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * This class handles the scene. This is where most rendering calls will be made and where the objects are positioned.
 * By Alexandre Mercier-Aubin
 */
using namespace std;
class Scene
{
public:
	Scene(string RES_DIR, float* aspect, GLuint aPosLocation, GLuint aNorLocation, GLuint aTexLocation );
	virtual ~Scene();

	//renders the scene
	void renderScene(const std::shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV, bool isShadowPipeline, double time);

	//aspect ratio
	float* aspect;
	
	//the scene's camera
	shared_ptr<Camera> cam;

	//camera used by the light to draw shadows
	shared_ptr<Camera> lightCam;
	
	//VAO to draw the frames
	GLuint* axisVAO;
	GLuint* depthMap;
	float sigma;
private:
	//pointers to the scene objects
	shared_ptr<Shape> teapot;
	shared_ptr<Shape> cube;
	shared_ptr<Shape> sphere;
	Axis axis;

	//draw calls for the scene objects
	void renderTeapot(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV, double time);
	void renderFloor(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV);
	void renderTentacles(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, double time);

	//function that sets the uniform variables of the current shader. This is mostly here to reduce code redundancy.
	void setUniforms(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV);

	//location of the shader inputs for the vao. This determines where the positions, normals and texture coordinate are stored.
	GLuint aPosLocation;
	GLuint aNorLocation;
	GLuint aTexLocation;

	glm::vec3 ks;
	float shininess;
};

#endif
