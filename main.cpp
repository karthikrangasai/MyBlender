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
    // const char* glsl_version = "#version 130";
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
    Scene* scene = renderer.getScene();
    CollisionPhysx physx = CollisionPhysx();
    scene->attachPhysics(&physx);

    Plane groundPlane = Plane("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/plane.obj");
    groundPlane._translation[1] = -5.0f;
    groundPlane._scale[0] = groundPlane._scale[1] = groundPlane._scale[2] = 5.0f;
    groundPlane.updateTransforms();
    groundPlane.Odist = 0.0f;
    groundPlane.meshes[0].material.setDiffuseColor(glm::vec3(0, 0, 1.0f));

    Plane wallOne = Plane("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/plane.obj");
    wallOne._translation[2] = -25.0f;
    wallOne._translation[1] = 12.0f;
    wallOne._scale[0] = wallOne._scale[1] = wallOne._scale[2] = 5.0f;
    wallOne._rotation[0] = 90.0f;
    wallOne.updateTransforms();
    wallOne.Odist = -25.0f;
    wallOne.meshes[0].material.setDiffuseColor(glm::vec3(0, 0, 1.0f));

    Plane wallTwo = Plane("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/plane.obj");
    wallTwo._translation[0] = -25.0f;
    wallTwo._translation[1] = 12.0f;
    wallTwo._scale[0] = wallTwo._scale[1] = wallTwo._scale[2] = 5.0f;
    wallTwo._rotation[2] = 90.0f;
    wallTwo.updateTransforms();
    wallTwo.Odist = -25.0f;
    wallTwo.meshes[0].material.setDiffuseColor(glm::vec3(0, 0, 1.0f));

    Plane wallThree = Plane("/home/karthikrangasai/Documents/Acads/4th Year/4 - 2/IS F311 Comp Graphics/assignment/assignment_2/problem_statement/plane.obj");
    wallThree._translation[0] = 25.0f;
    wallThree._translation[1] = 12.0f;
    wallThree._scale[0] = wallThree._scale[1] = wallThree._scale[2] = 5.0f;
    wallThree._rotation[2] = -90.0f;
    wallThree.updateTransforms();
    wallThree.Odist = 25.0f;
    wallThree.meshes[0].material.setDiffuseColor(glm::vec3(0, 0, 1.0f));

    Sphere s1 = Sphere(1, 30);
    s1._translation[1] = 15.0f;
    s1.updateTransforms();
    s1.meshes[0].material.setDiffuseColor(glm::vec3(1.0f, 0, 0));
    // Sphere s2 = Sphere(2, 30);
    // s2._translation[0] = -20.0f;
    // s2.updateTransforms();

    scene->addModel(&groundPlane);
    scene->addModel(&wallOne);
    scene->addModel(&wallTwo);
    scene->addModel(&wallThree);
    scene->addModel(&s1);
    // scene->addModel(&s2);

    PhysxObject gPlane = PhysxObject(PhysxShape::PLANE, &groundPlane, 2.0f, glm::vec3(0, 0, 0));
    PhysxObject gWallOne = PhysxObject(PhysxShape::PLANE, &wallOne, 2.0f, glm::vec3(0, 0, 0));
    PhysxObject gwallTwo = PhysxObject(PhysxShape::PLANE, &wallTwo, 2.0f, glm::vec3(0, 0, 0));
    PhysxObject gwallThree = PhysxObject(PhysxShape::PLANE, &wallThree, 2.0f, glm::vec3(0, 0, 0));
    PhysxObject ps1 = PhysxObject(PhysxShape::SPHERE, &s1, 1.0f, glm::vec3(-1, 0, 0));
    ps1.enableGravity();
    // PhysxObject ps2 = PhysxObject(PhysxShape::SPHERE, &s2, 2, glm::vec3(20, 0, 0));

    physx.addObject(&gPlane);
    physx.addObject(&gWallOne);
    physx.addObject(&gwallTwo);
    physx.addObject(&gwallThree);
    physx.addObject(&ps1);
    // physx.addObject(&ps2);
    scene->isPhysicsOn = true;

    // // ImGui Setup
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // (void)io;
    // ImGui::StyleColorsDark();
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(renderer.shader.ID);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, renderer.camera);

        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();

        // if (ImGui::Begin("Settings")) {
        //     {
        //         ImGui::BeginChild("Light Properties", ImVec2(0, 150), true);
        //         ImGui::SliderFloat3("Position", renderer.scene.light._position, -60.0f, 60.0f);
        //         // ImGui::ColorPicker3("Ambient Color", renderer.scene.light._ambient, 0);
        //         // ImGui::ColorPicker3("Diffuse Color", renderer.scene.light._diffuse, 0);
        //         // ImGui::ColorPicker3("Specular Color", renderer.scene.light._specular, 0);
        //         ImGui::EndChild();
        //         renderer.scene.light.updateLighting();
        //     }

        //     for (unsigned int i = 0; i < renderer.scene.models.size(); ++i) {
        //         char v[] = "Model 10 Visibility";
        //         snprintf(v, (5 + 1 + 2 + 1 + 10 + 1), "Model %d Visibility", (i + 1));
        //         ImGui::Checkbox(v, &(renderer.scene.models[i].visibility));
        //     }

        //     ImGui::Separator();

        //     static int modelNumber = 0;
        //     for (unsigned int i = 0; i < renderer.scene.models.size(); ++i) {
        //         char v[] = "Model 10";
        //         snprintf(v, (5 + 1 + 2), "Model %d", (i + 1));
        //         ImGui::RadioButton(v, &modelNumber, i);
        //         if ((i + 1) != renderer.scene.models.size()) {
        //             ImGui::SameLine();
        //         }
        //     }

        //     ImGui::Separator();

        //     {
        //         ImGui::BeginChild("Obj Properties Child", ImVec2(0, 100), true);
        //         ImGui::Text("Object Transfromation Properties");
        //         ImGui::SliderFloat3("Translation", renderer.scene.models[modelNumber]._translation, -30.0f, 30.0f);
        //         ImGui::SliderFloat3("Rotation", renderer.scene.models[modelNumber]._rotation, 0.0f, 360.0f);
        //         ImGui::SliderFloat3("Scale", renderer.scene.models[modelNumber]._scale, 1.0f, 10.0f);

        //         ImGui::EndChild();
        //         renderer.scene.models[modelNumber].updateTransforms();
        //     }

        //     ImGui::Separator();

        //     {
        //         ImGui::BeginChild("Camera Properties Child", ImVec2(0, 100), true);
        //         ImGui::Text("Camera Properties");
        //         static float speed = 2.5f;
        //         static float sensitivity = 0.05f;
        //         ImGui::SliderFloat("Camera Speed", &speed, 1.0f, 5.0f);
        //         ImGui::SliderFloat("Camera Sensitivity", &sensitivity, 0.01f, 5.0f);
        //         renderer.camera.updateCameraSpeed(speed);
        //         renderer.camera.updateCameraSensitivity(sensitivity);
        //         ImGui::EndChild();
        //     }

        //     ImGui::Separator();

        //     if (ImGui::Button("Reset Camera")) {
        //         for (unsigned int i = 0; i < renderer.scene.models.size(); ++i) {
        //             renderer.scene.models[i].reset();
        //         }
        //         renderer.camera.reset();
        //     }

        //     ImGui::Separator();

        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //     ImGui::End();
        // }

        renderer.renderAll();
        // ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

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