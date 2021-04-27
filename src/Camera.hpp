/** @file Camera.cpp
 *  @brief Class definition for Camera.
 *
 *  @author Sivaraman Karthik Rangasai (karthikrangasai)
 *  @author G Sathyaram (wreck-count)
 */

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

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
/** @class Camera
 *  @brief Data class for a Camera object.
 *  @details This class containes all the properties for a given Camera. 
 *  This class also handles the functionality of the View Matrix used 
 *  during the display of the objects in the scene. Camera transformations 
 *  in the world space is also possible via this class.
 */
class Camera {
   public:
    //! @brief Camera Position in the world.
    glm::vec3 Position;

    //! @brief Camera Direction to point wrt to camera centre on a unit sphere.
    glm::vec3 Front;

    //! @brief Camera Up to define the camera axis.
    glm::vec3 Up;

    //! @brief Camera Right to define the camera axis.
    glm::vec3 Right;

    //! @brief Y-axis of the world.
    glm::vec3 WorldUp;

    //! @brief Where the Camera should `lookAt` in the world. Computed as Center = Position + Front
    glm::vec3 Center;

    //! @brief World origin = (0,0,0)
    glm::vec3 Origin;

    //! @brief View matrix as defined and required for the display of model in the scene by the MVP method.
    glm::mat4 viewMatrix;

    //! @brief To keep the Yaw, Pitch actions constant during all time, the distance from the camera view center is computed and used to scale the front vector.
    float distanceFromCenter;

    //! @brief For pinned and non pinned movement.
    Camera_Movement_State State;

    //! @brief Camera's Yaw angle.
    float Yaw;

    //! @brief Camera's Pitch angle.
    float Pitch;

    //! @brief Camera's Roll angle.
    float Roll;

    //! @brief Camera's movement speed for translation controls.
    float MovementSpeed;

    //! @brief Camera's movement speed for angualar controls.
    float MouseSensitivity;

    /**
	 * @brief Default Constructor.
	 * @details The constructor initializes the camera at a certain location and the camera center is initalized to World Origin.
	 * Hence the camera points to the World Origin. The front vector is calculated accordingly using the Center and Position of the Camera.
	 * Camera's Up vector is initialzed with World Up which is initialized to Y-axis.
	 * The camers starts in the NON_PINNED state.
	*/
    Camera() : Origin(0.0f, 0.0f, 0.0f) {
        this->Position = glm::vec3(0.0f, 20.0f, 20.0f);
        this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Center = glm::vec3(0.0f, 0.0f, 0.0f);

        this->Front = this->Center - this->Position;
        this->distanceFromCenter = glm::distance(this->Position, this->Center);                                            // r = sqrt(x^2 + y^2 + z^2)
        this->Yaw = glm::atan(this->Front.z, this->Front.x);                                                               // atan(z/x)
        this->Pitch = glm::atan(this->Front.y, glm::sqrt(glm::pow(this->Front.x, 2.0f) + glm::pow(this->Front.z, 2.0f)));  // atan(y / sqrt(x^2 + z^2))
        this->Roll = 0.0f;

        this->MovementSpeed = SPEED;
        this->MouseSensitivity = SENSITIVITY;
        // this->Zoom = ZOOM;

        this->State = NON_PINNED;
        updateCameraVectors();
    }

    /** @brief getViewMatrix - Returns the view matrix.
 	 * @details Returns the view matrix calculated using Camera's Euler Angles and the lookAt Matrix.
 	 *
 	 * @return A 4x4 3D Transformation matrix.
 	*/
    glm::mat4 getViewMatrix() const {
        return this->viewMatrix;
    }

    /**
	 * @brief Get the camera's position
	 * 
	 * @return glm::vec3 
	 */
    glm::vec3 getPosition() const {
        return this->Position;
    }

    /** @brief processKeyboard - Transform camera based on keyboard input.
	 * @details The camera controls are mentioned below:
	 * W,S : Zoom In and Zoom Out
	 * A,D : Move Left and Move Right
	 * Q,Z : Move Up and Move Down
	 * 
	 * LEFT, RIGHT : Rotate about Center of view from left and right respectively
	 * UP, DOWN : Rotate about Center of view from up and down respectively
	 * 
	 * I,K : Pitch Up and Down
	 * J,L : Yaw left and right
	 * O,U : Roll right and left respectively.
	 * 
	 * @return void
	*/
    void processKeyboard(Camera_Movement direction, float deltaTime) {
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
            Roll += this->MouseSensitivity;
            this->updateCenterVectorForRoll(this->MouseSensitivity, Front);
        }
        if (direction == ROLL_LEFT) {
            Roll -= this->MouseSensitivity;
            this->updateCenterVectorForRoll(-this->MouseSensitivity, Front);
        }

        this->updateCameraVectors();
    }

    /** @brief updateCameraSpeed - Updates the Camera Speed based on GUI input.
	 * @return void
	*/
    void updateCameraSpeed(float speed) {
        this->MovementSpeed = speed;
    }

    /** @brief updateCameraSensitivity - Updates the Camera Euler Angle update Sensitivity based on GUI input.
	 * @return void
	*/
    void updateCameraSensitivity(float sensitivity) {
        this->MouseSensitivity = sensitivity;
    }

    /** @brief reset - Reset all the Model properties.
	 * @details Resets all the properties of the camera i.e. code in the constructor again.
	 * 
	 * @return void
	*/
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

#ifdef __cplusplus
}
#endif
#endif