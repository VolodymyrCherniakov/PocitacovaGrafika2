#include "app.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>
#include <algorithm>

// Vertex shader for the simple triangle
const char* vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

// Fragment shader for the simple triangle
const char* fragmentShaderSource = R"(
    #version 460 core
    out vec4 FragColor;
    uniform vec3 triangleColor;
    void main() {
        FragColor = vec4(triangleColor, 1.0f);
    }
)";

App::App() : lastX(400.0), lastY(300.0), firstMouse(true), fov(DEFAULT_FOV) {
    camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));
}

App::~App() {
    shader.clear();
    if (triangle) {
        delete triangle;
        triangle = nullptr;
    }
    for (auto& wall : maze_walls) {
        delete wall;
    }
    maze_walls.clear();
    for (auto& bunny : transparent_bunnies) {
        delete bunny;
    }
    transparent_bunnies.clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

bool App::init(GLFWwindow* win) {
    window = win;
    fov = DEFAULT_FOV;

    if (!GLEW_ARB_direct_state_access) {
        std::cerr << "No DSA :-(" << std::endl;
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, fbsize_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    init_assets();
    init_triangle();
    update_projection_matrix();

    if (shader.getID() != 0) {
        shader.activate();
        shader.setUniform("uV_m", camera.GetViewMatrix());
    }

    return true;
}

void App::init_assets() {
    try {
        std::cout << "Loading shaders..." << std::endl;
        shader = ShaderProgram("resources/shaders/tex.vert", "resources/shaders/tex.frag");
        std::cout << "Shaders loaded successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Shader loading error: " << e.what() << std::endl;
        throw;
    }

    try {
        std::cout << "Creating maze..." << std::endl;
        createMazeModel();
        std::cout << "Maze created successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Maze creation error: " << e.what() << std::endl;
        throw;
    }

    try {
        std::cout << "Creating transparent bunnies..." << std::endl;
        createTransparentBunnies();
        std::cout << "Transparent bunnies created successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Transparent bunnies creation error: " << e.what() << std::endl;
        throw;
    }
}

void App::init_triangle() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void App::createTransparentBunnies() {
    GLuint bunnyTexture = textureInit("resources/textures/kralik.jpg");
    std::vector<glm::vec3> bunny_positions = {
        glm::vec3(0.0f, 5.0f, 3.0f),
        glm::vec3(20.0f, 5.0f, 7.0f),
        glm::vec3(40.0f, 5.0f, 5.0f)
    };
    std::vector<glm::vec4> bunny_colors = {
        glm::vec4(1.0f, 0.3f, 0.3f, 0.8f),
        glm::vec4(0.3f, 1.0f, 0.3f, 0.6f),
        glm::vec4(0.3f, 0.3f, 1.0f, 0.4f)
    };

    for (int i = 0; i < 3; i++) {
        Model* bunny = new Model("resources/models/bunny_tri_vnt.obj", shader);
        bunny->meshes[0].texture_id = bunnyTexture;
        bunny->meshes[0].diffuse_material = bunny_colors[i];
        bunny->origin = bunny_positions[i];
        bunny->scale = glm::vec3(0.5f, 0.5f, 0.5f);
        bunny->transparent = true;
        transparent_bunnies.push_back(bunny);
    }
}

GLuint App::textureInit(const std::filesystem::path& filepath) {
    std::cout << "Loading texture: " << filepath << std::endl;
    cv::Mat image = cv::imread(filepath.string(), cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        throw std::runtime_error("Failed to load texture from file: " + filepath.string());
    }
    return gen_tex(image);
}

GLuint App::gen_tex(cv::Mat& image) {
    GLuint ID = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);
    switch (image.channels()) {
    case 3:
        glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case 4:
        glTextureStorage2D(ID, 1, GL_RGBA8, image.cols, image.rows);
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
        break;
    default:
        throw std::runtime_error("Unsupported number of channels in texture: " + std::to_string(image.channels()));
    }
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateTextureMipmap(ID);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return ID;
}

uchar App::getmap(cv::Mat& map, int x, int y) {
    x = std::clamp(x, 0, map.cols - 1);
    y = std::clamp(y, 0, map.rows - 1);
    return map.at<uchar>(y, x);
}

void App::genLabyrinth(cv::Mat& map) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_height(1, map.rows - 2);
    std::uniform_int_distribution<int> uniform_width(1, map.cols - 2);
    std::uniform_int_distribution<int> uniform_block(0, 15);

    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            map.at<uchar>(j, i) = (uniform_block(e1) == 0) ? '#' : '.';
        }
    }
    for (int i = 0; i < map.cols; i++) {
        map.at<uchar>(0, i) = '#';
        map.at<uchar>(map.rows - 1, i) = '#';
    }
    for (int j = 0; j < map.rows; j++) {
        map.at<uchar>(j, 0) = '#';
        map.at<uchar>(j, map.cols - 1) = '#';
    }

    cv::Point2i start_position, end_position;
    do {
        start_position.x = uniform_width(e1);
        start_position.y = uniform_height(e1);
    } while (getmap(map, start_position.x, start_position.y) == '#');
    do {
        end_position.x = uniform_width(e1);
        end_position.y = uniform_height(e1);
    } while (start_position == end_position || getmap(map, end_position.x, end_position.y) == '#');
    map.at<uchar>(end_position.y, end_position.x) = 'e';

    std::cout << "Start: " << start_position << "\nEnd: " << end_position << std::endl;
    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            if (i == start_position.x && j == start_position.y) std::cout << 'X';
            else std::cout << getmap(map, i, j);
        }
        std::cout << std::endl;
    }

    camera.Position.x = start_position.x + 0.5f;
    camera.Position.z = start_position.y + 0.5f;
    camera.Position.y = 0.5f;
}

