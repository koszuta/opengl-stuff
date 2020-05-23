#define SDL_MAIN_HANDLED

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "DistantLight.hpp"
#include "Input.hpp"
#include "Koszutils.hpp"
#include "OBJModel.hpp"
#include "PointLight.hpp"
#include "Shader.hpp"
#include "Window.hpp"

// Screen dimension constants
// float aspect = (float) windowWidth / (float) windowHeight;

// Camera spherical coordinates
float camRadius = 64.0;
float camPolar = glm::pi<float>() / 2.0f;
float camAzimuth = 0.0;
float camRot = 0.1;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Model file path required\n");
    }

    try {
        SDL_SetMainReady();
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
            exit(1);
        } else {
            printf("SDL initted...\n");
        }
        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
            fprintf(stderr, "Couldn't initialize SDL_image: %s\n", IMG_GetError());
            exit(1);
        } else {
            printf("SDL_image initted...\n");
        }

        Window window("Practice makes perfect!", 900, 900, true);
        if (window.initialized()) {
            glewInit();

            printf("Model file path: %s\n", argv[1]);
            Model *teapot = new OBJModel(argv[1]);

            int maj, min;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
            printf("GL version: %d.%d\n", maj, min);

            int w, h;
            SDL_GetWindowSize(window.window(), &w, &h);
            printf("Window size: %dx%d\n", w, h);

            // IDs for vertex array and (2) vertex buffer objects
            GLuint vao, vbo[2];

            // Create VAO
            glGenVertexArrays(1, &vao);
            Koszutils::checkGlErrors(__LINE__, "generating vertex arrays");
            glBindVertexArray(vao);
            Koszutils::checkGlErrors(__LINE__, "binding vertex arrays");

            // Create VBOs
            glGenBuffers(2, vbo);
            Koszutils::checkGlErrors(__LINE__, "generating buffers");

            // Vertex buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, teapot->vertexData().size() * sizeof(GLfloat), teapot->vertexData().data(), GL_STATIC_DRAW);
            Koszutils::checkGlErrors(__LINE__, "creating vertices buffer");
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
            Koszutils::checkGlErrors(__LINE__, "enabling vertices attrib");

            // Normal buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, teapot->normalData().size() * sizeof(GLfloat), teapot->normalData().data(), GL_STATIC_DRAW);
            Koszutils::checkGlErrors(__LINE__, "creating normals buffer");
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);
            Koszutils::checkGlErrors(__LINE__, "enabling normals attrib");

            /*
            // TexCoord buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, teapot.polygonCount() * 2 * sizeof(GLfloat), teapot.texCoordsData(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(2);
            */

            /*
            // Quad element buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , vbo[3]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER , teapot.facesCount() * sizeof(GLfloat), teapot.facesData(), GL_STATIC_DRAW);
            glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(3);
            */

            GLuint program = glCreateProgram();
            Koszutils::checkGlErrors(__LINE__, "creating program");

            // Vertex shader
            Shader vertShader("shaders/basic.vert");
            vertShader.attachTo(program);

            // Fragment shader
            Shader fragShader("shaders/basic.frag");
            fragShader.attachTo(program);

            glLinkProgram(program);
            Koszutils::checkGlErrors(__LINE__, "linking program");
            GLint link_status;
            glGetProgramiv(program, GL_LINK_STATUS, &link_status);
            if (link_status == GL_FALSE) {
                int infoLogLength;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
                char errorLog[infoLogLength];
                glGetProgramInfoLog(program, infoLogLength, NULL, &errorLog[0]);
                printf("\nShader Program Error:\n%s", &errorLog[0]);
            }

            vertShader.detachFrom(program);
            fragShader.detachFrom(program);
            Koszutils::checkGlErrors(__LINE__, "detaching/deleting shaders");

            glUseProgram(program);
            Koszutils::checkGlErrors(__LINE__, "using program");

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            Koszutils::checkGlErrors(__LINE__, "enabling depth testing");

            glEnable(GL_MULTISAMPLE);
            if (glIsEnabled(GL_MULTISAMPLE) == GL_TRUE) {
                printf("Multisampling is enabled...\n");
            }

            // glEnable(GL_CULL_FACE);
            // glCullFace(GL_BACK);
            // Koszutils::checkGlErrors(__LINE__, "enabling backface culling");

            // printf("%s\n", glm::to_string(glm::vec3(0.1f, 0.2f, 0.3f) + glm::vec3(0.1f, 0.2f, 0.3f)).data());

            int frame = 0;
            bool model_should_rotate = false;
            glm::mat4 model_rot_matrix = glm::mat4(1.0f);
            while (Input::poll_events()) {

                if (Input::key_is_down(SDLK_UP) || Input::key_is_down(SDLK_w)) {
                    camPolar = glm::max(camPolar - camRot, 0.0f);
                }
                if (Input::key_is_down(SDLK_DOWN) || Input::key_is_down(SDLK_s)) {
                    camPolar = glm::min(camPolar + camRot, glm::pi<float>());
                }
                if (Input::key_is_down(SDLK_RIGHT) || Input::key_is_down(SDLK_d)) {
                    camAzimuth += camRot;
                }
                if (Input::key_is_down(SDLK_LEFT) || Input::key_is_down(SDLK_a)) {
                    camAzimuth -= camRot;
                }
                if (Input::key_is_down(SDLK_PAGEUP)) {
                    camRadius -= 3*camRot;
                }
                if (Input::key_is_down(SDLK_PAGEDOWN)) {
                    camRadius += 3*camRot;
                }
                if (Input::key_was_clicked(SDLK_SPACE)) {
                    model_should_rotate = !model_should_rotate;
                }

                // Build MVP matrix
                if (model_should_rotate) {
                    float rot = frame++ / 100.0f;
                    glm::mat4 model = glm::mat4(1.0f);
                    glm::mat4 xRot = glm::mat4(
                        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                        glm::vec4(0.0f, glm::cos(rot), -glm::sin(rot), 0.0f),
                        glm::vec4(0.0f, glm::sin(rot), glm::cos(rot), 0.0f),
                        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
                    );
                    glm::mat4 yRot = glm::mat4(
                        glm::vec4(glm::cos(rot), 0.0f, glm::sin(rot), 0.0f),
                        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                        glm::vec4(-glm::sin(rot), 0.0f, glm::cos(rot), 0.0f),
                        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
                    );
                    glm::mat4 zRot = glm::mat4(
                        glm::vec4(glm::cos(rot), -glm::sin(rot), 0.0f, 0.0f),
                        glm::vec4(glm::sin(rot), glm::cos(rot), 0.0f, 0.0f),
                        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
                    );
                    model_rot_matrix = zRot * xRot * yRot * model;
                }

                glm::vec3 eye = glm::vec3(camRadius * glm::cos(camPolar), camRadius * glm::sin(camPolar) * glm::sin(camAzimuth), camRadius * glm::sin(camPolar) * glm::cos(camAzimuth));
                glm::mat4 view = glm::lookAt(
                    eye,
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );

                glm::mat4 proj = glm::perspective(45.0f, (GLfloat) window.width() / (GLfloat) window.height(), 1.0f, 100.0f);

                glm::mat4 mvp = proj * view * model_rot_matrix;
                Koszutils::setUniformMat4(program, mvp, "mvp");
                Koszutils::setUniformMat4(program, model_rot_matrix, "model");
                Koszutils::setUniformVec3(program, eye, "eye");

                glm::vec3 color = glm::vec3(1.0f, 0.566f, 0.684f);
                Koszutils::setUniformVec3(program, color, "color");
                float shininess = 20.0f;
                Koszutils::setUniformFloat(program, shininess, "shininess");

                glm::vec3 sunDir = glm::normalize(glm::vec3(0.0, 1.0, 0.0));
                glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 1.0f);
                DistantLight sun(sunDir, sunColor);
                sun.addToProgram(program, eye);


                float diffIntensity = 0.9f;
                Koszutils::setUniformFloat(program, 1.0f - diffIntensity, "ambientMag");
                Koszutils::setUniformFloat(program, diffIntensity, "lightMag");

                teapot->draw();

                SDL_GL_SwapWindow(window.window());
            }

            glUseProgram(0);
            glDeleteProgram(program);
            printf("Deleted program...\n");
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
            glDeleteBuffers(2, vbo);
            printf("Deleted VBOs...\n");
            glDeleteVertexArrays(1, &vao);
            printf("Deleted VAO...\n");

            delete teapot;
            printf("Deleted teapot model...\n");
        }

        IMG_Quit();
        printf("Quit SDL_image...\n");
        SDL_Quit();
        printf("Quit SDL...\n");

    } catch (std::exception &e) {
        fprintf(stderr, "%s\n", e.what());
    }
    return 0;
}
