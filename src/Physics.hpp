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

#include "Model.hpp"

#ifdef __cplusplus
extern "C" {
#endif

const float g = 9.81f;
const double G = 6.67430e-11;
const float SUN_MASS = 1.989E7;

enum PhysxShape {
    PLANE,
    SPHERE,
};

typedef struct PhysxObject {
    PhysxShape shape;
    Model* model;
    glm::vec3 velocity;
    glm::vec3 force;
    float mass;

    PhysxObject(PhysxShape shape, Model* model, float mass, glm::vec3 initVelocity) {
        this->shape = shape;
        this->model = model;
        this->mass = mass;
        this->velocity = glm::vec3(initVelocity);
        this->force = this->mass * glm::vec3(0.0f);
    }

    void enableGravity() {
        this->force = this->mass * glm::vec3(0, -g, 0);
    }
} PhysxObject;

class Physx {
   public:
    void addObject(PhysxObject* object) {
        this->objects.push_back(object);
    }

    virtual void step(float dt) = 0;

   protected:
    std::vector<PhysxObject*> objects;
    glm::vec3 gravity = glm::vec3(0, -9.81f, 0);
};

class SolarSystemPhysx : public Physx {
    virtual void step(float dt) {
        int numObjects = this->objects.size();
        PhysxObject* sun = this->objects[0];
        for (int i = 1; i < numObjects; ++i) {
            PhysxObject* p = this->objects[i];
            p->force = glm::vec3(0);
            float gForce = p->mass * sun->mass / glm::pow(glm::distance(p->model->worldPosition, sun->model->worldPosition), 2);

            // std::cout << ">>> Distance: " << glm::distance(p->model->worldPosition, sun->model->worldPosition) << endl;
            glm::vec3 gForceDirection = glm::normalize(sun->model->worldPosition - p->model->worldPosition);
            p->force += gForceDirection * gForce;
            p->model->worldPosition += p->velocity * dt;
            p->velocity += (p->force / p->mass) * dt;
            // std::cout << ">>> Velocity: " << p->velocity.x << " " << p->velocity.y << " " << p->velocity.z << endl;

            // std::cout << ">>> Position: " << p->model->worldPosition.x << " " << p->model->worldPosition.y << " " << p->model->worldPosition.z << endl;
            // update model transforms and modelMatrix
            p->model->_translation[0] = p->model->worldPosition.x;
            p->model->_translation[1] = p->model->worldPosition.y;
            p->model->_translation[2] = p->model->worldPosition.z;
            p->model->updateTransforms();
        }
    }
};

/*
	PhysxObject {
		Model
		Mass
		Position
		Velocity
		type: PLANE or SPHERE
	}

	Dynamics {
		How to update
		
		for all Physx objects {
			v = v + (mg/m) * dt
			x = x + v * dt
		}
	}

	CollisionDetection {
		Sphere v Sphere
		Sphere v Plane

		If time is there, add (GJK + EPA) Algo
	}

	CollisionResponse {
		Solver : {
			Impulse Solver,
			Position Solver
		}
	}


	Final Workflow:
		while((I'm an a rendering) && (an a Physx enabled)) {
			CollisionDetection;
			CollisionResponse;
			Dynamics;

			then make render calls;
		}
	
	Best way to show output is to make a
		- billiards table simulation
		- fountain of balls shooting out and colliding on ground
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
                        std::cout << i << " " << j << endl;
                        this->solvePlaneSphereCollision(p, q);
                    }
                    // this->stepSphere(q, dt);
                } else if (p->shape == SPHERE and q->shape == PLANE) {
                    if (this->testPlaneSphereCollision(q, p)) {
                        std::cout << j << " " << i << endl;
                        this->solvePlaneSphereCollision(q, p);
                    }
                    // this->stepSphere(p, dt);
                } else if (p->shape == SPHERE and q->shape == SPHERE) {
                    if (this->testSphereSphereCollision(p, q)) {
                        solveSphereSphereCollision(p, q);
                    }
                    // stepSphere(p, dt);
                    // stepSphere(q, dt);
                }
            }
        }

        for (int i = 0; i < numObjects; ++i) {
            PhysxObject* p = objects[i];
            if (p->shape == PhysxShape::SPHERE) {
                this->stepSphere(p, dt);
                p->velocity += (p->force / p->mass) * dt;
                // std::cout << ">>> Velocity: " << p->velocity.x << " " << p->velocity.y << " " << p->velocity.z << endl;
            }
        }
    }

    bool testPlaneSphereCollision(PhysxObject* plane, PhysxObject* sphere) {
        Plane* p = static_cast<Plane*>(plane->model);
        Sphere* s = static_cast<Sphere*>(sphere->model);
        // glm::vec3 ncap = glm::normalize(p->normal);
        return (fabs(p->Odist + glm::dot(s->worldPosition, p->normal)) <= s->radius);
    }

    void solvePlaneSphereCollision(PhysxObject* plane, PhysxObject* sphere) {
        Plane* p = static_cast<Plane*>(plane->model);
        float dotP = glm::dot(p->normal, sphere->velocity);
        if (dotP > 0) {
            p->normal = -p->normal;
        }
        glm::vec3 ncap = p->normal;
        glm::vec3 lcap = glm::normalize(sphere->velocity);
        float dotnl = glm::dot(ncap, lcap);
        glm::vec3 rcap = 2 * dotnl * ncap - lcap;
        // rcap*=glm::length(sphere->velocity);
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

        // std::cout << ">>> Position: " << s->worldPosition.x << " " << s->worldPosition.y << " " << s->worldPosition.z << endl;
        // update model transforms and modelMatrix
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