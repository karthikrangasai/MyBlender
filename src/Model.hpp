/** @file Model.cpp
 *  @brief Class definition for a Model.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 *  @author G Sathyaram (wreck-count)
 */

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

#include "Material.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

const double PI = 3.1415926535;

/**
 * @brief Structure to control the properties of the Perspective Matrix.
*/
typedef struct Vertex {
    //! @brief Position a vertex in the world coordinates.
    glm::vec3 position;

    //! @brief Normals of the current vertex
    glm::vec3 normal;
} Vertex;

/** @class Mesh
 *  @brief Data class for a Mesh object.
 *  @details This class that stores all the vertices of a mesh and the index strcuture of the faces and also created the VAO, VBO, and EBO for the Mesh.
 */
class Mesh {
   public:
    //! @brief List of all vertices of the current Mesh.
    std::vector<Vertex> vertices;

    //! @brief List of all indices of the current Mesh's vertices. (Read as triples, since we deal with Triangulated Polygons.)
    std::vector<unsigned int> indices;

    //! @brief Material of the object.
    Material material;

    /**
	 * @brief Default Constructor.
	*/
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) : material(material) {
        this->vertices = vertices;
        this->indices = indices;

        setupMesh();
    }

    /** @brief getVertexArrayObjectPointer - Return the VAO index in memory for the current Mesh.
 	*
 	* @return unsigned int - The VAO index.
 	*/
    unsigned int getVertexArrayObjectPointer() const {
        return this->VAO;
    }

   private:
    unsigned int VAO, VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex colors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glBindVertexArray(0);
    }
};

/** @class Model
 *  @brief Data class for a Model object.
 *  @details This class stores all the Meshes for a given Model. 
 *  This class also handles the functionality of the Model Matrix used 
 *  during the display of the objects in the scene. Model transformations 
 *  in the world space is also possible via this class.
 */
class Model {
   public:
    //! @brief Defines whether object is visible on the screen.
    bool visibility;

    //! @brief Defines the whether object's transforms can be changed.
    bool control;

    //! @brief List of all the meshes for the model.
    std::vector<Mesh> meshes;
    //! @brief Number of meshes for the model.
    std::uint32_t numMeshes;

    glm::vec3 worldPosition;

    glm::vec3 translation;
    //! @brief Rotation vector (rx,ry,rz) used in the Transformation operation's matrix.
    glm::vec3 rotation;
    //! @brief Scaling vector (sx,sy,sz) used in the Transformation operation's matrix.
    glm::vec3 scale;

    //! @brief The Model matrix that transforms the object in the world coordinate space.
    glm::mat4 modelMatrix;

    //! @brief The 3-tuple to store the updates values of translation from the GUI.
    float _translation[3];
    //! @brief The 3-tuple to store the updates values of rotation from the GUI.
    float _rotation[3];
    //! @brief The 3-tuple to store the updates values of scale from the GUI.
    float _scale[3];

    /**
	 * @brief Construct a new Model object from the OBJ file specified using the path.
	 * 
	 * @param path Absolute path to the location of the model's Wavefront Object file in the OS.
	*/
    Model(std::string const& path) {
        loadmodel(path);

        for (int i = 0; i < 3; ++i) {
            _translation[i] = _rotation[i] = 0.0f;
            _scale[i] = 1.0f;
        }

        this->worldPosition = glm::vec3(0.0f);
        this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        this->modelMatrix = glm::mat4(1.0f);
        visibility = true;
        control = false;
    }

    /** @brief updateTransforms - Transform the object properties.
 	* @details Applies the translation, rotation, and scaling transformations to the current model as updated in the GUI.
	* 
 	* @return void
 	*/
    void updateTransforms() {
        this->worldPosition = glm::vec3(this->_translation[0], this->_translation[1], this->_translation[2]);
        this->translation = glm::vec3(this->_translation[0], this->_translation[1], this->_translation[2]);
        this->rotation = glm::vec3(this->_rotation[0], this->_rotation[1], this->_rotation[2]);
        this->scale = glm::vec3(this->_scale[0], this->_scale[1], this->_scale[2]);
        this->updateModelMatrix();
    }

    void updateGlobalTransforms(float* translation, float* rotation, float* scale) {
        if (this->control) {
            for (int i = 0; i < 3; ++i) {
                translation[i] = _translation[i];
                rotation[i] = _rotation[i];
                scale[i] = _scale[i];
            }
        }
    }

