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
        glShaderSource(vertex, 1, &_vertexShaderSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &_fragmentShaderSource, NULL);
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

    void setMVPMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
        int modelLocation = glGetUniformLocation(this->ID, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        int viewLocation = glGetUniformLocation(this->ID, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLocation = glGetUniformLocation(this->ID, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    }

   private:
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