#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "Model.hpp"

class OBJModel: public Model {
public:
    OBJModel(std::string source_path);
    ~OBJModel(){}

private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec2> tex_coords_;
    std::vector<glm::vec3> normals_;

    glm::vec3 vertex_(GLushort index);
    glm::vec2 texCoord_(GLushort index);
    glm::vec3 normal_(GLushort index);

    void parse_(std::string source_path);
};

#endif