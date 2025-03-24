#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
    // Constructors
    ShaderProgram(void) = default; // Does nothing
    ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file);

    // Activate / Deactivate shader program
    void activate(void) { glUseProgram(ID); }
    void deactivate(void) { glUseProgram(0); }

    // Clear resources
    void clear(void) {
        deactivate();
        glDeleteProgram(ID);
        ID = 0;
    }

    // Uniform setter functions
    void setUniform(const std::string& name, const float val);
    void setUniform(const std::string& name, const int val);
    void setUniform(const std::string& name, const glm::vec3 val);
    void setUniform(const std::string& name, const glm::vec4 val);
    void setUniform(const std::string& name, const glm::mat3 val);
    void setUniform(const std::string& name, const glm::mat4 val);

private:
    GLuint ID{ 0 }; // Shader program ID

    // Helper functions to get info logs
    std::string getShaderInfoLog(const GLuint obj);
    std::string getProgramInfoLog(const GLuint obj);

    // Shader compilation and linking functions
    GLuint compile_shader(const std::filesystem::path& source_file, const GLenum type);
    GLuint link_shader(const std::vector<GLuint> shader_ids);

    // Read text file (shader source)
    std::string textFileRead(const std::filesystem::path& filename);
};
