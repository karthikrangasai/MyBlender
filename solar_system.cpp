#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "src/Model.hpp"
#include "src/Camera.hpp"
#include "src/Shader.hpp"
#include "src/Renderer.hpp"
#include "src/Physics.hpp"

void logString(const std::string& s);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Camera& camera);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

bool firstMouse = true;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MoonShot", NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "glewInit failed: " << glewGetErrorString(err) << endl;
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    Renderer renderer = Renderer(PerpectiveProperties(SCR_WIDTH, SCR_HEIGHT));
    Scene* universe = renderer.getScene();
    SolarSystemPhysx ssp = SolarSystemPhysx();
    // SETUP SCENE //
    Sphere sun = Sphere(5, 30);
    sun.meshes[0].material.setDiffuseColor(glm::vec3(0.937f, 0.557f, 0.22f));
    PhysxObject sunPhysics = PhysxObject(PhysxShape::SPHERE, &sun, 100, glm::vec3(0.0f));

    Sphere mercury = Sphere(1, 20);
    mercury._translation[0] = 5;
    mercury.updateTransforms();
    mercury.meshes[0].material.setDiffuseColor(glm::vec3(1.0f, 0.898f, 0.706f));
    PhysxObject mercuryPhysics = PhysxObject(PhysxShape::SPHERE, &mercury, 8, glm::vec3(0, glm::sqrt(100 / 5), 0));

    Sphere venus = Sphere(2, 20);
    venus._translation[0] = 15;
    venus.updateTransforms();
    venus.meshes[0].material.setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
    PhysxObject venusPhysics = PhysxObject(PhysxShape::SPHERE, &venus, 12, glm::vec3(0, 0, glm::sqrt(100 / 15)));

    Sphere earth = Sphere(2, 20);
    earth._translation[0] = 25;
    earth.updateTransforms();
    earth.meshes[0].material.setDiffuseColor(glm::vec3(0.0f, 0.9f, 0.9f));
    PhysxObject earthPhysics = PhysxObject(PhysxShape::SPHERE, &earth, 16, glm::vec3(0, glm::sqrt(100 / 25), 0));

    Sphere mars = Sphere(1.5, 20);
    mars._translation[0] = 30;
    mars.updateTransforms();
    mars.meshes[0].material.setDiffuseColor(glm::vec3(0.5765f, 0.2824f, 0.2196f));
    PhysxObject marsPhysics = PhysxObject(PhysxShape::SPHERE, &mars, 6, glm::vec3(0, 0, glm::sqrt(100 / 30)));

    universe->addModel(&sun);
    universe->addModel(&mercury);
    universe->addModel(&venus);
    universe->addModel(&earth);
    universe->addModel(&mars);

    ssp.addObject(&sunPhysics);
    ssp.addObject(&mercuryPhysics);
    ssp.addObject(&venusPhysics);
    ssp.addObject(&earthPhysics);
    ssp.addObject(&marsPhysics);

    universe->attachPhysics(&ssp);
    universe->isPhysicsOn = true;

    // ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(renderer.shader.ID);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, renderer.camera);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Settings")) {
            {
                ImGui::BeginChild("Camera Properties Child", ImVec2(0, 100), true);
                ImGui::Text("Camera Properties");
                static float speed = 2.5f;
                static float sensitivity = 0.05f;
                ImGui::SliderFloat("Camera Speed", &speed, 1.0f, 5.0f);
                ImGui::SliderFloat("Camera Sensitivity", &sensitivity, 0.01f, 5.0f);
                renderer.camera.updateCameraSpeed(speed);
                renderer.camera.updateCameraSensitivity(sensitivity);
                ImGui::EndChild();
            }
            ImGui::Separator();

            if (ImGui::Button("Toggle Physics")) {
                renderer.scene.isPhysicsOn = !renderer.scene.isPhysicsOn;
            }

            ImGui::Separator();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        renderer.renderAll();
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

void processInput(GLFWwindow* window, Camera& camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PINNED_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PINNED_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PINNED_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PINNED_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PITCH_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::PITCH_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::YAW_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::YAW_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::ROLL_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        camera.processKeyboard(Camera_Movement::ROLL_LEFT, deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void logString(const std::string& s) {
    std::cout << s << std::endl;
}