#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <stdexcept>
#include <glm/glm.hpp>

#include "assets.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
    glm::vec3 orientation{};
    ShaderProgram shader;

    // Constructor: load the model from an OBJ file
    Model(const std::filesystem::path& filename, ShaderProgram shader) : shader(shader) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(filename.string().c_str(), positions, uvs, normals)) {
            throw std::runtime_error("Failed to load OBJ file: " + filename.string());
        }

        // Combine the loaded positions, uvs, and normals into vertices
        std::vector<Vertex> vertices;
        size_t numVertices = positions.size();
        vertices.reserve(numVertices);
        for (size_t i = 0; i < numVertices; ++i) {
            Vertex vertex;
            vertex.Position = positions[i];
            vertex.TexCoords = (i < uvs.size() ? uvs[i] : glm::vec2(0.0f));
            vertex.Normal = (i < normals.size() ? normals[i] : glm::vec3(0.0f));
            vertices.push_back(vertex);
        }

        // Create sequential indices (since the OBJ loader unrolls the data)
        std::vector<GLuint> indices(vertices.size());
        for (GLuint i = 0; i < vertices.size(); ++i) {
            indices[i] = i;
        }

        // Create one mesh from the loaded vertex data
        Mesh mesh(GL_TRIANGLES, shader, vertices, indices, glm::vec3(0.0f), glm::vec3(0.0f));
        meshes.push_back(mesh);

        // Set model name based on the filename stem
        name = filename.stem().string();
    }

    // Update model properties (for animation, etc.)
    void update(const float delta_t) {
        // Example: update the origin based on elapsed time
        // origin += glm::vec3(3.0f, 0.0f, 0.0f) * delta_t;
    }

    // Draw all meshes in the model
    void draw(const glm::vec3& offset = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f)) {
        for (auto& mesh : meshes) {  // Removed 'const'
            mesh.draw(origin + offset, orientation + rotation);
        }
    }

};
