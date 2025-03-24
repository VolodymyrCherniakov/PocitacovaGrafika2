#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstddef> // for offsetof
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "assets.hpp"
#include "ShaderProgram.hpp"

class Mesh {
public:
    // Mesh transform data
    glm::vec3 origin{};
    glm::vec3 orientation{};

    GLuint texture_id{ 0 }; // texture id=0 means no texture
    GLenum primitive_type = GL_TRIANGLES; // default primitive type
    ShaderProgram shader;

    // Mesh material properties
    glm::vec4 ambient_material{ 1.0f };  // white, opaque
    glm::vec4 diffuse_material{ 1.0f };  // white, opaque
    glm::vec4 specular_material{ 1.0f }; // white, opaque
    float reflectivity{ 1.0f };

    // Constructor for indirect (indexed) drawing
    Mesh(GLenum primitive_type, ShaderProgram shader, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices,
        const glm::vec3& origin, const glm::vec3& orientation, GLuint texture_id = 0) :
        primitive_type(primitive_type),
        shader(shader),
        origin(origin),
        orientation(orientation),
        texture_id(texture_id),
        meshVertices(vertices),
        meshIndices(indices)
    {
        // Create and initialize VAO, VBO, EBO and set vertex attribute pointers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Create and fill the vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(Vertex), meshVertices.data(), GL_STATIC_DRAW);

        // Create and fill the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(GLuint), meshIndices.data(), GL_STATIC_DRAW);

        // Setup vertex attribute pointers
        // Position attribute (location 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);
        // Normal attribute (location 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // Texture coordinates attribute (location 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0); // Unbind VAO
    };

    // Draw the mesh using the stored shader and buffers
    void draw(const glm::vec3& offset, const glm::vec3& rotation) {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!\n";
            return;
        }

        shader.activate();
        // (For future use, set additional uniform variables such as transformation matrices)

        glBindVertexArray(VAO);
        glDrawElements(primitive_type, static_cast<GLsizei>(meshIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Clear allocated resources and reset member variables
    void clear(void) {
        texture_id = 0;
        primitive_type = GL_POINTS;
        meshVertices.clear();
        meshIndices.clear();
        origin = glm::vec3(0.0f);
        orientation = glm::vec3(0.0f);

        if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
        if (EBO) { glDeleteBuffers(1, &EBO); EBO = 0; }
        if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    };

private:
    // OpenGL buffer IDs (0 indicates uninitialized)
    unsigned int VAO{ 0 }, VBO{ 0 }, EBO{ 0 };
    // Private storage for vertex and index data
    std::vector<Vertex> meshVertices;
    std::vector<GLuint> meshIndices;
};
