#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Atributy kamery
    glm::vec3 Position{ 0.0f, 0.0f, 3.0f };
    glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 Up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 Right{ 1.0f, 0.0f, 0.0f };

    // Euler angles
    float Yaw{ -90.0f };  // oto�en� kolem Y osy (vlevo/vpravo)
    float Pitch{ 0.0f };  // oto�en� kolem X osy (nahoru/dol�)
    float Roll{ 0.0f };   // oto�en� kolem Z osy (n�klon)

    // Parametry kamery
    float MovementSpeed{ 2.5f };
    float MouseSensitivity{ 0.1f };

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f)) : Position(position) {
        updateCameraVectors();
    }

    // Z�sk�n� View matice
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Zpracov�n� vstupu z kl�vesnice
    glm::vec3 ProcessKeyboard(GLFWwindow* window, float deltaTime) {
        glm::vec3 direction{ 0.0f };

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction -= Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction -= Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += Right;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            direction += Up;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            direction -= Up;

        // Normalizace vektoru sm�ru, pokud nen� nulov�
        if (glm::length(direction) > 0.0f)
            direction = glm::normalize(direction);

        // Kontrola, zda je Shift stisknut�, pro rychlej�� pohyb
        float speed = MovementSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed *= 2.5f;  // 2.5x rychlej�� se Shiftem

        return direction * speed * deltaTime;
    }

    // Pohyb kamery
    void Move(const glm::vec3& offset) {
        Position += offset;
    }

    // Zpracov�n� pohybu my�i
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Omezen� maxim�ln�ho oto�en� nahoru/dol�
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Aktualizace vektor� kamery
        updateCameraVectors();
    }

private:
    // V�po�et vektor� kamery z Euler �hl�
    void updateCameraVectors() {
        // V�po�et forward vektoru
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // V�po�et right a up vektor�
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};