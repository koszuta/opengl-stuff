#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>

class Shader {
public:
    Shader(std::string source_path);
    ~Shader();

    void attachTo(GLuint program);
    void detachFrom(GLuint program);

private:
    std::string source_path_;
    GLuint ref_;
};

#endif