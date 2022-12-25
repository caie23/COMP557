// name and student number: Claire Feng (260899684)
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "ArcBall.h"
#include "Camera.h"
#include "Scene.h"

using namespace std;

GLFWwindow *window; // Main application window
string RES_DIR = ""; // Where data files live

shared_ptr<Program> perFragmentLightingProgram; // to draw with shadows
shared_ptr<Program> drawLightDepthProgram; // to draw depth from the ligth's view 
shared_ptr<Program> depthDrawDebugProgram; // program that takes as input a depthmap and renders it.
shared_ptr<Scene> scene;

GLuint frameVAO;	
GLuint posBufID; // position buffer for drawing a line loop
GLuint depthMap;
GLuint depthMapFBO;

GLuint aPosLocation = 0; // location set in col_vert.glsl (or can be queried)
GLuint aNorLocation = 1;
GLuint aTexLocation = 2;


const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
float aspect;
ArcBall arcBall;

int windowWidth, windowHeight; // viewing window width and height

int buttonState = 0;

static void error_callback(int error, const char *description) {
	cerr << description << endl;
}

float dthetaz = 0;
float dthetax = 0;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	float dtheta = 0.01; // theta increment for rotating light
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	} else if (key == GLFW_KEY_LEFT) {
		dthetaz = action == GLFW_RELEASE ? 0 : dtheta;
	} else if (key == GLFW_KEY_RIGHT) {
		dthetaz = action == GLFW_RELEASE ? 0 : -dtheta;
	} else if (key == GLFW_KEY_UP) {
		dthetax = action == GLFW_RELEASE ? 0 : dtheta;
	} else if (key == GLFW_KEY_DOWN) {
		dthetax = action == GLFW_RELEASE ? 0 : -dtheta;
	} else if (key == GLFW_KEY_K) {
		scene->sigma = scene->sigma * 1.1;
	} else if (key == GLFW_KEY_L) {
		scene->sigma = scene->sigma / 1.1;
	}
}

static void mouseDown_callback(GLFWwindow* window, int button, int action, int mods) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {	
		buttonState = 0;
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		double x;
		double y;
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glfwGetCursorPos(window, &x, &y);
		arcBall.startRotation(x, y, windowWidth, windowHeight);
		buttonState = 1;
	}
}

static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos) {
	if (buttonState == 1) {
		double x;
		double y;
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glfwGetCursorPos(window, &x, &y);
		arcBall.updateRotation(x, y, windowWidth, windowHeight);
	}
}

static void checkFramebufferStatus() {
	GLint framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (framebufferStatus) {
	case GL_FRAMEBUFFER_COMPLETE:
		cout << "GL_FRAMEBUFFER_COMPLETE" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		cout << "GL_FRAMEBUFFER_UNSUPPORTED" << endl;
		break;
	}
}

