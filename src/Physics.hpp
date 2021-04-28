/** @file Physics.cpp
 *  @brief Class definition for a Physics.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 *  @author G Sathyaram (wreck-count)
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <cmath>
#include "Model.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//! Constant of Accelaration of gravity
const float g = 9.81f;

//! Universal Gravitational Constant
const double G = 6.67430e-11;

//! Scaled mass of the Sun for the simulation
const float SUN_MASS = 1.989E7;

/**
 * @enum PhysxShape
 * @brief Shape of the Object used in the physics simulation.
 * 
 */
enum PhysxShape {
    PLANE,
    SPHERE,
};

/**
 * @struct PhysxObject
 * @brief Physics Object of the Model used in the simulation.
 */
typedef struct PhysxObject {
    //! Shape of the Model.
    PhysxShape shape;
    //! Model from the scene used for physics simulation.
    Model* model;
    //! Velocity of the object.
    glm::vec3 velocity;
    //! Force being applied on the object.
    glm::vec3 force;
    //! Mass of the object
    float mass;

    bool gravityEnabled = false;
    bool airResistanceEnabled = false;

    /**
	 * @brief Construct a new PhysxObject
	 * 
	 * @param shape 
	 * @param model 
	 * @param mass 
	 * @param initVelocity 
	 */
    PhysxObject(PhysxShape shape, Model* model, float mass, glm::vec3 initVelocity) {
        this->shape = shape;
        this->model = model;
        this->mass = mass;
        this->velocity = glm::vec3(initVelocity);
        this->force = this->mass * glm::vec3(0.0f);
    }

    /**
	 * @brief Enables gravity for the current object.
	 */
    void enableGravity() {
        gravityEnabled = true;

        this->force = this->mass * glm::vec3(0, -g, 0);
    }

    /**
	 * @brief Enables gravity for the current object.
	 */
    void enableAirResistance() {
        airResistanceEnabled = true;
        if (this->shape == PhysxShape::SPHERE) {
            Sphere* s = static_cast<Sphere*>(this->model);
            this->force += this->mass * (6.0f * 3.1415f * 0.1f * s->radius) * this->velocity;
        }
    }

    void recomputeTotalForce() {
        this->force = this->mass * glm::vec3(0.0f);

        if (gravityEnabled) {
            this->force += this->mass * glm::vec3(0, -g, 0);
        }

        if (airResistanceEnabled) {
            if (this->shape == PhysxShape::SPHERE) {
                Sphere* s = static_cast<Sphere*>(this->model);
                this->force -= this->mass * (6.0f * 3.1415f * 0.0f * s->radius) * this->velocity;
            }
        }
    }
} PhysxObject;

/** @class Physx
 *  @brief Handles the Physics calculations for all objects in the scene with physics enabled.
 */
class Physx {
   public:
    /**
	 * @brief Consider the Model specified through the PhysxObject for all physics calculations.
	 * 
	 * @param object 
	 */
    void addObject(PhysxObject* object) {
        this->objects.push_back(object);
    }

    /**
	 * @brief Calculate all the changes governed by the physics of the system for a single time step.
	 * 
	 * @param dt 
	 */
    virtual void step(float dt) = 0;

   protected:
    //! List of all objects interacting in the simulation.
    std::vector<PhysxObject*> objects;
};

/** @class SolarSystemPhysx
 *  @brief Handles the Physics for the Solar System Simulation.
 */
class SolarSystemPhysx : public Physx {
    virtual void step(float dt) {
        int numObjects = this->objects.size();
        PhysxObject* sun = this->objects[0];
        for (int i = 1; i < numObjects; ++i) {
            PhysxObject* p = this->objects[i];
            p->force = glm::vec3(0);
            float gForce = p->mass * sun->mass / glm::pow(glm::distance(p->model->worldPosition, sun->model->worldPosition), 2);
            glm::vec3 gForceDirection = glm::normalize(sun->model->worldPosition - p->model->worldPosition);
            p->force += gForceDirection * gForce;
            p->model->worldPosition += p->velocity * dt;
            p->velocity += (p->force / p->mass) * dt;
            p->model->_translation[0] = p->model->worldPosition.x;
            p->model->_translation[1] = p->model->worldPosition.y;
            p->model->_translation[2] = p->model->worldPosition.z;
            p->model->updateTransforms();
        }
    }
};

