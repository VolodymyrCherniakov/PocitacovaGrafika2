/* model.hpp */
#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 
#include <GL/glew.h>
#include "assets.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"
#include <iostream>

class Model {
public:
    std::vector<Mesh> meshes;
    ShaderProgram* shader;

    Model(const std::string& filename, ShaderProgram* shader) : shader(shader) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        std::vector<GLuint> indices;

        if (!loadOBJ(filename.c_str(), vertices, texCoords, normals)) {
            std::cerr << "Error loading OBJ file: " << filename << std::endl;
            return;
        }

        std::vector<Vertex> vertexData;
        for (size_t i = 0; i < vertices.size(); ++i) {
            Vertex v;
            v.Position = vertices[i];
            v.Normal = (i < normals.size()) ? normals[i] : glm::vec3(0.0f);
            v.TexCoords = (i < texCoords.size()) ? texCoords[i] : glm::vec2(0.0f);
            vertexData.push_back(v);
        }

        meshes.emplace_back(GL_TRIANGLES, shader, vertexData, indices, glm::vec3(0.0f), glm::vec3(0.0f));
    }

    void draw() {
        for (auto& mesh : meshes) {
            mesh.draw();
        }
    }
};