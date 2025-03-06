#include "app.hpp"

bool App::vsync_on = true;


App::App() : window(nullptr), shader_prog_ID(0), VBO_ID(0), VAO_ID(0), r(1.0f), g(0.0f), b(0.0f), a(1.0f) {
    std::cout << "Constructed...\n";
}

bool App::init() {
    try {
        glfwSetErrorCallback(error_callback);
        if (!glfwInit()) throw std::runtime_error("GLFW initialization failed!");

        window = glfwCreateWindow(800, 600, "OpenGL Context", nullptr, nullptr);
        if (!window) throw std::runtime_error("Failed to create GLFW window!");

        glfwMakeContextCurrent(window);
        glewInit();
        wglewInit();

        printOpenGLInfo();

        std::cout << "Initialized...\n";

        glfwSetKeyCallback(window, key_callback);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, nullptr);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glfwSwapInterval(vsync_on ? 1 : 0);

        glGenVertexArrays(1, &VAO_ID);
        glGenBuffers(1, &VBO_ID);

        glBindVertexArray(VAO_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        glBufferData(GL_ARRAY_BUFFER, triangle_vertices.size() * sizeof(vertex), triangle_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
        glEnableVertexAttribArray(0);

        const char* vertex_shader_src = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main() {\n"
            "gl_Position = vec4(aPos, 1.0);\n"
            "}";

        const char* fragment_shader_src = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 color;\n"
            "void main() {\n"
            "FragColor = vec4(color, 1.0);\n"
            "}";

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
        glCompileShader(fragment_shader);

        shader_prog_ID = glCreateProgram();
        glAttachShader(shader_prog_ID, vertex_shader);
        glAttachShader(shader_prog_ID, fragment_shader);
        glLinkProgram(shader_prog_ID);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    catch (const std::exception& e) {
        std::cerr << "Init failed: " << e.what() << std::endl;
        return false;
    }
    return true;
}

int App::run() {
    GLfloat r, g, b, a;
    r = g = b = a = 1.0f;
    std::cout << "Running...\n";
    auto startTime = std::chrono::steady_clock::now();
    auto lastTime = std::chrono::steady_clock::now();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        gl_check_error();
        r = App::r; g = App::g; b = App::b; a = App::a;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;
        float time = elapsed.count();
        //r = (sin(time) + 1.0f) / 2.0f;

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_prog_ID);
        GLuint colorLocation = glGetUniformLocation(shader_prog_ID, "color");
        glUniform3f(colorLocation, r, g, b);

        glBindVertexArray(VAO_ID);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;
        std::chrono::duration<double> fpsElapsed = currentTime - lastTime;
        if (fpsElapsed.count() >= 1.0) {
            double fps = frameCount / fpsElapsed.count();
            std::string title = "OpenGL context - FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


App::~App() {
    glDeleteProgram(shader_prog_ID);
    glDeleteBuffers(1, &VBO_ID);
    glDeleteVertexArrays(1, &VAO_ID);
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "Bye...\n";
}

// -------------------- CALLBACKY --------------------

void App::error_callback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_F12:
                vsync_on = !vsync_on;
                glfwSwapInterval(vsync_on ? 1 : 0);
                std::cout << "VSync " << (vsync_on ? "ON" : "OFF") << "\n";
                break;
            case GLFW_KEY_R:
                app->r = (app->r == 1.0f) ? 0.0f : 1.0f;
                break;
            case GLFW_KEY_G:
                app->g = (app->g == 1.0f) ? 0.0f : 1.0f;
                break;
            case GLFW_KEY_B:
                app->b = (app->b == 1.0f) ? 0.0f : 1.0f;
                break;
            default:
                break;
            }
        }
    }

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            std::cout << "Left mouse button pressed\n";
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            std::cout << "Right mouse button pressed\n";
            break;
        default:
            break;
        }
    }
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0.0)
        std::cout << "Mouse wheel up...\n";
    else if (yoffset < 0.0)
        std::cout << "Mouse wheel down...\n";
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    std::cout << "Cursor position: (" << xpos << ", " << ypos << ")\n";
}
