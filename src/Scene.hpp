/** @file Scene.cpp
 *  @brief Class definition of a 3D Scene.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 */

#ifndef SCENE_H
#define SCENE_H

#include "Light.hpp"
#include "Model.hpp"
#include "Physics.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class Scene {
   public:
    glm::vec3 ambientColor;
    Light light;
    std::vector<Model*> models;

    bool isPhysicsOn;
    Physx* physx = nullptr;

    Scene() {
        this->ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
        this->light = Light();
        this->models = std::vector<Model*>();
        this->isPhysicsOn = false;

        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/cube.obj"));
        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/sphere.obj"));
        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/teapot.obj"));
        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/pyramid.obj"));
        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/table_2.obj"));
        // this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/chair.obj"));
    }

    void addModel(Model* model) {
        this->models.push_back(model);
    }

    void addSphereModel() {
        Model model = Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/sphere.obj");
        this->models.push_back(&model);
    }

    void attachPhysics(SolarSystemPhysx* physx) {
        this->physx = physx;
    }

    void attachPhysics(CollisionPhysx* physx) {
        this->physx = physx;
    }

    // static Scene openScene() {}

    // void save() {}
   private:
};

#ifdef __cplusplus
}
#endif
#endif