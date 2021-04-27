/** @file Material.cpp
 *  @brief Class definition for a Material of a Model.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

/** @class Material
 *  @brief Data class for a Material property of 3D Models.
 *  @details This class containes all the properties for a 
 *  given Model's material. This class also handles the 
 *  functionality of updating the values provided by the 
 *  GUI controls.
 */
class Material {
   public:
    /**
    * @brief Construct a new Material object 
	* with default material as white color.
    */
    Material() {
        //Chrome
        // this->ambient = glm::vec3(0.25f, 0.25f, 0.25f);
        // this->diffusion = glm::vec3(0.4f, 0.4f, 0.4f);
        // this->specular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
        this->ambient = glm::vec3(1.0f);
        this->diffusion = glm::vec3(1.0f);
        this->specular = glm::vec3(1.0f);
        this->shininess = 76.8f;
    }

    /**
	 * @brief Construct a new Material object.
	 * 
	 * @param ambient Ambient Color of the material.
	 * @param diffusion Diffusion Color of the material.
	 * @param specular Specular Color of the material.
	 * @param shininess The shininess of the material. Higher the value, more light shines off of the material.
	 */
    Material(glm::vec3 ambient, glm::vec3 diffusion, glm::vec3 specular, float shininess) {
        this->ambient = glm::vec3(ambient);
        this->diffusion = glm::vec3(diffusion);
        this->specular = glm::vec3(specular);
        this->shininess = shininess;
    }

    /**
     * @brief Get the Ambient color of the Material
     * 
     * @return const glm::vec3& 
     */
    const glm::vec3& getMaterialAmbient() const {
        return this->ambient;
    }

    /**
	 * @brief Get the Diffuse color of the Material
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getMaterialDiffuse() const {
        return this->diffusion;
    }

    /**
	 * @brief Get the Specular color of the Material
	 * 
	 * @return const glm::vec3& 
	 */
    const glm::vec3& getMaterialSpecular() const {
        return this->specular;
    }

    /**
	 * @brief Get the Shininess of the Material
	 * 
	 * @return const float 
	 */
    const float getMaterialShininess() const {
        return this->shininess;
    }

    /**
	 * @brief Set the Diffuse Color of the material
	 * 
	 * @param color 
	 */
    void setDiffuseColor(glm::vec3 color) {
        this->diffusion = color;
    }

   private:
    glm::vec3 ambient;
    glm::vec3 diffusion;
    glm::vec3 specular;
    float shininess;
};

#ifdef __cplusplus
}
#endif
#endif