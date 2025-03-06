#pragma once

// OpenCV (does not depend on GL)
//#include <opencv2\opencv.hpp>

// include anywhere, in any order
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <vector>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 
#include <GL/gl.h>

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW __MUST__ be first.
#include <GLFW/glfw3.h>

// OpenGL math (and other additional GL libraries, at the end)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// User includes
#include "getinfo.hpp"
#include "gl_err_callback.h"            // Added for GL debug output
#include "glerror.h"
#include "assets.hpp"

class App {
public:
    App();
    bool init(void);
    int run(void);
    ~App();

private:
    GLFWwindow* window;
    static bool vsync_on;

    GLuint shader_prog_ID{ 0 };
    GLuint VBO_ID{ 0 };
    GLuint VAO_ID{ 0 };

    GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };

    std::vector<vertex> triangle_vertices =
    {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    //float camera_x = 0.0f, camera_y = 0.0f; // Pro WASD pohyb
};
