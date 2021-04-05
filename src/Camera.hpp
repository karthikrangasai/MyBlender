#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

/** Camera Class Notes:
 * 	Position - Camera position in the world
 * 	Direction - where to look at (shouldn't be (0,0,0)) [Yaw and Pitch Decide the direction] (aka Front)
 * 	Up - Always by (0,1,0) [Can be changed by Roll I think, not sure]
 * 
 * 	Yaw = theta: XZ plane (Angle made by vector with X-axis)
 * 	- x = cos(yaw)
 * 	- z = sin(yaw)
 * 
 * 	Pitch = phi: YZ plane (Angle made by vector with XZ-plane)
 * 	- y = sin(pitch)
 * 	- z = cos(pitch)
 * 
 * 	Total Pitch and Yaw is in 3 axes: Thus spherical coordinates with r = 1 is
 * 	- x = cos(yaw) * cos(pitch)
 * 	- y = sin(pitch)
 * 	- z = sin(yaw) * cos(pitch)
 * 
 * 	Starting position for Camera:
 * 	- Position: (5,5,5)
 * 	- Front: ()
*/

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PINNED_LEFT,
    PINNED_RIGHT,
    PINNED_UP,
    PINNED_DOWN,
    PITCH_UP,
    PITCH_DOWN,
    YAW_RIGHT,
    YAW_LEFT,
    ROLL_RIGHT,
    ROLL_LEFT
};

enum Camera_Movement_State {
    NON_PINNED,
    PINNED
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
   public:
    // camera Attributes
    glm::vec3 Position;  // Camera Position in the world.
    glm::vec3 Front;     // Camera Direction to point wrt to camera centre on a unit sphere.
    glm::vec3 Up;        // Camera Up to define the camera axis.
    glm::vec3 Right;     // Camera Right to define the camera axis.
    glm::vec3 WorldUp;   // Y-axis of the world.
    glm::vec3 Center;    // Where the Camera should `lookAt` in the world. Computed as Center = Position + Front
    glm::vec3 Origin;    // World origin
    glm::mat4 viewMatrix;
    float distanceFromCenter;
    Camera_Movement_State State;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera() : Origin(0.0f, 0.0f, 0.0f) {
        this->Position = glm::vec3(7.0f, 3.0f, 0.0f);
        this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Center = glm::vec3(0.0f, 0.0f, 0.0f);

        this->Front = this->Center - this->Position;
        this->distanceFromCenter = glm::distance(this->Position, this->Center);                                            // r = sqrt(x^2 + y^2 + z^2)
        this->Yaw = glm::atan(this->Front.z, this->Front.x);                                                               // atan(z/x)
        this->Pitch = glm::atan(this->Front.y, glm::sqrt(glm::pow(this->Front.x, 2.0f) + glm::pow(this->Front.z, 2.0f)));  // atan(y / sqrt(x^2 + z^2))

        this->MovementSpeed = SPEED;
        this->MouseSensitivity = SENSITIVITY;
        this->Zoom = ZOOM;

        this->State = NON_PINNED;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const {
        return this->viewMatrix;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            Position += Front * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }
        if (direction == BACKWARD) {
            Position -= Front * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }
        if (direction == LEFT) {
            Position -= Right * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }
        if (direction == RIGHT) {
            Position += Right * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }
        if (direction == UP) {
            Position += Up * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }
        if (direction == DOWN) {
            Position -= Up * velocity;
            this->Center = this->Position + this->Front;
            this->State = NON_PINNED;
        }

        if (direction == PINNED_LEFT) {
            Position -= Right * velocity;
            this->updateFrontVector();
            this->State = PINNED;
        }
        if (direction == PINNED_RIGHT) {
            Position += Right * velocity;
            this->updateFrontVector();
            this->State = PINNED;
        }
        if (direction == PINNED_UP) {
            Position += Up * velocity;
            this->updateFrontVector();
            this->State = PINNED;
        }
        if (direction == PINNED_DOWN) {
            Position -= Up * velocity;
            this->updateFrontVector();
            this->State = PINNED;
        }

        if (direction == PITCH_UP) {
            Pitch += this->MouseSensitivity;
            this->updateCenterVector(this->MouseSensitivity, Right);
        }
        if (direction == PITCH_DOWN) {
            Pitch -= this->MouseSensitivity;
            this->updateCenterVector(-this->MouseSensitivity, Right);
        }
        if (direction == YAW_RIGHT) {
            Yaw += this->MouseSensitivity;
            this->updateCenterVector(this->MouseSensitivity, Up);
        }
        if (direction == YAW_LEFT) {
            Yaw -= this->MouseSensitivity;
            this->updateCenterVector(-this->MouseSensitivity, Up);
        }
        if (direction == ROLL_RIGHT) {
            Yaw += this->MouseSensitivity;
            this->updateCenterVectorForRoll(this->MouseSensitivity, Front);
        }
        if (direction == ROLL_LEFT) {
            Yaw -= this->MouseSensitivity;
            this->updateCenterVectorForRoll(-this->MouseSensitivity, Front);
        }

        // this->updateFrontVector();
        this->updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    float getZoom() {
        return this->Zoom;
    }

    void updateCameraSpeed(float speed) {
        this->MovementSpeed = speed;
    }

    void updateCameraSensitivity(float sensitivity) {
        this->MouseSensitivity = sensitivity;
    }

    void reset() {
        this->Position = glm::vec3(7.0f, 3.0f, 0.0f);
        this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Center = glm::vec3(0.0f, 0.0f, 0.0f);

        this->Front = this->Center - this->Position;
        this->distanceFromCenter = glm::distance(this->Position, this->Center);                                            // r = sqrt(x^2 + y^2 + z^2)
        this->Yaw = glm::atan(this->Front.z, this->Front.x);                                                               // atan(z/x)
        this->Pitch = glm::atan(this->Front.y, glm::sqrt(glm::pow(this->Front.x, 2.0f) + glm::pow(this->Front.z, 2.0f)));  // atan(y / sqrt(x^2 + z^2))

        this->MovementSpeed = SPEED;
        this->MouseSensitivity = SENSITIVITY;
        this->Zoom = ZOOM;

        this->State = NON_PINNED;
        updateCameraVectors();
    }

   private:
    void updateFrontVector() {
        this->Front = this->Center - this->Position;
        this->distanceFromCenter = glm::distance(this->Position, this->Center);                                            // r = sqrt(x^2 + y^2 + z^2)
        this->Yaw = glm::atan(this->Front.z, this->Front.x);                                                               // atan(z/x)
        this->Pitch = glm::atan(this->Front.y, glm::sqrt(glm::pow(this->Front.x, 2.0f) + glm::pow(this->Front.z, 2.0f)));  // atan(y / sqrt(x^2 + z^2))
    }

    void updateCenterVector(float offsetAngle, glm::vec3 axis) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(offsetAngle), axis);
        Front = glm::vec3(rotation * glm::vec4(Front, 1.0));

