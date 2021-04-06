/** @file Renderer.cpp
 *  @brief Class definition for a Renderer.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 *  @author G Sathyaram (wreck-count)
 */

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

/// @enum ProjectionType
/* @brief Enumeration used a state specifier for the projection type for a camera. */
typedef enum ProjectionType {
    PERSPECTIVE, /* Denotes the perspective projection */
    ORTHOGONAL   /* Denotes the orthogonal projection */
} ProjectionType;

/**
 * @brief Structure to control the properties of the Perspective Matrix.
*/
typedef struct PerpectiveProperties {
    //! @brief Angle we look at, at any given time.
    float fieldOfVision;

    //! @brief Screen width - X Axis.
    float screenWidth;

    //! @brief Screen Height  - Y Axis.
    float screenHeight;

    //! @brief Distance of the nearest point we can see. (Like for human eye).
    float nearDistance;

    //! @brief Distance of the farthest point we can see. (Like for human eye).
    float farDistance;

    /**
	 * @brief Default Constructor.
	*/
    PerpectiveProperties(float screenWidth, float screenHeight, float fieldOfVision = 45.0f, float nearDistance = 0.1f, float farDistance = 100.0f) {
        this->fieldOfVision = fieldOfVision;
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->nearDistance = nearDistance;
        this->farDistance = farDistance;
    }
} PerpectiveProperties;

/** @class Renderer
 *  @brief Class to handle all the rendering.
 *  @details This class handles all the rendering overhead like setting up the MVP matrices in the shaders's uniforms as well as calling all the necessary draw the methods.
 */
class Renderer {
   public:
    //! @brief Decides the projection type between Perspective and Orthogonal. Defaults to Perspective.
    ProjectionType projectionType;

    //! @brief Holds all the current properties of the perspective projectin changed by the user.
    PerpectiveProperties perpectiveProperties;

    //! @brief The projection matrix of the current MVP triple.
    glm::mat4 projectionMatrix;

    //! @brief The product of Model, View, Projection matrices.
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

    /** @brief render - Render a scene.
     * @details This function renders a model thorugh a camera using the provided shader.
     *
     * @param model The current model to be rendered.
     * @param camera The camera through render through. (Used to obtain the view matrix)
     * @param shader The shader needed to be used.
     * 
     * @return void
    */
    void render(const Model& model, const Camera& camera, const Shader& shader) {
        glm::mat4 viewMatrix = camera.getViewMatrix();
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

    void setMVPMatrices(const Model& model, const Camera& camera, const Shader& shader) {
        int MLocation = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(MLocation, 1, GL_FALSE, glm::value_ptr(model.modelMatrix));

        int VLocation = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(VLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

        int PLocation = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(this->projectionMatrix));
    }

    /** @brief updateProjectionMatrix - Update Feild of Vision of the Perspective Projection.
 	*
 	* @param fieldOfVision The desired angle value for field of vison. Provide as a float.
	* 
 	* @return void
 	*/
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