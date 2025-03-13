#include "app.hpp"

bool App::vsync_on = true;


App::App() : window(nullptr), shader(nullptr), model(nullptr) {
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
        glfwSwapInterval(vsync_on ? 1 : 0);

        // ✨ Načti model z .OBJ souboru
        shader = new ShaderProgram("shader.vert", "shader.frag");
        std::ifstream testFile("triangle.obj");
        if (!testFile) {
            std::cerr << "Soubor triangle.obj nebyl nalezen!\n";
        }
        else {
            std::cout << "Soubor triangle.obj nalezen.\n";
        }
        model = new Model("triangle.obj", shader);

    }
    catch (const std::exception& e) {
        std::cerr << "Init failed: " << e.what() << std::endl;
        return false;
    }
    return true;
}


int App::run() {
    std::cout << "Running...\n";
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        shader->activate();
        shader->setUniform("color", glm::vec3(r, g, b));

        // ✨ Vykresli model ze souboru
        model->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


App::~App() {
    delete shader;
    delete model; // ✨ Uvolnit model!
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
                std::cout << "Key R pressed" << std::endl;
                break;
            case GLFW_KEY_G:
                app->g = (app->g == 1.0f) ? 0.0f : 1.0f;
                std::cout << "Key G pressed" << std::endl;
                break;
            case GLFW_KEY_B:
                app->b = (app->b == 1.0f) ? 0.0f : 1.0f;
                std::cout << "Key B pressed" << std::endl;
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
