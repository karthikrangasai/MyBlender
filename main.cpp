#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

// #include <glad/glad.h>
#include <GL/glew.h>
#include "vendor/GLFW/glfw3.h"

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

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

bool firstMouse = true;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

Camera camera = Camera();
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync
                          // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
                          // glfwSetCursorPosCallback(window, mouse_callback);
                          // glfwSetScrollCallback(window, scroll_callback);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // Problem: glewInit failed, something is seriously wrong.
        cout << "glewInit failed: " << glewGetErrorString(err) << endl;
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    vector<Model> models;
    // models.reserve(2);
    models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/cube.obj"));
    models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/sphere.obj"));
    models.push_back(Model("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/teapot.obj"));
    Shader shader = Shader();
    Renderer renderer = Renderer(PerpectiveProperties(SCR_WIDTH, SCR_HEIGHT));

    // ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(shader.ID);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Settings")) {
            for (unsigned int i = 0; i < models.size(); ++i) {
                char v[] = "Model 10 Visibility";
                snprintf(v, (5 + 1 + 2 + 1 + 10 + 1), "Model %d Visibility", (i + 1));
                ImGui::Checkbox(v, &(models[i].visibility));

                // ImGui::SameLine();

                // char c[] = "Model 10 Control";
                // snprintf(c, (5 + 1 + 2 + 1 + 7 + 1), "Model %d Control", (i + 1));
                // ImGui::Checkbox(c, &(models[i].control));
            }

            ImGui::Separator();

            static int modelNumber = 0;
            for (unsigned int i = 0; i < models.size(); ++i) {
                char v[] = "Model 10";
                snprintf(v, (5 + 1 + 2), "Model %d", (i + 1));
                ImGui::RadioButton(v, &modelNumber, i);
                if ((i + 1) != models.size()) {
                    ImGui::SameLine();
                }
            }

            ImGui::Separator();

            if (ImGui::BeginChild("Obj Properties Child", ImVec2(0, 100), true)) {
                // static float translation[] = {0.0f, 0.0f, 0.0f};
                // static float rotation[] = {0.0f, 0.0f, 0.0f};
                // static float scale[] = {1.0f, 1.0f, 1.0f};

                // for (unsigned int i = 0; i < models.size(); ++i) {
                //     models[i].updateGlobalTransforms(translation, rotation, scale);
                // }

                ImGui::Text("Object Transfromation Properties");
                ImGui::SliderFloat3("Translation", models[modelNumber]._translation, -30.0f, 30.0f);
                ImGui::SliderFloat3("Rotation", models[modelNumber]._rotation, 0.0f, 360.0f);
                ImGui::SliderFloat3("Scale", models[modelNumber]._scale, 1.0f, 10.0f);

                // for (unsigned int i = 0; i < models.size(); ++i) {
                //     models[i].updateTransforms(translation, rotation, scale);
                // }
                ImGui::EndChild();
                models[modelNumber].updateTransforms();
            }

            ImGui::Separator();

            if (ImGui::BeginChild("Camera Properties Child", ImVec2(0, 100), true)) {
                ImGui::Text("Camera Properties");
                static float speed = 2.5f;
                static float sensitivity = 0.05f;
                ImGui::SliderFloat("Camera Speed", &speed, 1.0f, 5.0f);
                ImGui::SliderFloat("Camera Sensitivity", &sensitivity, 0.01f, 5.0f);
                camera.updateCameraSpeed(speed);
                camera.updateCameraSensitivity(sensitivity);
                ImGui::EndChild();
            }

            ImGui::Separator();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        /* RENDER CALL */
        for (const Model& m : models) {
            if (m.visibility) {
                renderer.render(m, camera, shader);
            }
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/*
	W,S : Zoom In and Zoom Out
	A,D : Move Left and Move Right
	Q,Z : Move Up and Move Down

	LEFT, RIGHT : Rotate about Center of view from left and right respectively
	UP, DOWN : Rotate about Center of view from up and down respectively

	I,K : Pitch Up and Down
	J,L : Yaw left and right
	O,U : Roll right and left respectively.
*/
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PINNED_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PINNED_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PINNED_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PINNED_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PITCH_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::PITCH_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::YAW_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::YAW_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::ROLL_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::ROLL_LEFT, deltaTime);
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