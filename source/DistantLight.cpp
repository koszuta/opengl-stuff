#include "DistantLight.hpp"
#include "Koszutils.hpp"

DistantLight::DistantLight(glm::vec3 direction, glm::vec3 color)
    : direction{direction}, color{color} {
}

DistantLight::~DistantLight() {
}

void DistantLight::addToProgram(GLuint program, glm::vec3 eyePos) {
    Koszutils::setUniformVec3(program, direction, "lightDir");
    Koszutils::setUniformVec3(program, color, "lightColor");

    glm::vec3 halfway = (direction + glm::normalize(eyePos)) / 2.0f;
    Koszutils::setUniformVec3(program, halfway, "halfway");
}