/** @class CollisionPhysx
 *  @brief Handles the Physics for the Collision Simulation.
 */
class CollisionPhysx : public Physx {
    virtual void step(float dt) {
        int numObjects = this->objects.size();
        for (int i = 0; i < numObjects; ++i) {
            PhysxObject* p = objects[i];
            for (int j = 0; j < i; ++j) {
                PhysxObject* q = objects[j];
                if (p->shape == PLANE and q->shape == SPHERE) {
                    if (this->testPlaneSphereCollision(p, q)) {
                        this->solvePlaneSphereCollision(p, q);
                    }
                } else if (p->shape == SPHERE and q->shape == PLANE) {
                    if (this->testPlaneSphereCollision(q, p)) {
                        this->solvePlaneSphereCollision(q, p);
                    }
                } else if (p->shape == SPHERE and q->shape == SPHERE) {
                    if (this->testSphereSphereCollision(p, q)) {
                        solveSphereSphereCollision(p, q);
                    }
                }
            }
        }

        for (int i = 0; i < numObjects; ++i) {
            PhysxObject* p = objects[i];
            if (p->shape == PhysxShape::SPHERE) {
                this->stepSphere(p, dt);
                p->recomputeTotalForce();
                p->velocity += (p->force / p->mass) * dt;
                this->stepSphere(p, dt);
            }
        }
    }

    bool testPlaneSphereCollision(PhysxObject* plane, PhysxObject* sphere) {
        Plane* p = static_cast<Plane*>(plane->model);
        Sphere* s = static_cast<Sphere*>(sphere->model);
        float dotP = glm::dot(p->normal, sphere->velocity);
        if (dotP > 0) {
            p->normal = -1.0f * p->normal;
        }
        p->updateOdist();
        float dist = fabs(p->Odist + glm::dot(s->worldPosition, p->normal));
        return (dist <= s->radius);
    }

    void solvePlaneSphereCollision(PhysxObject* plane, PhysxObject* sphere) {
        Plane* p = static_cast<Plane*>(plane->model);
        glm::vec3 ncap = p->normal;
        glm::vec3 lcap = glm::normalize(sphere->velocity);
        float dotnl = glm::dot(ncap, lcap);
        glm::vec3 rcap = 2 * dotnl * ncap - lcap;
        rcap = glm::normalize(rcap);
        sphere->velocity = (-1.0f) * rcap * glm::length(sphere->velocity);
        return;
    }

    bool testSphereSphereCollision(PhysxObject* sphereOne, PhysxObject* sphereTwo) {
        Sphere* sOne = static_cast<Sphere*>(sphereOne->model);
        Sphere* sTwo = static_cast<Sphere*>(sphereTwo->model);
        return (glm::distance(sOne->worldPosition, sTwo->worldPosition) <= (sOne->radius + sTwo->radius));
    }

    void solveSphereSphereCollision(PhysxObject* sphereOne, PhysxObject* sphereTwo) {
        Sphere* s1 = static_cast<Sphere*>(sphereOne->model);
        Sphere* s2 = static_cast<Sphere*>(sphereTwo->model);
        glm::vec3 d = glm::normalize(s2->worldPosition - s1->worldPosition);
        glm::vec3 _v1 = sphereOne->velocity - sphereTwo->velocity;
        glm::vec3 __v2 = (2.0f) * d * (sphereOne->mass / (sphereTwo->mass + sphereOne->mass)) * glm::dot(_v1, d);
        glm::vec3 u2 = sphereTwo->velocity + __v2;
        glm::vec3 u1 = (sphereOne->mass * sphereOne->velocity + sphereTwo->mass * sphereTwo->velocity - sphereTwo->mass * u2) / sphereOne->mass;
        sphereOne->velocity = u1;
        sphereTwo->velocity = u2;
        return;
    }

    void stepSphere(PhysxObject* sphere, float dt) {
        Sphere* s = static_cast<Sphere*>(sphere->model);
        s->worldPosition += sphere->velocity * dt;
        s->_translation[0] = s->worldPosition.x;
        s->_translation[1] = s->worldPosition.y;
        s->_translation[2] = s->worldPosition.z;
        s->updateTransforms();
    }
};

#ifdef __cplusplus
}
#endif
#endif