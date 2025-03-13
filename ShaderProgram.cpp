#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ShaderProgram.hpp"

GLuint ShaderProgram::currentShaderID = 0;
std::unordered_map<std::string, GLint> ShaderProgram::uniformCache;

ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file) {
    std::vector<GLuint> shader_ids;
    shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
    shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));
    ID = link_shader(shader_ids);
    cacheUniformLocations();
}

void ShaderProgram::setUniform(const std::string& name, const int val) {
    GLint loc = getUniformLocation(name);
    if (loc != -1) glUniform1i(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val) {
    GLint loc = getUniformLocation(name);
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4 val) {
    GLint loc = getUniformLocation(name);
    if (loc != -1) glUniform4fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val) {
    GLint loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
    GLint loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::activate() {
    if (currentShaderID != ID) {
        glUseProgram(ID);
        currentShaderID = ID;
    }
}

void ShaderProgram::cacheUniformLocations() {
    uniformCache.clear();
    GLint count;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; ++i) {
        char name[256];
        GLsizei length;
        glGetActiveUniformName(ID, i, sizeof(name), &length, name);
        uniformCache[name] = glGetUniformLocation(ID, name);
    }
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) return it->second;
    return -1;
}

std::string ShaderProgram::getShaderInfoLog(const GLuint shader) {
    GLint log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    std::string log(log_length, '\0');
    glGetShaderInfoLog(shader, log_length, nullptr, log.data());
    return log;
}

std::string ShaderProgram::getProgramInfoLog(const GLuint program) {
    GLint log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    std::string log(log_length, '\0');
    glGetProgramInfoLog(program, log_length, nullptr, log.data());
    return log;
}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type) {
    GLuint shader = glCreateShader(type);
    std::string source = textFileRead(source_file);
    const char* src_ptr = source.c_str();
    glShaderSource(shader, 1, &src_ptr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Shader compilation failed: " << getShaderInfoLog(shader) << '\n';
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation error");
    }
    return shader;
}

GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids) {
    GLuint program = glCreateProgram();
    for (GLuint id : shader_ids) glAttachShader(program, id);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Shader program linking failed: " << getProgramInfoLog(program) << '\n';
        glDeleteProgram(program);
        throw std::runtime_error("Shader linking error");
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
