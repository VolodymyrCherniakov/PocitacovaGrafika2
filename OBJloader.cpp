#include <string>
#include <GL/glew.h> 
#include <glm/glm.hpp>

#include "OBJloader.hpp"
#include <map>

#define MAX_LINE_SIZE 255

bool loadOBJ(const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<GLuint>& out_indices)
{
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::map<std::tuple<int, int, int>, GLuint> uniqueVertexMap;

    FILE* file;
    fopen_s(&file, path, "r");
    if (!file) {
        printf("Error: Could not open OBJ file: %s\n", path);
        return false;
    }

    char lineHeader[256];
    while (fscanf_s(file, "%s", lineHeader, (unsigned)_countof(lineHeader)) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf_s(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf_s(file, "%f %f", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf_s(file, "%f %f %f", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            GLuint indices[4];
            int vertexIndex[4], uvIndex[4], normalIndex[4];
            int count = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2],
                &vertexIndex[3], &uvIndex[3], &normalIndex[3]);

            if (count < 9) {
                printf("Error parsing face in OBJ file.\n");
                fclose(file);
                return false;
            }

            // Convert from 1-based to 0-based
            for (int i = 0; i < (count == 12 ? 4 : 3); ++i) {
                std::tuple<int, int, int> key = { vertexIndex[i] - 1, uvIndex[i] - 1, normalIndex[i] - 1 };

                if (uniqueVertexMap.count(key) == 0) {
                    uniqueVertexMap[key] = static_cast<GLuint>(out_vertices.size());
                    out_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
                    out_uvs.push_back(temp_uvs[uvIndex[i] - 1]);
                    out_normals.push_back(temp_normals[normalIndex[i] - 1]);
                }
                indices[i] = uniqueVertexMap[key];
            }

            // Always push first triangle
            out_indices.push_back(indices[0]);
            out_indices.push_back(indices[1]);
            out_indices.push_back(indices[2]);

            // If we have a quad, convert to two triangles
            if (count == 12) {
                out_indices.push_back(indices[0]);
                out_indices.push_back(indices[2]);
                out_indices.push_back(indices[3]);
            }
        }
    }

    fclose(file);
    return true;
}
