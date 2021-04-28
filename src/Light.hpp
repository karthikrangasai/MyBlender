/** @file Light.cpp
 *  @brief Class definition for Light present in a 3D scene.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/** @class Light
 *  @brief Data class for a Light object.
 *  @details This class containes all the properties for a Light object 
 *  used in a 3D scene. This class also handles the functionality of updating
 *  the values provided by the GUI controls.
 */
class Light {
   public:
    float _position[3];
    float _ambient[3];
    float _diffuse[3];
    float _specular[3];

    /**
	 * @brief Construct a new Light object 
	 * with default light color as white color.
	 */
    Light() {
        this->position = glm::vec3(10.f, 60.f, 10.f);
        this->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
        this->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        this->specular = glm::vec3(1.0f, 1.0f, 1.0f);

        this->_position[0] = 10.0f;
        this->_position[1] = 60.0f;
        this->_position[2] = 10.0f;

        this->_ambient[0] = 0.2f;
        this->_ambient[1] = 0.2f;
        this->_ambient[2] = 0.2f;

        this->_diffuse[0] = 1.0f;
        this->_diffuse[1] = 1.0f;
        this->_diffuse[2] = 1.0f;

        this->_specular[0] = 1.0f;
        this->_specular[1] = 1.0f;
        this->_specular[2] = 1.0f;
    }

    /**
	 * @brief Update the properties of the light 
	 * object as provided using the GUI controls.
	 */
    void updateLighting() {
        this->position = glm::vec3(_position[0], _position[1], _position[2]);
        this->ambient = glm::vec3(_ambient[0], _ambient[1], _ambient[2]);
        this->diffuse = glm::vec3(_diffuse[0], _diffuse[1], _diffuse[2]);
        this->specular = glm::vec3(_specular[0], _specular[1], _specular[2]);
    }

    /**
	 * @brief Get the Light Position
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getLightPosition() const {
        return this->position;
    }

    /**
	 * @brief Get the Light Ambient
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getLightAmbient() const {
        return this->ambient;
    }

    /**
	 * @brief Get the Light Diffuse
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getLightDiffuse() const {
        return this->diffuse;
    }

    /**
	 * @brief Get the Light Specular
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getLightSpecular() const {
        return this->specular;
    }

   private:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#ifdef __cplusplus
}
#endif
#endif