    /** @brief reset - Reset all the Model properties.
 	* @details Resets all the model properties i.e. translation and rotation 0 scaling to 1.
	* Updates the Model Matrix to Identity Matrix and sets visibility to true, control to false (to avoid accidental value changes).
	* 
 	* @return void
 	*/
    void reset() {
        for (int i = 0; i < 3; ++i) {
            _translation[i] = _rotation[i] = 0.0f;
            _scale[i] = 1.0f;
        }

        this->updateTransforms();
        visibility = true;
        control = false;
    }

    /**
	 * @brief Get the Model Matrix of the current Model
	 * 
	 * @return const glm::mat4& 
	 */
    const glm::mat4& getModelMatrix() const {
        return this->modelMatrix;
    }

   protected:
    /**
    * @brief Construct a new Model object using the mesh provided.
    * 
    * @param mesh 
    */
    Model(Mesh mesh) {
        this->meshes.push_back(mesh);
        this->numMeshes = meshes.size();
        for (int i = 0; i < 3; ++i) {
            _translation[i] = _rotation[i] = 0.0f;
            _scale[i] = 1.0f;
        }
        this->worldPosition = glm::vec3(0.0f);
        this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        this->modelMatrix = glm::mat4(1.0f);
        visibility = true;
        control = false;
    }

    /**
	 * @brief Update the Model Matric based on the
	 * transformations made to the Model in the 3D scene.
	 */
    void updateModelMatrix() {
        this->modelMatrix = glm::translate(glm::mat4(1.0f), this->translation);
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        this->modelMatrix = glm::scale(this->modelMatrix, this->scale);
    }

    /**
	 * @brief Read an OBJ file specified using the path to read Model data
	 * 
	 * @param path 
	 */
    void loadmodel(const std::string path) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ASSIMP::ERROR::" + std::string(importer.GetErrorString()) << std::endl;
            return;
        }

        numMeshes = scene->mNumMeshes;
        this->meshes.reserve(numMeshes);

        // glm::mat4 blenderToOpenGL = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 blenderToOpenGL(1.0f);

        for (std::uint32_t i = 0u; i < scene->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[i];

            // Extract Material for this Mesh
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            aiColor4D ambientColor;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientColor);

            aiColor4D diffuseColor;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);

            aiColor4D specularColor;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColor);

            float shininess;
            aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

            Material meshMaterial = Material(
                glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
                glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a),
                glm::vec4(specularColor.r, specularColor.g, specularColor.b, shininess),
                shininess);

            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            for (std::uint32_t j = 0; j < mesh->mNumVertices; ++j) {
                Vertex vertex;
                glm::vec3 posVector;
                posVector.x = mesh->mVertices[j].x;
                posVector.y = mesh->mVertices[j].y;
                posVector.z = mesh->mVertices[j].z;

                glm::vec3 normalVector;
                normalVector.x = mesh->mNormals[j].x;
                normalVector.y = mesh->mNormals[j].y;
                normalVector.z = mesh->mNormals[j].z;

                glm::vec4 transformed = blenderToOpenGL * glm::vec4(posVector, 1.0);
                vertex.position = glm::vec3(transformed.x, transformed.y, transformed.z);

                transformed = blenderToOpenGL * glm::vec4(normalVector, 1.0);
                vertex.normal = glm::vec3(transformed.x, transformed.y, transformed.z);

                vertices.push_back(vertex);
            }

            std::vector<std::uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3u);
            for (std::uint32_t k = 0u; k < mesh->mNumFaces; ++k) {
                indices.push_back(mesh->mFaces[k].mIndices[0u]);
                indices.push_back(mesh->mFaces[k].mIndices[1u]);
                indices.push_back(mesh->mFaces[k].mIndices[2u]);
            }
            this->meshes.push_back(Mesh(vertices, indices, meshMaterial));
        }
    }
};

/** @class Sphere
 *  @brief Class for a Sphere model inherited from Model class.
 *  @details This class stores all the Meshes for a given Sphere.
 */
class Sphere : public Model {
   public:
    float radius;

    /**
	 * @brief Construct a new Sphere object
	 * 
	 * @param radius 
	 * @param resolution 
	 */
    Sphere(float radius, unsigned resolution) : Model(Sphere::generateSphere(radius, resolution)) {
        this->radius = radius;
    }

    /**
	 * @brief Procedurally generate a sphere mesh using the resolution and radius
	 * 
	 * @param resolution 
	 * @param resolution 
	 * @param radius 
	 * @return Mesh 
	 */
    static Mesh generateSphere(float radius, unsigned resolution) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned indexCount = 6 * resolution * (resolution - 1);
        unsigned vertCount = (resolution + 1) * (resolution + 1);

