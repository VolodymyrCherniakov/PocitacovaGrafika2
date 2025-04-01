#pragma once

#include <GL/glew.h> 
#include <GL/wglew.h> 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

//vertex description
struct vertex {
    glm::vec3 position;  // Pozice vrcholu
    glm::vec3 normal;    // Norm?la vrcholu
    glm::vec2 texCoord;  // Texturovac? koordin?ty
};