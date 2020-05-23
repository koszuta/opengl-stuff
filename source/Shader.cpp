#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>

#include "Koszutils.hpp"
#include "Shader.hpp"

Shader::Shader(std::string source_path): source_path_{source_path} {
    std::stringstream vertBuffer;
    std::ifstream vertStream(this->source_path_, std::ifstream::in);
    vertBuffer << vertStream.rdbuf();
    std::string vertString = vertBuffer.str();
    const GLchar* source = (const GLchar*) vertString.c_str();

    GLenum shader_type;
    int lastDotIndex = this->source_path_.find_last_of(".");
    std::string extension = this->source_path_.substr(lastDotIndex);
    if (extension.compare(".vert") == 0) {
        shader_type = GL_VERTEX_SHADER;
        // printf("GL_VERTEX_SHADER\n");
    } else if (extension.compare(".frag") == 0) {
        shader_type = GL_FRAGMENT_SHADER;
        // printf("GL_FRAGMENT_SHADER\n");
    } else if (extension.compare(".geom") == 0) {
        shader_type = GL_GEOMETRY_SHADER;
        // printf("GL_GEOMETRY_SHADER\n");
    } else if (extension.compare(".comp") == 0) {
        shader_type = GL_COMPUTE_SHADER;
        // printf("GL_COMPUTE_SHADER\n");
    } else if (extension.compare(".tesc") == 0) {
        shader_type = GL_TESS_CONTROL_SHADER;
        // printf("GL_TESS_CONTROL_SHADER\n");
    } else if (extension.compare(".tese") == 0) {
        shader_type = GL_TESS_EVALUATION_SHADER;
        // printf("GL_TESS_EVALUATION_SHADER\n");
    } else {
        fprintf(stderr, "Unsupported shader type: %s", extension.c_str());
        exit(1);
    }
    this->ref_ = glCreateShader(shader_type);

    glShaderSource(this->ref_, 1, &source, NULL);
    Koszutils::checkGlErrors(__LINE__, "setting source for shader \"" + this->source_path_ + "\"");
    glCompileShader(this->ref_);
    Koszutils::checkGlErrors(__LINE__, "compiling shader \"" + this->source_path_ + "\"");
    GLint compilation_status;
    glGetShaderiv(this->ref_, GL_COMPILE_STATUS, &compilation_status);
    if (compilation_status == GL_FALSE) {
        int infoLogLength;
        glGetShaderiv(this->ref_, GL_INFO_LOG_LENGTH, &infoLogLength);
        char errorLog[infoLogLength];
        glGetShaderInfoLog(this->ref_, infoLogLength, NULL, errorLog);
        fprintf(stderr, "%s", errorLog);
        exit(1);
    }
}

Shader::~Shader() {
    glDeleteShader(this->ref_);
    Koszutils::checkGlErrors(__LINE__, "deleting shader \"" + this->source_path_ + "\"");
}

void Shader::attachTo(GLuint program) {
    // printf("Attaching shader %d to program %d...\n", this->ref_, program);
    glAttachShader(program, this->ref_);
    Koszutils::checkGlErrors(__LINE__, "attaching shader \"" + this->source_path_ + "\"");
}

void Shader::detachFrom(GLuint program) {
    // printf("Detaching shader %d from program %d...\n", this->ref_, program);
    glDetachShader(program, this->ref_);
    Koszutils::checkGlErrors(__LINE__, "detaching shader \"" + this->source_path_ + "\"");
}
