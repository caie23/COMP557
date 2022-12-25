#include "Plane.h"
#include "Ray.h"
#include "IntersectionData.h"

float PLANE_EPSILON = 1e-4; // To prevent shadow acne

Plane::Plane() :
    normal(glm::vec3(0.0f, 1.0f, 0.0f)),
    position(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Plane::Plane(glm::vec3 _normal, glm::vec3 _position) :
    normal(_normal),
    position(_position)
{
}

Plane::Plane(glm::vec3 _normal) :
    normal(_normal), position(0.0f, 0.0f, 0.0f)
{
}

Plane::~Plane()
{
}

void Plane::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 4: intersection of ray with plane
    float t0 = dot((position - ray->origin), normal) / dot(ray->direction, normal);
    if (t0 < intersection->t){
        intersection->t = t0;
        ray->computePoint(intersection->t, intersection->p);
        intersection->n = normal;
        intersection->material = materials[1];
    }
}
