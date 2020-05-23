#ifndef OBJMODEL_HPP
#define OBJMODEL_HPP

#include <string>
#include <vector>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>

class Model {
public:
    virtual ~Model(){}

    void draw();

    GLushort polyCount();
    std::vector<GLfloat> vertexData();
    std::vector<GLfloat> texCoordData();
    std::vector<GLfloat> normalData();

protected:
    Model(): mode_(GL_TRIANGLES), poly_count_(0){}

    GLenum mode_;
    GLushort poly_count_;
    std::vector<GLfloat> vertex_data_;
    std::vector<GLfloat> tex_coord_data_;
    std::vector<GLfloat> normal_data_;
};

#endif