void App::createMazeModel() {
    cv::Mat atlas = cv::imread("resources/textures/tex_256.png", cv::IMREAD_UNCHANGED);
    if (atlas.empty()) {
        throw std::runtime_error("Failed to load texture atlas!");
    }

    const int texCount = 16;
    const int tileSize = atlas.cols / texCount;
    auto saveTextureFromAtlas = [&](int row, int col, const std::string& filename) {
        cv::Mat tileMat = atlas(cv::Rect(col * tileSize, row * tileSize, tileSize, tileSize));
        cv::imwrite("resources/textures/" + filename, tileMat);
        };
    saveTextureFromAtlas(1, 1, "floor.png");
    saveTextureFromAtlas(3, 2, "wall.png");

    wall_textures.clear();
    GLuint floorTexture = textureInit("resources/textures/floor.png");
    GLuint wallTexture = textureInit("resources/textures/wall.png");
    wall_textures.push_back(floorTexture);
    wall_textures.push_back(wallTexture);

    maze_map = cv::Mat(15, 15, CV_8U);
    genLabyrinth(maze_map);

    for (int j = 0; j < 15; j++) {
        for (int i = 0; i < 15; i++) {
            Model* floor = new Model("resources/models/cube.obj", shader);
            floor->meshes[0].texture_id = floorTexture;
            floor->origin = glm::vec3(i, 0.0f, j);
            floor->scale = glm::vec3(1.0f, 1.0f, 1.0f);
            maze_walls.push_back(floor);

            if (getmap(maze_map, i, j) == '#') {
                Model* wall = new Model("resources/models/cube.obj", shader);
                wall->meshes[0].texture_id = wallTexture;
                wall->origin = glm::vec3(i, 1.0f, j);
                wall->scale = glm::vec3(1.0f, 1.0f, 1.0f);
                maze_walls.push_back(wall);
            }
        }
    }
}

bool App::run() {
    if (!window) {
        std::cerr << "No active GLFW window!" << std::endl;
        return false;
    }

    if (fov <= 0.0f) fov = DEFAULT_FOV;

    shader.activate();
    update_projection_matrix();
    shader.setUniform("uV_m", camera.GetViewMatrix());

    double lastTime = glfwGetTime();
    double lastFrameTime = lastTime;
    int frameCount = 0;
    std::string title = "OpenGL Maze Demo";

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            std::string fpsTitle = title + " | FPS: " + std::to_string(frameCount);
            glfwSetWindowTitle(window, fpsTitle.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }

        glm::vec3 direction = camera.ProcessKeyboard(window, deltaTime);
        camera.Move(direction);
        shader.setUniform("uV_m", camera.GetViewMatrix());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw simple triangle
        glUseProgram(shaderProgram);
        int colorLoc = glGetUniformLocation(shaderProgram, "triangleColor");
        glUniform3f(colorLoc, r, g, b);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw maze (opaque objects)
        for (auto& wall : maze_walls) {
            shader.setUniform("uM_m", wall->getModelMatrix());
            wall->draw();
        }

        // Painter's algorithm for transparent objects
        std::vector<Model*> transparent_objects;
        for (auto& bunny : transparent_bunnies) {
            transparent_objects.push_back(bunny);
        }
        std::sort(transparent_objects.begin(), transparent_objects.end(),
            [this](Model* a, Model* b) {
                float dist_a = glm::distance(camera.Position, a->origin);
                float dist_b = glm::distance(camera.Position, b->origin);
                return dist_a > dist_b;
            });

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        for (auto* model : transparent_objects) {
            shader.setUniform("uM_m", model->getModelMatrix());
            shader.setUniform("u_diffuse_color", model->meshes[0].diffuse_material);
            model->draw();
        }
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return true;
}