static void init() {
	GLSL::checkVersion();

	// Check how many texture units are supported in the vertex shader
	int tmp;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
	cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
	// Check how many uniforms are supported in the vertex shader
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
	cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

	// Set background color.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	// Initialize scene
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	aspect = windowWidth / (float)windowHeight;
	Scene newScene(RES_DIR, &aspect, aPosLocation, aNorLocation, aTexLocation);
	scene = make_shared<Scene>(newScene);

	perFragmentLightingProgram = make_shared<Program>();
	perFragmentLightingProgram->setVerbose(true);
	perFragmentLightingProgram->setShaderNames(RES_DIR + "perFragmentLighting_vert.glsl", RES_DIR + "perFragmentLighting_frag.glsl");
	perFragmentLightingProgram->init();
	perFragmentLightingProgram->addUniform("P");
	perFragmentLightingProgram->addUniform("V");
	perFragmentLightingProgram->addUniform("M");
	perFragmentLightingProgram->addUniform("MinvT");
	perFragmentLightingProgram->addUniform("LightPV");
	perFragmentLightingProgram->addUniform("depthMap");
	perFragmentLightingProgram->addUniform("lightPos");
	perFragmentLightingProgram->addUniform("enableLighting");
	perFragmentLightingProgram->addUniform("col");
	perFragmentLightingProgram->addUniform("ks");
	perFragmentLightingProgram->addUniform("sigma");
	perFragmentLightingProgram->addUniform("shininess");
	perFragmentLightingProgram->addUniform("lightColor");
	perFragmentLightingProgram->addAttribute("aPos");
	perFragmentLightingProgram->addAttribute("aNor");
	perFragmentLightingProgram->setVerbose(false);
	
	drawLightDepthProgram = make_shared<Program>();
	drawLightDepthProgram->setVerbose(true);
	drawLightDepthProgram->setShaderNames(RES_DIR + "drawLightDepth_vert.glsl", RES_DIR + "drawLightDepth_frag.glsl");
	drawLightDepthProgram->init();
	drawLightDepthProgram->addUniform("P");
	drawLightDepthProgram->addUniform("V");
	drawLightDepthProgram->addUniform("M");
	drawLightDepthProgram->addAttribute("aPos");
	drawLightDepthProgram->setVerbose(false);

	depthDrawDebugProgram = make_shared<Program>();
	depthDrawDebugProgram->setVerbose(true);
	depthDrawDebugProgram->setShaderNames(RES_DIR + "depthDrawDebug_vert.glsl", RES_DIR + "depthDrawDebug_frag.glsl");
	depthDrawDebugProgram->init();
	depthDrawDebugProgram->addUniform("P");
	depthDrawDebugProgram->addUniform("V");
	depthDrawDebugProgram->addUniform("M");
	depthDrawDebugProgram->addUniform("depthMap");
	depthDrawDebugProgram->addAttribute("aPos");
	depthDrawDebugProgram->addAttribute("aTex");
	depthDrawDebugProgram->setVerbose(false);

	scene->lightCam->quadShader = depthDrawDebugProgram;

	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);


	glGenTextures(1, &depthMap);
	glActiveTexture(GL_TEXTURE1); // use texture unit 1
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// float or unsigned int for depth??? TODO... was this wrong in previous sample solution?
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// might not be the best way to handle lookups outside of the depth texture
	float c[4] = { 1,1,1,1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);


	//create offscreen framebuffer for drawing to depth map texture 
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	checkFramebufferStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// this is awkward
	scene->lightCam->debugDepthMapQuad->textureID = &depthMap;
	scene->depthMap = &depthMap;
}

static void render()
{
	// Create model matrix stack
	auto M = make_shared<MatrixStack>();

	// update camera aspect ratio with current frame buffer size 
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	aspect = (float)windowWidth / (float)windowHeight;
	scene->cam->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);

	// update the light camera (position changes with keyboard controls)
	glm::mat4 R(1); // Creates a identity matrix
	R = glm::rotate(R, dthetaz, glm::vec3(0.f, 0.f, 1.f));
	R = glm::rotate(R, dthetax, glm::vec3(1.f, 0.f, 0.f));
	scene->lightCam->position = glm::vec3(R * glm::vec4(scene->lightCam->position, 1));

	scene->lightCam->updateView();

	// get light and eye projection matricies
	glm::mat4 PL = scene->lightCam->P;
	glm::mat4 PE = scene->cam->P;

	// get light and eye viewing transformations
	glm::mat4 VL = scene->lightCam->V;
	glm::mat4 VE = scene->cam->V * arcBall.R;

	glm::mat4 LightPV = PL * VL;

	double time = glfwGetTime();

	/// Render pass for the shadow map
	// make the FBO active
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT);
	scene->renderScene(drawLightDepthProgram, PL, VL, M, LightPV, true, time);

	/// Render pass for the main window view
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->renderScene(perFragmentLightingProgram, PE, VE, M, LightPV, false, time);

	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RES_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
    
	// https://en.wikipedia.org/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Claire Feng (260899684)", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseDown_callback);
	glfwSetCursorPosCallback(window, mouseMove_callback);
	
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