        indices.resize(indexCount);
        vertices.resize(vertCount);

        float lonStep = (2 * PI) / resolution;
        float latStep = PI / resolution;

        for (unsigned lat = 0, v = 0; lat <= resolution; lat++) {
            for (unsigned lon = 0; lon <= resolution; lon++, v++) {
                vertices[v].normal = glm::vec3(
                    cos(lon * lonStep) * sin(lat * latStep),
                    cos(lat * latStep - PI),
                    sin(lon * lonStep) * sin(lat * latStep));

                vertices[v].position = glm::vec3(
                    radius * vertices[v].normal.x,
                    radius * vertices[v].normal.y,
                    radius * vertices[v].normal.z);
            }
        }
        unsigned i = 0;
        unsigned v = resolution + 1;
        for (unsigned lon = 0; lon < resolution; lon++, v++) {
            indices[i++] = lon;
            indices[i++] = v;
            indices[i++] = v + 1;
        }

        v = resolution + 1;
        for (unsigned lat = 1; lat < resolution - 1; lat++, v++) {
            for (unsigned lon = 0; lon < resolution; lon++, v++) {
                indices[i++] = v;
                indices[i++] = v + resolution + 1;
                indices[i++] = v + 1;

                indices[i++] = v + 1;
                indices[i++] = v + resolution + 1;
                indices[i++] = v + resolution + 2;
            }
        }

        for (unsigned lon = 0; lon < resolution; lon++, v++) {
            indices[i++] = v;
            indices[i++] = v + resolution + 1;
            indices[i++] = v + 1;
        }

        Material material;

        return Mesh(vertices, indices, material);
    }
};

/** @class Plane
 *  @brief Class for a Plane model inherited from Model class.
 *  @details This class stores all the Meshes for a given Plane.
 */
class Plane : public Model {
   public:
    glm::vec3 normal;
    float Odist = 0.0f;

    /**
	 * @brief Construct a new Plane object from the OBJ file specified in the path.
	 * 
	 * @param path 
	 */
    Plane(std::string const& path) : Model(path) {
        this->normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    /**
	 * @brief Construct a new Plane object
	 * 
	 * @param scale 
	 */
    Plane(unsigned scale) : Model(Plane::generatePlane(scale)) {
        this->normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    /**
	 * @brief Updates the model matrix and the normal of the
	 * plane based on the transforms applied to the Model.
	 */
    void updateTransforms() {
        this->worldPosition = glm::vec3(this->_translation[0], this->_translation[1], this->_translation[2]);
        this->translation = glm::vec3(this->_translation[0], this->_translation[1], this->_translation[2]);
        this->rotation = glm::vec3(this->_rotation[0], this->_rotation[1], this->_rotation[2]);
        this->scale = glm::vec3(this->_scale[0], this->_scale[1], this->_scale[2]);

        glm::mat4 normalRotator = glm::translate(glm::mat4(1.0f), this->translation);
        normalRotator = glm::rotate(normalRotator, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        normalRotator = glm::rotate(normalRotator, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        normalRotator = glm::rotate(normalRotator, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        this->normal = glm::vec3(normalRotator * glm::vec4(this->normal, 1.0f));
        this->normal = glm::normalize(this->normal);

        this->updateOdist();
        this->updateModelMatrix();
    }

    void updateOdist() {
        this->Odist = -1.0f * glm::dot(this->worldPosition, this->normal) / glm::sqrt(glm::dot(this->normal, this->normal));
    }

    /**
	 * @brief Procedurally generate a sphere mesh using the resolution and radius
	 * 
	 * @param scale 
	 * @return Mesh 
	 */
    static Mesh generatePlane(unsigned scale) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned indexCount = 6;
        unsigned vertCount = 4;

        indices.resize(indexCount);
        vertices.resize(vertCount);

        vertices[0].position = glm::vec3(scale, 0, scale);
        vertices[0].normal = glm::vec3(0, 1, 0);

        vertices[1].position = glm::vec3(scale, 0, -scale);
        vertices[1].normal = glm::vec3(0, 1, 0);

        vertices[2].position = glm::vec3(-scale, 0, scale);
        vertices[2].normal = glm::vec3(0, 1, 0);

        vertices[3].position = glm::vec3(-scale, 0, -scale);
        vertices[3].normal = glm::vec3(0, 1, 0);

        indices[0] = 3;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 2;
        indices[4] = 1;
        indices[5] = 0;

        Material material;

        return Mesh(vertices, indices, material);
    }
};

#ifdef __cplusplus
}
#endif
#endif