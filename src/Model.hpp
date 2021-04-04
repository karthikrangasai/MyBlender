#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>

// #include "Shader.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    // Vertex(glm::vec3 position, glm::vec4 color) {
    //     this->position = position;
    //     this->color = color;
    // }
} Vertex;

class Mesh {
   public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;

        setupMesh();
    }

    unsigned int getVertexArrayObjectPointer() const {
        return this->VAO;
    }
    // void Draw(Shader& shader, const glm::mat4& modelMatrix) {
    //     glUseProgram(shader.ID);
    //     // shader.setMVPMatrices();
    //     glBindVertexArray(VAO);
    //     glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //     glBindVertexArray(0);
    // }

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
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glBindVertexArray(0);
    }
};

class Model {
   public:
    std::vector<Mesh> meshes;
    std::uint32_t numMeshes;
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;

    Model(std::string const& path) {
        loadmodel(path);
        this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        this->modelMatrix = glm::mat4(1.0f);
    }

    // draws the model, and thus all its meshes
    // void Draw(Shader& shader, const glm::mat4& modelMatrix) {
    //     for (unsigned int i = 0; i < numMeshes; i++)
    //         this->meshes[i].Draw(shader, modelMatrix);
    // }

    void updateModelMatrix() {
        this->modelMatrix = glm::translate(glm::mat4(1.0f), this->translation);
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        this->modelMatrix = glm::scale(this->modelMatrix, this->scale);
    }

   private:
    void loadmodel(const std::string path) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ASSIMP::ERROR::" + std::string(importer.GetErrorString()) << std::endl;
            return;
        }

        // Load the Model
        numMeshes = scene->mNumMeshes;
        this->meshes.reserve(numMeshes);

        glm::mat4 blenderToOpenGL = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        for (std::uint32_t i = 0u; i < scene->mNumMeshes; ++i) {
            std::cout << "Mesh #" << i << ":" << std::endl;
            aiMesh* mesh = scene->mMeshes[i];

            // Extract Material for this Mesh
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor4D diffuseColor;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);

            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            for (std::uint32_t j = 0; j < mesh->mNumVertices; ++j) {
                aiVector3D position = mesh->mVertices[j];
                std::cout << "    Vertex #" << j << ": (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[j].x;
                vector.y = mesh->mVertices[j].y;
                vector.z = mesh->mVertices[j].z;

                // vertex.position = vector;

                glm::vec4 transformed = blenderToOpenGL * glm::vec4(vector, 1.0);
                std::cout << "    Vertex #" << j << ": (" << transformed.x << ", " << transformed.y << ", " << transformed.z << ")" << std::endl;
                vertex.position = glm::vec3(transformed.x, transformed.y, transformed.z);

                vertex.color = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);

                vertices.push_back(vertex);
                // vertices.push_back(Vertex(vector, glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a)));
            }

            std::vector<std::uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3u);
            for (std::uint32_t k = 0u; k < mesh->mNumFaces; ++k) {
                std::cout << "    Face #" << k << ": (" << mesh->mFaces[k].mIndices[0u] << ", " << mesh->mFaces[k].mIndices[1u] << ", " << mesh->mFaces[k].mIndices[2u] << ")" << std::endl;
                indices.push_back(mesh->mFaces[k].mIndices[0u]);
                indices.push_back(mesh->mFaces[k].mIndices[1u]);
                indices.push_back(mesh->mFaces[k].mIndices[2u]);
            }
            this->meshes.push_back(Mesh(vertices, indices));
        }
    }
};

#ifdef __cplusplus
}
#endif
#endif