void App::update_projection_matrix() {
    if (height < 1) height = 1;
    if (fov <= 0.0f) fov = DEFAULT_FOV;

    float ratio = static_cast<float>(width) / height;
    projection_matrix = glm::perspective(glm::radians(fov), ratio, 0.1f, 20000.0f);

    if (shader.getID() != 0) {
        shader.activate();
        shader.setUniform("uP_m", projection_matrix);
    }
}

GLuint App::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void App::fbsize_callback(GLFWwindow* window, int width, int height) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->width = width;
    app->height = height;
    glViewport(0, 0, width, height);
    app->update_projection_matrix();
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->fov += -5.0f * static_cast<float>(yoffset);
    app->fov = glm::clamp(app->fov, 20.0f, 170.0f);
    app->update_projection_matrix();
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    static bool vsync = false; // Move vsync initialization outside of the switch statement

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_F10:
            vsync = !vsync;
            glfwSwapInterval(vsync ? 1 : 0);
            std::cout << "VSync: " << (vsync ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_R:
            app->r += 0.1f;
            if (app->r > 1.0f) app->r = 0.0f;
            break;
        case GLFW_KEY_G:
            app->g += 0.1f;
            if (app->g > 1.0f) app->g = 0.0f;
            break;
        case GLFW_KEY_B:
            app->b += 0.1f;
            if (app->b > 1.0f) app->b = 0.0f;
            break;
        }
    }
}


void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app->firstMouse) {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }
    double xoffset = xpos - app->lastX;
    double yoffset = app->lastY - ypos;
    app->lastX = xpos;
    app->lastY = ypos;
    app->camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        app->fov = app->DEFAULT_FOV;
        app->update_projection_matrix();
        std::cout << "Zoom reset to default" << std::endl;
    }
}

void create_default_config() {
    json config;
    config["window"] = {
        {"width", 800},
        {"height", 600},
        {"title", "OpenGL Maze Demo"}
    };
    config["graphics"] = {
        {"antialiasing", {
            {"enabled", false},
            {"samples", 4}
        }}
    };
    std::ofstream file("config.json");
    if (!file.is_open()) {
        std::cerr << "Failed to create config file!" << std::endl;
        return;
    }
    file << config.dump(4);
}

json load_config() {
    try {
        std::ifstream file("config.json");
        if (!file.is_open()) {
            std::cout << "Config file does not exist, creating default." << std::endl;
            create_default_config();
            file.open("config.json");
            if (!file.is_open()) {
                throw std::runtime_error("Failed to create config file");
            }
        }
        return json::parse(file);
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        json default_config;
        default_config["window"] = {
            {"width", 800},
            {"height", 600},
            {"title", "OpenGL Maze Demo"}
        };
        default_config["graphics"] = {
            {"antialiasing", {
                {"enabled", false},
                {"samples", 4}
            }}
        };
        return default_config;
    }
}

bool validate_antialiasing_settings(const json& config, bool& antialiasing_enabled, int& samples) {
    bool valid = true;
    if (!config.contains("graphics") || !config["graphics"].contains("antialiasing")) {
        std::cerr << "Warning: Antialiasing settings missing in config." << std::endl;
        antialiasing_enabled = false;
        samples = 0;
        return false;
    }
    const auto& aa_config = config["graphics"]["antialiasing"];
    antialiasing_enabled = aa_config.value("enabled", false);
    samples = aa_config.value("samples", 0);
    if (antialiasing_enabled) {
        if (samples <= 1) {
            std::cerr << "Warning: Antialiasing enabled but samples <= 1. Setting to 4." << std::endl;
            samples = 4;
            valid = false;
        }
        else if (samples > 8) {
            std::cerr << "Warning: Too many antialiasing samples (> 8). Setting to 8." << std::endl;
            samples = 8;
            valid = false;
        }
    }
    return valid;
}