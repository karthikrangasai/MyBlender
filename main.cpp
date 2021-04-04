#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "src/Model.hpp"
#include "src/Camera.hpp"
#include "src/Shader.hpp"
#include "src/Renderer.hpp"

void logString(const std::string& s);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

bool firstMouse = true;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

Camera camera = Camera();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD gayed" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Model cubeModel = Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/cube.obj");
    Shader shader = Shader();
    Renderer renderer = Renderer(PerpectiveProperties(SCR_WIDTH, SCR_HEIGHT));

    // // create transformations
    // glm::mat4 model = glm::mat4(1.0f);
    // // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // int modelLoc = glGetUniformLocation(shader.ID, "model");
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    // // // note that we're translating the scene in the reverse direction of where we want to move
    // glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // glm::mat4 view;  // = camera.GetViewMatrix();
    // // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // glm::mat4 projection;
    // projection = glm::perspective(glm::radians(45.0f), ((float)SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
    // int projLoc = glGetUniformLocation(shader.ID, "projection");
    // glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    // int viewLoc = glGetUniformLocation(shader.ID, "view");
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);

    // shader.setMVPMatrices(model);

    // glm::mat4 MVP = projection * view * model;
    // glm::mat4 MVP;  // = projection * view * model;

    glUseProgram(shader.ID);

    // int MVPLocation = glGetUniformLocation(shader.ID, "MVP");
    // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        /* RENDER CALL */

        // view = camera.GetViewMatrix();
        // MVP = renderer.projectionMatrix * view * cubeModel.modelMatrix;
        // renderer = Renderer(renderer.projectionType, renderer.perpectiveProperties, renderer.projectionMatrix, MVP);

        // int MVPLocation = glGetUniformLocation(shader.ID, "MVP");
        // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
        // for (unsigned int i = 0; i < cubeModel.numMeshes; ++i) {
        //     unsigned int meshVAO = cubeModel.meshes[i].getVertexArrayObjectPointer();

        //     glBindVertexArray(meshVAO);
        //     glDrawElements(GL_TRIANGLES, cubeModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
        //     glBindVertexArray(0);
        // }

        // /*--->*/ renderer.render(cubeModel, camera, shader); /*--->*/
        // Renderer renderer = Renderer(PerpectiveProperties(SCR_WIDTH, SCR_HEIGHT));
        /*--->*/ renderer.render(cubeModel, camera, shader); /*--->*/

        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        // cubeModel.Draw(shader);

        // glm::lookAt();
        // glUseProgram(shaderProgram);
        // glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.ProcessKeyboard(Camera_Movement::PINNED_LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.ProcessKeyboard(Camera_Movement::PINNED_RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.ProcessKeyboard(Camera_Movement::PINNED_UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            camera.ProcessKeyboard(Camera_Movement::PINNED_DOWN, deltaTime);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::PITCH_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::PITCH_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::YAW_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        // std::cout << "Pressed Key" << std::endl;
        camera.ProcessKeyboard(Camera_Movement::YAW_LEFT, deltaTime);
    }
}

// void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//     if (firstMouse) {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = false;
//     }

//     float xoffset = xpos - lastX;
//     float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
//     lastX = xpos;
//     lastY = ypos;

//     camera.ProcessMouseMovement(xoffset, yoffset);
// }

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
    // fov -= (float)yoffset;
    // if (fov < 1.0f)
    //     fov = 1.0f;
    // if (fov > 45.0f)
    //     fov = 45.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void logString(const std::string& s) {
    std::cout << s << std::endl;
}