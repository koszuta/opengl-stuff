#include <stdio.h>
#include <cctype>
#include <fstream>
#include <iterator>

#include "Koszutils.hpp"
#include "OBJModel.hpp"

OBJModel::OBJModel(std::string source_path): Model::Model() {
    int lastDotIndex = source_path.find_last_of(".");
    std::string extension = source_path.substr(lastDotIndex);
    for (std::string::size_type i = 0; i < extension.length(); i++) {
        extension[i] = std::tolower(extension[i]);
    }
    if (extension.compare(".obj") != 0) {
        char *msg = 0;
        sprintf(msg, "Not a Wavefront .OBJ file: %s", source_path.data());
        throw std::invalid_argument(msg);
        throw msg;
    }
    OBJModel::parse_(source_path);
}

glm::vec3 OBJModel::vertex_(GLushort index) {
    if (index < 1 || index > this->vertices_.size()) {
        char *msg = 0;
        sprintf(msg, "Vertex index out of range [1, %u): %d", (unsigned int) this->vertices_.size() + 1, index);
        throw std::out_of_range(msg);
    }
    return this->vertices_[index - 1];
}

glm::vec2 OBJModel::texCoord_(GLushort index) {
    if (index < 1 || index > this->tex_coords_.size()) {
        char *msg = 0;
        sprintf(msg, "TexCoord index out of range [1, %u): %d", (unsigned int) this->tex_coords_.size() + 1, index);
        throw std::out_of_range(msg);
    }
    return this->tex_coords_[index - 1];
}

glm::vec3 OBJModel::normal_(GLushort index) {
    if (index < 1 || index > this->normals_.size()) {
        char *msg = 0;
        sprintf(msg, "Normal index out of range [1, %u): %d", (unsigned int) this->normals_.size() + 1, index);
        throw std::out_of_range(msg);
    }
    return this->normals_[index - 1];
}

void OBJModel::parse_(std::string source_path) {
    printf("OBJModel source path is %s\n", source_path.data());
    std::ifstream fileStream(source_path, std::ifstream::in);
    if (!fileStream) {
        char *msg = 0;
        sprintf(msg, "Could not open Wavefront .OBJ file %s", source_path.data());
        throw std::runtime_error(msg);
    }

    bool need_to_set_mode = true;
    std::string line;
    while (std::getline(fileStream, line)) {
        std::vector<std::string> parts = Koszutils::split(line, ' ');
        auto it = parts.begin();
        std::string first = *it;
        it++;
        if (first.compare("o") == 0) {
            printf("Object name: %s\n", line.data());
        }
        else if (first.compare("mtllib") == 0) {
            printf("Material file path: %s\n", line.data());
        }
        else if (first.compare("v") == 0) {
            glm::vec3 vert;
            vert.x = std::stof(*it);
            it++;
            vert.y = std::stof(*it);
            it++;
            vert.z = std::stof(*it);
            it++;
            vertices_.push_back(vert);
        }
        else if (first.compare("vt") == 0) {
            glm::vec2 tex;
            tex.x = std::stof(*it);
            it++;
            tex.y = std::stof(*it);
            it++;
            tex_coords_.push_back(tex);
        }
        else if (first.compare("vn") == 0) {
            glm::vec3 norm;
            norm.x = std::stof(*it);
            it++;
            norm.y = std::stof(*it);
            it++;
            norm.z = std::stof(*it);
            it++;
            normals_.push_back(norm);
        }
        else if (first.compare("f") == 0) {
            this->poly_count_++;
            unsigned int gons = 0;
            for (; it != parts.end(); it++, gons++) {
                std::vector<std::string> attrs = Koszutils::split(*it, '/');
                GLushort vertIndex = std::stoul(attrs[0]);
                glm::vec3 vert = vertex_(vertIndex);
                this->vertex_data_.push_back((GLfloat) vert.x);
                this->vertex_data_.push_back((GLfloat) vert.y);
                this->vertex_data_.push_back((GLfloat) vert.z);
                if (attrs.size() > 1) {
                    if (attrs[1].length() > 0) {
                        GLushort texCoordIndex = std::stoul(attrs[1]);
                        glm::vec2 tex = texCoord_(texCoordIndex);
                        this->tex_coord_data_.push_back((GLfloat) tex.x);
                        this->tex_coord_data_.push_back((GLfloat) tex.y);
                    }
                    if (attrs.size() > 2) {
                        GLushort normIndex = std::stoul(attrs[2]);
                        glm::vec3 norm = normal_(normIndex);
                        this->normal_data_.push_back((GLfloat) norm.x);
                        this->normal_data_.push_back((GLfloat) norm.y);
                        this->normal_data_.push_back((GLfloat) norm.z);
                    }
                }
            }

            if (need_to_set_mode) {
                need_to_set_mode = !need_to_set_mode;
                printf("n-gon=%u\n", gons);
                switch (gons) {
                    case 1: {
                        this->mode_ = GL_POINTS;
                    } break;
                    case 2: {
                        this->mode_ = GL_LINES;
                    } break;
                    case 3: {
                        this->mode_ = GL_TRIANGLES;
                    } break;
                    case 4: {
                        this->mode_ = GL_QUADS;
                    } break;
                }
            }
        }
        else {
            // printf("%s\n", line.data());
        }
    }
    printf("Done parsing %s...\n", source_path.data());
}
