#include "OBJModel.hpp"

#include <stdio.h>
#include <cctype>
#include <fstream>
#include <iterator>

OBJModel::OBJModel(std::string file_path): polygonCount(0) {
    std::string file_extension = file_path.substr(file_path.length() - 4, file_path.length());
    for (int i = 0; i < 3; i++) {
        file_extension[i] = std::tolower(file_extension[i]);
    }
    if (file_extension.compare(".obj") != 0) {
        throw std::invalid_argument("Not a Wavefront .OBJ file " + file_path);
    }
    OBJModel::parse_(file_path);
}

OBJModel::~OBJModel() {
    delete &vertices_;
    delete &texCoords_;
    delete &normals_;
    delete &verticesData;
    delete &texCoordsData;
    delete &normalsData;
}

glm::vec3 OBJModel::vertex_(GLushort index) {
    if (index < 1 || index > vertices_.size()) {
        throw std::invalid_argument("Vertex index out of range [1, " + std::to_string(vertices_.size() + 1) + "): " + std::to_string(index));
    }
    return vertices_[index - 1];
}

glm::vec2 OBJModel::texCoord_(GLushort index) {
    if (index < 1 || index > texCoords_.size()) {
        throw std::invalid_argument("TexCoord index out of range [1, " + std::to_string(texCoords_.size() + 1) + "): " + std::to_string(index));
    }
    return texCoords_[index - 1];
}

glm::vec3 OBJModel::normal_(GLushort index) {
    if (index < 1 || index > normals_.size()) {
        throw std::invalid_argument("Normal index out of range [1, " + std::to_string(normals_.size() + 1) + "): " + std::to_string(index));
    }
    return normals_[index - 1];
}

void OBJModel::parse_(std::string file_path) {
    printf("OBJModel file_path is %s\n", file_path.c_str());
    std::ifstream fileStream(file_path, std::ifstream::in);
    if (!fileStream) {
        throw std::runtime_error("Could not open Wavefront .OBJ file " + file_path);
    }

    std::string line;
    while (std::getline(fileStream, line)) {
        std::vector<std::string> parts = OBJModel::split_(line, ' ');
        auto it = parts.begin();
        std::string first = *it;
        it++;
        if (first.compare("o") == 0) {
            printf("Object name: %s\n", line.c_str());
        }
        else if (first.compare("mtllib") == 0) {
            printf("Material file path: %s\n", line.c_str());
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
            texCoords_.push_back(tex);
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
            polygonCount++;
            for (; it != parts.end(); it++) {
                std::vector<std::string> attrs = split_(*it, '/');
                GLushort vertIndex = std::stoul(attrs[0]);
                glm::vec3 vert = vertex_(vertIndex);
                verticesData.push_back((GLfloat) vert.x);
                verticesData.push_back((GLfloat) vert.y);
                verticesData.push_back((GLfloat) vert.z);
                if (attrs.size() > 1) {
                    if (attrs[1].length() > 0) {
                        GLushort texCoordIndex = std::stoul(attrs[1]);
                        glm::vec2 tex = texCoord_(texCoordIndex);
                        texCoordsData.push_back((GLfloat) tex.x);
                        texCoordsData.push_back((GLfloat) tex.y);
                    }
                    if (attrs.size() > 2) {
                        GLushort normIndex = std::stoul(attrs[2]);
                        glm::vec3 norm = normal_(normIndex);
                        normalsData.push_back((GLfloat) norm.x);
                        normalsData.push_back((GLfloat) norm.y);
                        normalsData.push_back((GLfloat) norm.z);
                    }
                }
            }
        }
        else {
            printf("%s\n", line.c_str());
        }
    }
    printf("Done parsing %s...\n", file_path.c_str());
}

std::vector<std::string> OBJModel::split_(std::string str, char delimiter) {
    std::vector<std::string> result;
    std::string::size_type start = 0, end = 0;
    for (; end < str.length(); end++) {
        if (str.at(end) == delimiter) {
            std::string sub = str.substr(start, end - start);
            result.push_back(sub);
            start = end + 1;
        }
    }
    std::string sub = str.substr(start, end - start);
    result.push_back(sub);
    return result;
}
