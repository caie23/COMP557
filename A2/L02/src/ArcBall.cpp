// name and student number: Claire Feng (260899684)
#include "ArcBall.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;


ArcBall::ArcBall():R(glm::mat4(1.0)), Rmem(glm::mat4(1.0)), p0(glm::vec3(1.0)), p1(glm::vec3(1.0)), fit(0.5), gain(5) // fit(0.5), gain(5)
{
}

ArcBall::~ArcBall()
{
}

glm::vec3 ArcBall::computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeight) {
	////TODO: compute the projection of mouse coords on the arcball
//    return glm::vec3(0,0,0);
    // center of the track ball is at the center of the screen
    double centerx = windowWidth / 2;
    double centery = windowHeight / 2;
    double radius = min(windowHeight, windowWidth) / fit;
    double x = (mousex - centerx) / radius;
    double y = (mousey - centery) / radius;
    double z;
    double r = x*x + y*y;
    if (r > 1) {
        double s = 1 / sqrt(r);
        x *= s;
        y *= s;
        z = 0;
    } else {
        z = sqrt(1-r);
    }
    return glm::vec3(x,-y,z);
}

double computeVectorAngle(glm::vec3& v1, glm::vec3& v2) {
	double vDot = glm::dot(v1, v2);
	if (vDot < -1.0) vDot = -1.0;
	if (vDot > 1.0) vDot = 1.0;
	return((double)(acos(vDot)));
}

void ArcBall::startRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
	Rmem = R;
	p0 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);
}

void ArcBall::updateRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
	////TODO: compute the rotation update for the view camera
    R = Rmem;
    p1 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);
    glm::vec3 axis = glm::cross(p0, p1);
    float angle = computeVectorAngle(p0, p1);
    angle *= gain; // scale the angle
    R = glm::rotate(glm::mat4(1.0), angle, axis)*R;
}
