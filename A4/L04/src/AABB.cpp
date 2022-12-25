#include "AABB.h"
#include "Ray.h"
#include "IntersectionData.h"

static float AABB_EPSILON = 1e-4; // To prevent shadow acne
float t_min = FLT_MIN;
float t_max = FLT_MAX;

AABB::AABB() :
    minpt(0.0f, 0.0f, 0.0f),
    maxpt(10.0f, 10.0f, 10.0f)
{
}

AABB::AABB(glm::vec3 size, glm::vec3 position)
{
    // Compute the min and max points from the size and com
    minpt = position - size / 2.0f;
    maxpt = position + size / 2.0f;
}

AABB::~AABB()
{
}

void AABB::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 6: intersection of Ray with axis aligned box
}