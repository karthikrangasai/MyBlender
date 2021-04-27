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

/** @class Scene
 *  @brief Class for a Scene object.
 */
class Scene {
   public:
    //! The light used in the scene
    Light light;
    //! List of models present in the scene.
    std::vector<Model*> models;

    //! Flag variable to decide whether physics simulation is rendered.
    bool isPhysicsOn;
    //! The type of Physics Simulator to use for the current scene.
    Physx* physx = nullptr;

    /**
	 * @brief Construct a new Scene object
	 */
    Scene() {
        this->light = Light();
        this->models = std::vector<Model*>();
        this->isPhysicsOn = false;
    }

    /**
	 * @brief Adds a Model to the scene.
	 * 
	 * @param model 
	 */
    void addModel(Model* model) {
        this->models.push_back(model);
    }

    /**
	 * @brief Attaches Solar System Physics Simulator to the scene.
	 * 
	 * @param physx 
	 */
    void attachPhysics(SolarSystemPhysx* physx) {
        this->physx = physx;
    }

    /**
	 * @brief Attaches Collision Physics Simulator to the scene.
	 * 
	 * @param physx 
	 */
    void attachPhysics(CollisionPhysx* physx) {
        this->physx = physx;
    }
};

#ifdef __cplusplus
}
#endif
#endif