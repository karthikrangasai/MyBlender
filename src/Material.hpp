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

class Material {
   public:
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
    Material(glm::vec3 ambient, glm::vec3 diffusion, glm::vec3 specular, float shininess) {
        this->ambient = glm::vec3(ambient);
        this->diffusion = glm::vec3(diffusion);
        this->specular = glm::vec3(specular);
        this->shininess = shininess;
    }

    // getter methods
    const glm::vec3& getMaterialAmbient() const {
        return this->ambient;
    }

    const glm::vec3& getMaterialDiffuse() const {
        return this->diffusion;
    }

    const glm::vec3& getMaterialSpecular() const {
        return this->specular;
    }

    const float getMaterialShininess() const {
        return this->shininess;
    }
    //update methods

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