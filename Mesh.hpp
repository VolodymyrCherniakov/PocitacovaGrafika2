/* mesh.hpp */
#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>

#include "assets.hpp"
#include "ShaderProgram.hpp"

class Mesh {
public:
    glm::vec3 origin{};
    glm::vec3 orientation{};
    GLuint texture_id{ 0 };
    GLenum primitive_type = GL_TRIANGLES;
    ShaderProgram* shader;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GLuint VAO{ 0 }, VBO{ 0 }, EBO{ 0 };

    Mesh(GLenum primitive_type, ShaderProgram* shader, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const glm::vec3& origin, const glm::vec3& orientation, GLuint texture_id = 0)
        : primitive_type(primitive_type), shader(shader), vertices(vertices), indices(indices), origin(origin), orientation(orientation), texture_id(texture_id) {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }

    void draw() {
        shader->activate();
        shader->setUniform("color", glm::vec3(1.0, 0.0, 0.0));
        glBindVertexArray(VAO);
        glDrawElements(primitive_type, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void clear() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
};