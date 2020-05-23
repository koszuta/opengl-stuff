#include "Koszutils.hpp"
#include "Model.hpp"

GLushort Model::polyCount() {
    return this->poly_count_;
}

std::vector<GLfloat> Model::vertexData() {
    return this->vertex_data_;
}

std::vector<GLfloat> Model::texCoordData() {
    return this->tex_coord_data_;
}

std::vector<GLfloat> Model::normalData() {
    return this->normal_data_;
}

void Model::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(mode_, 0, this->vertex_data_.size());
    Koszutils::checkGlErrors(__LINE__, "drawing");
}
