#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

class Shader {
   public:
    unsigned int ID;
    Shader() {
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &_vertexShaderSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
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

    // void setMVPMatrices(const glm::mat4& model) const {
    //     int modelLocation = glGetUniformLocation(this->ID, "model");
    //     glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    // }

    void setMVPMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
        int modelLocation = glGetUniformLocation(this->ID, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        int viewLocation = glGetUniformLocation(this->ID, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLocation = glGetUniformLocation(this->ID, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    }

   private:
    // const char* _vertexShaderSource =
    //     "#version 460 core\n"
    //     "layout (location = 0) in vec3 aPos;\n"
    //     "layout (location = 1) in vec4 aColor;\n"
    //     "out vec4 color;\n"
    //     "void main () {\n"
    //     "\tgl_Position = vec4(aPos, 1.0f);\n"
    //     "\tcolor = aColor;\n"
    //     "}\n\0";

    const char* _vertexShaderSource =
        "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "out vec4 color;\n"
        // "uniform mat4 model;\n"
        // "uniform mat4 view;\n"
        // "uniform mat4 projection;\n"
        "uniform mat4 MVP;\n"
        "void main () {\n"
        // "\tgl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
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

// class Shader {
//    public:
//     unsigned int ID;
//     // constructor generates the shader on the fly
//     // ------------------------------------------------------------------------
//     // Shader() {}
//     Shader(std::string vertexPath, std::string fragmentPath, const char *geometryPath = nullptr) {
//         // 1. retrieve the vertex/fragment source code from filePath
//         std::string vertexCode;
//         std::string fragmentCode;
//         std::string geometryCode;

//         std::fstream vShaderFile;
//         std::fstream fShaderFile;
//         std::fstream gShaderFile;

//         // ensure ifstream objects can throw exceptions:
//         vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//         fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//         gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//         try {
//             std::cout << vertexPath << std::endl;
//             std::cout << fragmentPath << std::endl;
//             // open files
//             vShaderFile.open(vertexPath, fstream::in);
//             fShaderFile.open(fragmentPath, fstream::in);
//             std::stringstream vShaderStream, fShaderStream;
//             // read file's buffer contents into streams
//             vShaderStream << vShaderFile.rdbuf();
//             fShaderStream << fShaderFile.rdbuf();
//             // close file handlers
//             vShaderFile.close();
//             fShaderFile.close();
//             // convert stream into string
//             vertexCode = vShaderStream.str();
//             fragmentCode = fShaderStream.str();

//             // if geometry shader path is present, also load a geometry shader
//             if (geometryPath != nullptr) {
//                 gShaderFile.open(geometryPath);
//                 std::stringstream gShaderStream;
//                 gShaderStream << gShaderFile.rdbuf();
//                 gShaderFile.close();
//                 geometryCode = gShaderStream.str();
//             }
//         } catch (std::ifstream::failure &e) {
//             std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//         }
//         const char *vShaderCode = vertexCode.c_str();
//         const char *fShaderCode = fragmentCode.c_str();
//         // 2. compile shaders
//         unsigned int vertex, fragment;
//         // vertex shader
//         vertex = glCreateShader(GL_VERTEX_SHADER);
//         glShaderSource(vertex, 1, &vShaderCode, NULL);
//         glCompileShader(vertex);
//         checkCompileErrors(vertex, "VERTEX");
//         // fragment Shader
//         fragment = glCreateShader(GL_FRAGMENT_SHADER);
//         glShaderSource(fragment, 1, &fShaderCode, NULL);
//         glCompileShader(fragment);
//         checkCompileErrors(fragment, "FRAGMENT");
//         // if geometry shader is given, compile geometry shader
//         unsigned int geometry;
//         if (geometryPath != nullptr) {
//             const char *gShaderCode = geometryCode.c_str();
//             geometry = glCreateShader(GL_GEOMETRY_SHADER);
//             glShaderSource(geometry, 1, &gShaderCode, NULL);
//             glCompileShader(geometry);
//             checkCompileErrors(geometry, "GEOMETRY");
//         }
//         // shader Program
//         ID = glCreateProgram();
//         glAttachShader(ID, vertex);
//         glAttachShader(ID, fragment);
//         if (geometryPath != nullptr)
//             glAttachShader(ID, geometry);
//         glLinkProgram(ID);
//         checkCompileErrors(ID, "PROGRAM");
//         // delete the shaders as they're linked into our program now and no longer necessery
//         glDeleteShader(vertex);
//         glDeleteShader(fragment);
//         if (geometryPath != nullptr)
//             glDeleteShader(geometry);
//     }
//     // activate the shader
//     // ------------------------------------------------------------------------
//     void use() {
//         glUseProgram(ID);
//     }
//     // utility uniform functions
//     // ------------------------------------------------------------------------
//     void setBool(const std::string &name, bool value) const {
//         glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
//     }
//     // ------------------------------------------------------------------------
//     void setInt(const std::string &name, int value) const {
//         glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
//     }
//     // ------------------------------------------------------------------------
//     void setFloat(const std::string &name, float value) const {
//         glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
//     }
//     // ------------------------------------------------------------------------
//     void setVec2(const std::string &name, const glm::vec2 &value) const {
//         glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
//     }
//     void setVec2(const std::string &name, float x, float y) const {
//         glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
//     }
//     // ------------------------------------------------------------------------
//     void setVec3(const std::string &name, const glm::vec3 &value) const {
//         glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
//     }
//     void setVec3(const std::string &name, float x, float y, float z) const {
//         glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
//     }
//     // ------------------------------------------------------------------------
//     void setVec4(const std::string &name, const glm::vec4 &value) const {
//         glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
//     }
//     void setVec4(const std::string &name, float x, float y, float z, float w) {
//         glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
//     }
//     // ------------------------------------------------------------------------
//     void setMat2(const std::string &name, const glm::mat2 &mat) const {
//         glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }
//     // ------------------------------------------------------------------------
//     void setMat3(const std::string &name, const glm::mat3 &mat) const {
//         glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }
//     // ------------------------------------------------------------------------
//     void setMat4(const std::string &name, const glm::mat4 &mat) const {
//         glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }

//    private:
//     // utility function for checking shader compilation/linking errors.
//     // ------------------------------------------------------------------------
//     const char *_vertexShaderCode =
//         "#version 460 core\n"
//         "layout (location = 0) in vec3 aPos;\n"
//         "layout (location = 1) in vec3 aColor;\n"
//         "out vec3 ourColor;\n"
//         "void main()\n"
//         "{\n"
//         "\tgl_Position = vec4(aPos, 1.0);\n"
//         "\tourColor = aColor;\n"
//         "}\n\0";

//     const char *_fragmentShaderCode =
//         "#version 460 core\n"
//         "out vec4 FragColor;\n"
//         "in vec3 ourColor;\n"
//         "void main()\n"
//         "{\n"
//         "    FragColor = vec4(ourColor, 1.0f);\n"
//         "}\n\0";
//     void checkCompileErrors(GLuint shader, std::string type) {
//         GLint success;
//         GLchar infoLog[1024];
//         if (type != "PROGRAM") {
//             glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//             if (!success) {
//                 glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//                 std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
//                           << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//             }
//         } else {
//             glGetProgramiv(shader, GL_LINK_STATUS, &success);
//             if (!success) {
//                 glGetProgramInfoLog(shader, 1024, NULL, infoLog);
//                 std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
//                           << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//             }
//         }
//     }
// };

#ifdef __cplusplus
}
#endif
#endif