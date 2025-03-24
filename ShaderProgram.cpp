#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.hpp"

// Constructor: Loads, compiles, and links the vertex and fragment shaders.
ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file) {
    std::vector<GLuint> shader_ids;
    // Compile shaders and store their IDs
    shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
    shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));

    // Link all compiled shaders into a shader program
    ID = link_shader(shader_ids);
}

void ShaderProgram::setUniform(const std::string& name, const float val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniform1f(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const int val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniform1i(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniform3fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4 val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniform4fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "No uniform variable with name: " << name << "\n";
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj) {
    GLint logLength = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);
    std::string infoLog(logLength, ' ');
    glGetShaderInfoLog(obj, logLength, NULL, &infoLog[0]);
    return infoLog;
}

std::string ShaderProgram::getProgramInfoLog(const GLuint obj) {
    GLint logLength = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &logLength);
    std::string infoLog(logLength, ' ');
    glGetProgramInfoLog(obj, logLength, NULL, &infoLog[0]);
    return infoLog;
}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type) {
    // Read the shader source code from file
    std::string shaderSource = textFileRead(source_file);
    const char* src = shaderSource.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // Check compilation success
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::string errorMsg = getShaderInfoLog(shader);
        std::cerr << "Shader compilation error (" << source_file.string() << "):\n" << errorMsg << std::endl;
        throw std::runtime_error("Shader compilation failed: " + source_file.string());
    }
    return shader;
}

GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids) {
    GLuint program = glCreateProgram();

    // Attach all shaders to the program
    for (const auto& shader : shader_ids)
        glAttachShader(program, shader);

    glLinkProgram(program);

    // Check link success
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::string errorMsg = getProgramInfoLog(program);
        std::cerr << "Shader program linking error:\n" << errorMsg << std::endl;
        throw std::runtime_error("Shader program linking failed");
    }

    // Detach and delete shaders as they are no longer needed
    for (const auto& shader : shader_ids) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return program;
}

std::string ShaderProgram::textFileRead(const std::filesystem::path& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Error opening file: " + filename.string());
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