        this->Center = this->Position + this->Front;
        this->distanceFromCenter = glm::distance(this->Front, this->Center);
    }

    void updateCenterVectorForRoll(float offsetAngle, glm::vec3 axis) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(offsetAngle), axis);
        Right = glm::vec3(rotation * glm::vec4(Right, 1.0));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        if (this->State == NON_PINNED) {
            this->viewMatrix = glm::lookAt(Position, Position + Front, Up);
        } else if (this->State == PINNED) {
            this->viewMatrix = glm::lookAt(Position, Center, Up);
        }
    }
};

// // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
// class Camera {
//    public:
//     // camera Attributes
//     glm::vec3 Position;  // Camera Position in the world.
//     glm::vec3 Front;     // Camera Direction to point wrt to camera centre on a unit sphere.
//     glm::vec3 Up;        // Camera Up to define the camera axis.
//     glm::vec3 Right;     // Camera Right to define the camera axis.
//     glm::vec3 WorldUp;   // Y-axis of the world.
//     glm::vec3 Center;    // Where the Camera should `lookAt` in the world. Computed as Center = Position + Front
//     glm::vec3 Origin;    // World origin
//     float distanceFromOrigin;
//     // euler Angles
//     float Yaw;
//     float Pitch;
//     // camera options
//     float MovementSpeed;
//     float MouseSensitivity;
//     float Zoom;

//     // constructor with vectors
//     Camera() : Origin(0.0f, 0.0f, 0.0f) {
//         this->Position = glm::vec3(5.0f, 5.0f, 5.0f);
//         this->distanceFromOrigin = glm::distance(Position, Origin);

//         this->Center = glm::vec3(0.0f, 0.0f, 0.0f);
//         this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

//         updateCameraVectors();
//     }

//     Camera(glm::vec3 position = glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(-5.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
//         Position = position;
//         WorldUp = up;
//         Yaw = yaw;
//         Pitch = pitch;
//         updateCameraVectors();
//     }
//     // constructor with scalar values
//     Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
//         Position = glm::vec3(posX, posY, posZ);
//         WorldUp = glm::vec3(upX, upY, upZ);
//         Yaw = yaw;
//         Pitch = pitch;
//         updateCameraVectors();
//     }

//     // returns the view matrix calculated using Euler Angles and the LookAt Matrix
//     glm::mat4 GetViewMatrix() const {
//         return glm::lookAt(Position, Position + Front, Up);
//         // return glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f), Up);
//     }

//     // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//     void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
//         float velocity = MovementSpeed * deltaTime;
//         if (direction == FORWARD)
//             Position += Front * velocity;
//         if (direction == BACKWARD)
//             Position -= Front * velocity;
//         if (direction == LEFT)
//             Position -= Right * velocity;
//         if (direction == RIGHT)
//             Position += Right * velocity;
//         if (direction == UP)
//             Position += WorldUp * velocity;
//         if (direction == DOWN)
//             Position -= WorldUp * velocity;
//     }

//     // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//     void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
//         xoffset *= MouseSensitivity;
//         yoffset *= MouseSensitivity;

//         Yaw += xoffset;
//         Pitch += yoffset;

//         // make sure that when pitch is out of bounds, screen doesn't get flipped
//         if (constrainPitch) {
//             if (Pitch > 89.0f)
//                 Pitch = 89.0f;
//             if (Pitch < -89.0f)
//                 Pitch = -89.0f;
//         }

//         // update Front, Right and Up Vectors using the updated Euler angles
//         updateCameraVectors();
//     }

//     // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//     void ProcessMouseScroll(float yoffset) {
//         Zoom -= (float)yoffset;
//         if (Zoom < 1.0f)
//             Zoom = 1.0f;
//         if (Zoom > 45.0f)
//             Zoom = 45.0f;
//     }

//     float getZoom() {
//         return this->Zoom;
//     }

//    private:
//     // calculates the front vector from the Camera's (updated) Euler Angles
//     void updateCameraVectors() {
//         // calculate the new Front vector
//         // glm::vec3 front;
//         // front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//         // front.y = sin(glm::radians(Pitch));
//         // front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//         // Front = glm::normalize(front);
//         // also re-calculate the Right and Up vector
//         Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//         Up = glm::normalize(glm::cross(Right, Front));
//     }
// };

#ifdef __cplusplus
}
#endif
#endif