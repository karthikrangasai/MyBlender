/** @file Scene.cpp
 *  @brief Class definition of a 3D Scene.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 */

#ifndef SCENE_H
#define SCENE_H

#include "Light.hpp"
#include "Model.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class Scene {
   public:
    glm::vec3 ambientColor;
    Light light;
    std::vector<Model> models;

    Scene() {
        this->ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
        this->light = Light();
        this->models = std::vector<Model>();

        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/cube.obj"));
        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/sphere.obj"));
        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/teapot.obj"));
        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/pyramid.obj"));
        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/table_2.obj"));
        this->models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/chair.obj"));
    }

    // static Scene openScene() {}

    // void save() {}

   private:
};

#ifdef __cplusplus
}
#endif
#endif