#include "Sphere.h"
#include "Ray.h"
#include "IntersectionData.h"

float SPHERE_EPSILON = 1e-4; // To prevent shadow acne

Sphere::Sphere() :
    radius(1.0f),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::Sphere(float _radius, glm::vec3 _center) :
    radius(_radius),
    center(_center)
{
}

Sphere::Sphere(float _radius) :
    radius(_radius),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::~Sphere()
{
}

void Sphere::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 2: intersection of ray with sphere
    float discri = pow(dot(ray->direction, ray->origin), 2) - (dot(ray->direction, ray->direction) * dot(ray->origin, ray->origin) - 1 );
    // if there is intersection
    if (discri >= 0) {
        float t0 = (-dot(ray->direction, ray->origin) + sqrt(discri)) / dot(ray->direction, ray->direction);
        float t1 = (-dot(ray->direction, ray->origin) - sqrt(discri)) / dot(ray->direction, ray->direction);
        if (t0 > t1) std::swap(t0, t1);
        if (t0 < intersection->t){
            intersection->t = t0;
            ray->computePoint(intersection->t, intersection->p);
            intersection->n = normalize(intersection->p - center);
            intersection->material = materials[0];
        }
    }
}
