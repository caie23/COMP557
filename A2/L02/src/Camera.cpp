// name and student number: Claire Feng (260899684)
#include "Camera.h"

using namespace std;


Camera::Camera(float aspect):lightColor(glm::vec3(1,1,1))
{
	this->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);
	this->updateView();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
	//// TODO: Replace the default projection matrix with one constructed from the parameters available in this class!
//	P = glm::mat4(1.0);
    P = glm::perspective(fov, aspect, near, far); // fov is in radians
}

void Camera::updateView() {
	//// TODO: Replace the default viewing matrix with one constructed from the parameters available in this class!
//	V = glm::mat4(1.0);
    V = glm::lookAt(position, lookAt, up);
}


void Camera::draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, Axis &axis) {
	program->bind();
	M->pushMatrix();
	
	//set uniform variables
	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("lightPV"), 1, GL_FALSE, &LightPV[0][0]);
	
	//// TODO: draw the light frame using a fancy axis... You must set up the right transformation!
    M->multMatrix(glm::inverse(this->V));
    
	axis.draw(program, M);

	//draw the camera's wirecube
	M->pushMatrix();

	//// TODO: draw the light camera frustum using the inverse projection with a wire cube. You must set up the right transformation!
    M->multMatrix(glm::inverse(this->P));
    
	debugWireCube->draw(program, P, V, M, LightPV);

	program->unbind();

	M->pushMatrix();	
	//// TODO: draw the light view on the near plane of the frustum. You must set up the right transformation! 
	// That is, translate and scale the x and y directions of the -1 to 1 quad so that the quad fits exactly the l r t b portion of the near plane
    M->translate(glm::vec3(0, 0, -1));
    
	debugDepthMapQuad->draw(quadShader, P, V, M, LightPV);

	M->popMatrix();
	M->popMatrix();
	M->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}
