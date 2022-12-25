#include "HierachicalShape.h"
#include "Ray.h"
#include "IntersectionData.h"

float HIERACHY_EPSILON = 1e-4; // To prevent shadow acne

#include <iostream>

HierachicalShape::HierachicalShape() :
    M(glm::mat4(1.0f)), Minv(glm::mat4(1.0f))
{
    transformRay = std::make_shared<Ray>();
    transformData = std::make_shared<IntersectionData>();
}

HierachicalShape::~HierachicalShape()
{
}

void HierachicalShape::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO 7: do something useful here!
}