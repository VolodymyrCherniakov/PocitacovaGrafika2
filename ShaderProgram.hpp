#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
    ShaderProgram() = default;
    ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file);

    void activate();
    void deactivate() { glUseProgram(0); };
    void clear();

    void setUniform(const std::string& name, float val);
    void setUniform(const std::string& name, int val);
    void setUniform(const std::string& name, glm::vec3 val);
    void setUniform(const std::string& name, glm::vec4 val);
    void setUniform(const std::string& name, glm::mat3 val);
    void setUniform(const std::string& name, glm::mat4 val);

private:
    GLuint ID{ 0 };
    static GLuint currentShaderID;
    static std::unordered_map<std::string, GLint> uniformCache;

    void cacheUniformLocations();
    GLint getUniformLocation(const std::string& name);

    std::string getShaderInfoLog(GLuint obj);
    std::string getProgramInfoLog(GLuint obj);

    GLuint compile_shader(const std::filesystem::path& source_file, GLenum type);
    GLuint link_shader(const std::vector<GLuint> shader_ids);

    std::string textFileRead(const std::filesystem::path& filename);
};