#ifndef RENDERER_H
#define RENDERER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Model.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

enum ProjectionType {
    PERSPECTIVE,
    ORTHOGONAL
};

typedef struct PerpectiveProperties {
    float fieldOfVision;
    float screenWidth;
    float screenHeight;
    float nearDistance;
    float farDistance;

    // PerpectiveProperties() {
    //     this->fieldOfVision = 45.0f;
    //     this->screenWidth = 1080;
    //     this->screenHeight = 720;
    //     this->nearDistance = 0.1f;
    //     this->farDistance = 100.0f;
    // }

    PerpectiveProperties(float screenWidth, float screenHeight, float fieldOfVision = 45.0f, float nearDistance = 0.1f, float farDistance = 100.0f) {
        this->fieldOfVision = fieldOfVision;
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->nearDistance = nearDistance;
        this->farDistance = farDistance;
    }
} PerpectiveProperties;

class Renderer {
   public:
    ProjectionType projectionType;
    PerpectiveProperties perpectiveProperties;
    glm::mat4 projectionMatrix;
    glm::mat4 MVPMatrix;

    Renderer(ProjectionType projectionType, PerpectiveProperties perpectiveProperties, glm::mat4 projectionMatrix, glm::mat4 MVPMatrix) : perpectiveProperties{perpectiveProperties} {
        this->projectionType = projectionType;
        this->projectionMatrix = projectionMatrix;
        this->MVPMatrix = MVPMatrix;
    }

    Renderer(const PerpectiveProperties& perpectiveProperties) : perpectiveProperties{perpectiveProperties} {
        this->projectionType = PERSPECTIVE;
        this->projectionMatrix = glm::perspective(
            glm::radians(perpectiveProperties.fieldOfVision),
            (perpectiveProperties.screenWidth / perpectiveProperties.screenHeight),
            perpectiveProperties.nearDistance,
            perpectiveProperties.farDistance);
        this->MVPMatrix = glm::mat4(1.0f);
    }

    void render(const Model& model, const Camera& camera, const Shader& shader) {
        // this->updateProjectionMatrix(camera.getZoom());

        glm::mat4 viewMatrix = camera.GetViewMatrix();
        this->MVPMatrix = this->projectionMatrix * viewMatrix * model.modelMatrix;
        int MVPLocation = glGetUniformLocation(shader.ID, "MVP");
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(this->MVPMatrix));
        for (unsigned int i = 0; i < model.numMeshes; ++i) {
            unsigned int meshVAO = model.meshes[i].getVertexArrayObjectPointer();

            glBindVertexArray(meshVAO);
            glDrawElements(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    void updateProjectionMatrix(float fieldOfVision) {
        this->perpectiveProperties.fieldOfVision = fieldOfVision;
        this->projectionMatrix = glm::perspective(
            glm::radians(perpectiveProperties.fieldOfVision),
            (perpectiveProperties.screenWidth / perpectiveProperties.screenHeight),
            perpectiveProperties.nearDistance,
            perpectiveProperties.farDistance);
    }
};

#ifdef __cplusplus
}
#endif
#endif