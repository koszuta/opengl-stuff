#ifndef KOSZUTILS_HPP
#define KOSZUTILS_HPP

#include <string>
#include <vector>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Koszutils {
    inline void checkGlErrors(int line, std::string activity) {
        GLenum glError;
        std::string errorString;
        while ((glError = glGetError()) != GL_NO_ERROR) {
            switch (glError) {
                case GL_INVALID_ENUM:
                    errorString = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    errorString = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    errorString = "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    errorString = "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    errorString = "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    errorString = "GL_OUT_OF_MEMORY";
            }
            fprintf(stderr, "Line %d: %s %s\n", line, errorString.c_str(), activity.c_str());
        }
    }

    inline void setUniformFloat(GLuint program, float f, std::string name) {
        GLint location = glGetUniformLocation(program, name.c_str());
        checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
        glUniform1f(location, f);
        checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
    }

    inline void setUniformVec3(GLuint program, glm::vec3 vec, std::string name) {
        GLint location = glGetUniformLocation(program, name.c_str());
        checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
        glUniform3fv(location, 1, glm::value_ptr(vec));
        checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
    }

    inline void setUniformMat4(GLuint program, glm::mat4 mat, std::string name) {
        GLint location = glGetUniformLocation(program, name.c_str());
        checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
    }

    inline std::vector<std::string> split(std::string str, char delim) {
        std::vector<std::string> result;
        std::string::size_type start = 0, end = 0;
        for (; end < str.length(); end++) {
            if (str.at(end) == delim) {
                std::string sub = str.substr(start, end - start);
                result.push_back(sub);
                start = end + 1;
            }
        }
        std::string sub = str.substr(start, end - start);
        result.push_back(sub);
        return result;
    }
}

#endif