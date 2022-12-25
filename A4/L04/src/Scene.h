#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <memory>

#include "Image.h"

class Shape;
class Camera;
class Light;
class Material;

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

/**
This class handles the scene.
This is where iterating over all the pixels happen (aka the render loop).
- By Lo�c Nassif
*/
class Scene
{
public:
	Scene(const std::string SCENE_PATH);
	virtual ~Scene();

	// Init scene parameters
	void init();

	//renders the scene
	void render();

	// Add object
	void addObject(shared_ptr<Shape> shape);

	// Add light
	void addLight(shared_ptr<Light> light);

	// Width and height of image
	int width, height;

	//aspect ratio
	float aspect;

	//the scene's camera
	shared_ptr<Camera> cam;

	// Ambient lighting of the scene
	glm::vec3 ambient;

	// jitter samples (to help with antialiasing)
	bool jitter = false;

	// Sample count for AA
	int samples = 1;

	// The scene's objects
	std::vector<shared_ptr<Shape>> shapes;

private:
	float shininess;

	// The scene's lights
	std::vector<shared_ptr<Light>> lights;

	// The scene's image
	shared_ptr<Image> image;

	// Scene output filename (should end in .png)
	std::string outputFilename;
};

#endif
