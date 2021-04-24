/** @file Shader.cpp
 *  @brief Class definition for a Shader.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 *  @author G Sathyaram (wreck-count)
 */

#ifndef SHADER_H
#define SHADER_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

/** @class Shader
 *  @brief Defines a shader for diaplying models with color.
 *  @details Compiles and generates a static shader that is used throughout the application. This shader can only display the diffuse color of the material.
 */
class Shader {
   public:
    //! @brief The ID stored in the memory which used to invoke this shader.
    unsigned int ID;

    /**
	 * @brief Default Constructor.
	*/
    Shader() {
        unsigned int vertex, fragment;
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &_materialVertexShaderSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &_materialFragmentShaderSource, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void setModelMatrix(const glm::mat4& model) const {
        int modelLocation = glGetUniformLocation(this->ID, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    }

    void setViewMatrix(const glm::mat4& view) const {
        int viewLocation = glGetUniformLocation(this->ID, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    }

    void setProjectionMatrix(const glm::mat4& projection) const {
        int projectionLocation = glGetUniformLocation(this->ID, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void setWorldAmbientLightColor(const glm::vec3& worldAmbientColor) const {
        int worldAmbientColorLocation = glGetUniformLocation(this->ID, "worldAmbientColor");
        glUniform3fv(worldAmbientColorLocation, 1, glm::value_ptr(worldAmbientColor));
    }

    void setCameraPosition(const glm::vec3& position) const {
        int cameraPosition = glGetUniformLocation(this->ID, "viewPos");
        glUniform3fv(cameraPosition, 1, glm::value_ptr(position));
    }

    void setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float& shininess) const {
        int materialAmbient = glGetUniformLocation(this->ID, "material.ambient");
        glUniform3fv(materialAmbient, 1, glm::value_ptr(ambient));

        int materialDiffuse = glGetUniformLocation(this->ID, "material.diffuse");
        glUniform3fv(materialDiffuse, 1, glm::value_ptr(diffuse));

        int materialSpecular = glGetUniformLocation(this->ID, "material.specular");
        glUniform3fv(materialSpecular, 1, glm::value_ptr(glm::vec3(specular)));

        int materialShininess = glGetUniformLocation(this->ID, "material.shininess");
        glUniform1f(materialShininess, shininess);
    }

    void setLighting(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) const {
        int lightPosition = glGetUniformLocation(this->ID, "light.position");
        glUniform3fv(lightPosition, 1, glm::value_ptr(position));

        int lightAmbient = glGetUniformLocation(this->ID, "light.ambient");
        glUniform3fv(lightAmbient, 1, glm::value_ptr(ambient));

        int lightDiffuse = glGetUniformLocation(this->ID, "light.diffuse");
        glUniform3fv(lightDiffuse, 1, glm::value_ptr(diffuse));

        int lightSpecular = glGetUniformLocation(this->ID, "light.specular");
        glUniform3fv(lightSpecular, 1, glm::value_ptr(specular));
    }

   private:
    const char* _materialVertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "\tFragPos = vec3(model * vec4(aPos, 1.0));\n"
        "\tNormal = mat3(transpose(inverse(model))) * aNormal;  \n"
        "\tgl_Position = projection * view * vec4(FragPos, 1.0);\n"
        "}\n\0";

    const char* _materialFragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "struct Material {\n"
        "\tvec3 diffuse;\n"
        "\tvec3 specular;\n"
        "\tfloat shininess;\n"
        "}; \n"
        "struct Light {\n"
        "\tvec3 position;\n"
        "\tvec3 diffuse;\n"
        "\tvec3 specular;\n"
        "};\n"
        "in vec3 FragPos;  \n"
        "in vec3 Normal;  \n"
        "uniform vec3 worldAmbientColor;\n"
        "uniform vec3 viewPos;\n"
        "uniform Material material;\n"
        "uniform Light light;\n"
        "void main() {\n"
        "\t// ambient\n"
        "\tvec3 ambient = worldAmbientColor;\n"
        "\t// diffuse \n"
        "\tvec3 norm = normalize(Normal);\n"
        "\tvec3 lightDir = normalize(light.position - FragPos);\n"
        "\tfloat diff = max(dot(norm, lightDir), 0.0);\n"
        "\tvec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
        "\t// specular\n"
        "\tvec3 viewDir = normalize(viewPos - FragPos);\n"
        "\tvec3 reflectDir = reflect(-lightDir, norm);  \n"
        "\tfloat spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "\tvec3 specular = light.specular * (spec * material.specular);\n"
        "\tvec3 result = ambient + diffuse + specular;\n"
        "\tFragColor = vec4(result, 1.0);\n"
        "}\n\0";

    const char* _vertexShaderSource =
        "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "out vec4 color;\n"
        "uniform mat4 MVP;\n"
        "void main () {\n"
        "\tgl_Position = MVP * vec4(aPos, 1.0f);\n"
        "\tcolor = aColor;\n"
        "}\n\0";

    const char* _fragmentShaderSource =
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec4 color;\n"
        "void main () {\n"
        "\tFragColor = color;\n"
        "}\n\0";

    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#ifdef __cplusplus
}
#endif
#endif