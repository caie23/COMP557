// name and student number: Claire Feng (260899684)
#include "Scene.h"

using namespace std;

Scene::Scene(string RES_DIR, float* aspect, GLuint aPosLocation, GLuint aNorLocation, GLuint aTexLocation): 
	aPosLocation(aPosLocation), 
	aNorLocation(aNorLocation), 
	aTexLocation(aTexLocation), 
	ks(glm::vec3(0.5,0.5,0.5)), 
	sigma(0.001),
	shininess(64),
	axis(aPosLocation)
{
	// Initialize mesh.
	teapot = make_shared<Shape>();
	teapot->loadMesh(RES_DIR + "teapot.obj");
	teapot->init();

	// Initialize floor
	cube = make_shared<Shape>();
	cube->loadMesh(RES_DIR + "cube.obj");
	cube->init();

	// Initialize sphere
	sphere = make_shared<Shape>();
	sphere->loadMesh(RES_DIR + "Sphere.obj");
	sphere->init();

	// Initialize camera	
	cam = make_shared<Camera>(*aspect);

	// Initialize the light's camera to create shadows
	lightCam = make_shared<Camera>(1);
	lightCam->position = glm::vec3(3, 2, 3);
	lightCam->setPerspective(glm::radians(55.0), 1, 1, 40);
	lightCam->updateView();

	// Initialize a textured quad
	QuadTextured depthQuad(aPosLocation, aNorLocation, aTexLocation);
	lightCam->debugDepthMapQuad = make_shared<QuadTextured>(depthQuad);

	// Initialize the debug wirecube
	WireCube debugWireCube(aPosLocation);
	lightCam->debugWireCube = make_shared<WireCube>(debugWireCube);
}

Scene::~Scene()
{
}

void Scene::renderScene(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, bool isShadowPipeline, double time) {
	//draw teapot
	renderTeapot(program, P, V, M, LightPV, time);

	//draw floor
	renderFloor(program, P, V, M, LightPV);

	renderTentacles(program, P, V, M, LightPV, time);

	if(!isShadowPipeline){
		//draw camera
		lightCam->draw(program, P, V, M, LightPV, axis);
	}
}

void Scene::renderTentacles(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, double time) {
	program->bind();
	M->pushMatrix();
	M->rotate(-M_PI / 2, 1, 0, 0);
	glUniform3f(program->getUniform("col"), 1, 1, 0);
	srand(0);

	for (int ii = 0; ii < 5; ii++) {
		M->pushMatrix();
		double x = (ii / 3 - 1.0);
		double y = (ii % 3 - 1.0);
		M->translate(x, y, 0);
		M->scale(0.1, 0.1, 0.1);
		for (int i = 0; i < 10; i++) {
			M->translate(0, 0, 1.2);
			double theta = 0.1 * cos(time + M_PI * i / 4.0 + 6 * rand());
			M->rotate(theta, 1, 0, 0);
			double alpha = 0.1 * sin(time *1.3 + M_PI * i / 4.0 + 6 * rand());
			M->rotate(alpha, 0, 1, 0);
			setUniforms(program, P, V, M, LightPV);
			cube->draw(program);
		}
		M->translate(0, 0, 1.5);
		setUniforms(program, P, V, M, LightPV);
		sphere->draw(program);
		M->popMatrix();
	}

	M->popMatrix();
	program->unbind();
}

void Scene::renderTeapot(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, double time) {
	// Draw teapot.
	program->bind();

	M->pushMatrix();
	M->translate(0.0f, -0.4f, 0.0f);

	double c = cos(time);
	M->translate((float)-c, 0, 0);
	M->rotate((float)c, 0, 0, 1);
	M->translate(0, 1, 0);
	
	setUniforms(program, P, V, M, LightPV);
	glUniform3f(program->getUniform("col"), 1, 0, 0);
	
	teapot->draw(program);
	M->popMatrix();
	program->unbind();
}

void Scene::renderFloor(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV) {
	// Draw floor.
	program->bind();

	M->pushMatrix();

	M->scale(glm::vec3(1.5, 0.06, 1.5));
	
	setUniforms(program, P, V, M, LightPV);
	glUniform3f(program->getUniform("col"), 0, 1, 0);

	cube->draw(program);
	M->popMatrix();
	program->unbind();
}

void Scene::setUniforms(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV) {
	//TODO: compute light position of lightCam
    
    
	glm::vec4 p(1);

	//storing uniform variables
	glUniform3fv(program->getUniform("lightPos"), 1, &p[0]);
	glUniform3fv(program->getUniform("ks"), 1, &ks[0]);
	glUniform3fv(program->getUniform("lightColor"), 1, &lightCam->lightColor[0]);
	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("LightPV"), 1, GL_FALSE, &LightPV[0][0]);
	glUniform1i(program->getUniform("enableLighting"), true);
	glUniform1i(program->getUniform("depthMap"), 1);
	glUniform1f(program->getUniform("sigma"), sigma);
	glUniform1f(program->getUniform("shininess"), shininess);
}
