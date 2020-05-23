#ifndef DISTANTLIGHT_HPP
#define DISTANTLIGHT_HPP

#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

class DistantLight {
public:
    DistantLight(glm::vec3 direction, glm::vec3 color);
    ~DistantLight();

    glm::vec3 direction;
    glm::vec3 color;

    void addToProgram(GLuint program, glm::vec3 eyePos);
};

#endif