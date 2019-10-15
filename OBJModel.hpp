#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <string>
#include <vector>
#include <GL/GL.h>
#include <glm/glm.hpp>

class OBJModel {
    public:
        OBJModel(std::string file_path);
        ~OBJModel();

        std::vector<GLfloat> verticesData;
        std::vector<GLfloat> texCoordsData;
        std::vector<GLfloat> normalsData;

        GLushort polygonCount;

    private:
        std::vector<glm::vec3> vertices_;
        std::vector<glm::vec2> texCoords_;
        std::vector<glm::vec3> normals_;

        glm::vec3 vertex_(GLushort index);
        glm::vec2 texCoord_(GLushort index);
        glm::vec3 normal_(GLushort index);

        void parse_(std::string file_path);
        
        std::vector<std::string> split_(std::string str, char delimiter);
};

#endif