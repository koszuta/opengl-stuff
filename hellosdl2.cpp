#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
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

#include "OBJModel.cpp"

// Textures constants
enum Texture {
    TEXTURE_DEFAULT,
    TEXTURE_UP,
    TEXTURE_DOWN,
    TEXTURE_LEFT,
    TEXTURE_RIGHT,
    TEXTURE_ENUM_LENGTH
};

// Keys
enum Key {
    NONE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_ESCAPE,
    KEY_QUIT,
    KEY_ENUM_LENGTH
};

// Screen dimension constants
int windowWidth = 1000;
int windowHeight = 1000;
float aspect = (float) windowWidth / (float) windowHeight;

// The window we'll be rendering to
SDL_Window* gWindow   = nullptr;
unsigned int windowID = 0;

// Images to load
SDL_Surface* surfaces[TEXTURE_ENUM_LENGTH];

// Current displayed texture
SDL_Texture* gTextures[TEXTURE_ENUM_LENGTH];

// OpenGL Context
SDL_GLContext glContext;

// Camera spherical coordinates
float camRadius = 57.0;
float camPolar = glm::pi<float>() / 2.0f;
float camAzimuth = 0.0;
float camRot = 0.1;

bool keyPressed[SDL_SCANCODE_APP2 + 1];

void checkGlErrors(int line, std::string activity) {
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
        printf("Line %d: Error %s: %s\n", line, activity.c_str(), errorString.c_str());
    }
}

void cleanUpSDL() {
    // Deallocate surface
    for (int i = 0; i < TEXTURE_ENUM_LENGTH; i++) {
        SDL_DestroyTexture(gTextures[i]);
        gTextures[i] = nullptr;
    }

    SDL_GL_DeleteContext(glContext);
    glContext = nullptr;

    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Keycode pollKeyEvents() {
    SDL_Scancode lastPressed;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                printf("X clicked...\n");
                lastPressed = (SDL_Scancode) -1;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.windowID == windowID) {
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        int newWidth  = e.window.data1;
                        int newHeight = e.window.data2;
                        printf("Window resized to %dx%d, aspect %f...\n", newWidth, newHeight, (float) newWidth / (float) newHeight);
                        if((float) newWidth / (float) newHeight > aspect) {
                            newHeight = (1.f / aspect) * newWidth;
                        }
                        else {
                            newWidth = aspect * newHeight; 
                        }
                        printf("Window resized to %dx%d, aspect %f...\n", newWidth, newHeight, (float) newWidth / (float) newHeight);
                        windowWidth  = newWidth;
                        windowHeight = newHeight;
                        // SDL_SetWindowSize(gWindow, windowWidth, windowHeight);
                    }
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                SDL_Scancode key = e.key.keysym.scancode;

                if (keyPressed[key] && e.type == SDL_KEYUP) {
                    keyPressed[key] = false;
                    printf("%x released...\n", key);
                }
                else if (!keyPressed[key] && e.type == SDL_KEYDOWN) {
                    lastPressed = key;
                    keyPressed[key] = true;
                    printf("%x pressed...\n", key);
                }
                break;
        }
    }
    return lastPressed >= 0 ? SDL_GetKeyFromScancode(lastPressed) : -1;
}

bool isPressed(SDL_Keycode key) {
    return keyPressed[SDL_GetScancodeFromKey(key)];
}

SDL_Texture* loadTexture(std::string path) {
    SDL_Texture* texture = nullptr;
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else {
		// Convert texture to surface pixels
		// texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (texture == nullptr) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
    return texture;
}

void setUniformFloat(GLuint program, float f, std::string name) {
    GLint location = glGetUniformLocation(program, name.c_str());
    checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
    glUniform1f(location, f);
    checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
}

void setUniformVec3(GLuint program, glm::vec3 vec, std::string name) {
    GLint location = glGetUniformLocation(program, name.c_str());
    checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
    glUniform3fv(location, 1, glm::value_ptr(vec));
    checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
}

void setUniformMat4(GLuint program, glm::mat4 mat, std::string name) {
    GLint location = glGetUniformLocation(program, name.c_str());
    checkGlErrors(__LINE__, "getting location of uniform \"" + name + "\"");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    checkGlErrors(__LINE__, "creating uniform \"" + name + "\"");
}

int main(int argc, char* args[]) {
    OBJModel teapot("models\\utah-teapot\\utah-teapot.obj");
    
    // Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        cleanUpSDL();
        return -1;
	}

    // Create window
    gWindow = SDL_CreateWindow("OpenGL 4.6", 1, 31, windowWidth, windowHeight, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (!gWindow) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        cleanUpSDL();
        return -1;
    }
    windowID = SDL_GetWindowID(gWindow);
    
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        cleanUpSDL();
        return -1;
    }

    // Create GL Context
    glContext = SDL_GL_CreateContext(gWindow);
    if (!glContext) {
        printf("Couldn't create GL Context! SDL_image Error: %s\n", SDL_GetError());
        cleanUpSDL();
        return -1;
    }
    
    // Use OpenGL 4.6 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Use double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Use 24-bit depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Turn on VSync
    if (SDL_GL_SetSwapInterval(-1)) {
        printf("Error setting Adaptive VSync! SDL_image Error: %s\n", SDL_GetError());
        printf("Using regular VSync instead...");
        if (SDL_GL_SetSwapInterval(1)) {
            printf("Error setting VSync! SDL_image Error: %s\n", SDL_GetError());
            cleanUpSDL();
            return -1;
        }
    }

    glewInit();

    // IDs for vertex array and (2) vertex buffer objects
    GLuint vao, vbo[2];

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    checkGlErrors(__LINE__, "creating vertex arrays");

    // Create VBOs
    glGenBuffers(2, vbo);
    checkGlErrors(__LINE__, "generating buffers");

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, teapot.verticesData.size() * sizeof(GLfloat), teapot.verticesData.data(), GL_STATIC_DRAW);
    checkGlErrors(__LINE__, "creating vertices buffer");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    checkGlErrors(__LINE__, "enabling vertices attrib");

    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, teapot.normalsData.size() * sizeof(GLfloat), teapot.normalsData.data(), GL_STATIC_DRAW);
    checkGlErrors(__LINE__, "creating normals buffer");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    checkGlErrors(__LINE__, "enabling normals attrib");

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
    checkGlErrors(__LINE__, "creating program");

    GLint isSuccess = GL_FALSE;
	int infoLogLength;

    // Read vertex shader
    std::stringstream vertBuffer;
    std::ifstream vertStream("shaders\\tutorial2.vert");
    vertBuffer << vertStream.rdbuf();
    std::string vertString = vertBuffer.str();
    const GLchar* tutorial2VertSource = (const GLchar*) vertString.c_str();
    // std::cout << tutorial2VertSource << std::endl;
    GLuint tutorial2Vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(tutorial2Vert, 1, &tutorial2VertSource, NULL);
    checkGlErrors(__LINE__, "setting source for shader \"tutorial2Vert\"");
    glCompileShader(tutorial2Vert);
    checkGlErrors(__LINE__, "compiling shader \"tutorial2Vert\"");
    glGetShaderiv(tutorial2Vert, GL_COMPILE_STATUS, &isSuccess);
	if (isSuccess == GL_FALSE) {
	    glGetShaderiv(tutorial2Vert, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorLog(infoLogLength);
		glGetShaderInfoLog(tutorial2Vert, infoLogLength, NULL, &errorLog[0]);
		printf("\nVert Shader Error:\n%s", &errorLog[0]);
	}
    glAttachShader(program, tutorial2Vert);
    checkGlErrors(__LINE__, "attaching shader \"tutorial2Vert\"");

    // Read fragment shader
    std::stringstream fragBuffer;
    std::ifstream fragStream("shaders\\tutorial2.frag");
    fragBuffer << fragStream.rdbuf();
    std::string fragString = fragBuffer.str();
    const GLchar* tutorial2FragSource = (const GLchar*) fragString.c_str();
    // std::cout << tutorial2FragSource << std::endl;
    GLuint tutorial2Frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(tutorial2Frag, 1, &tutorial2FragSource, NULL);
    checkGlErrors(__LINE__, "setting source for shader \"tutorial2Frag\"");
    glCompileShader(tutorial2Frag);
    checkGlErrors(__LINE__, "compiling shader \"tutorial2Frag\"");
    glGetShaderiv(tutorial2Frag, GL_COMPILE_STATUS, &isSuccess);
	if (isSuccess == GL_FALSE) {
	    glGetShaderiv(tutorial2Frag, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorLog(infoLogLength);
		glGetShaderInfoLog(tutorial2Frag, infoLogLength, NULL, &errorLog[0]);
		printf("\nFrag Shader Error:\n%s", &errorLog[0]);
	}
    glAttachShader(program, tutorial2Frag);
    checkGlErrors(__LINE__, "attaching shader \"tutorial2Frag\"");

    glLinkProgram(program);
    checkGlErrors(__LINE__, "linking program");
    glGetProgramiv(program, GL_LINK_STATUS, &isSuccess);
	if (isSuccess == GL_FALSE) {
	    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &errorLog[0]);
		printf("\nShader Program Error:\n%s", &errorLog[0]);
	}

    glDetachShader(program, tutorial2Vert);
    glDetachShader(program, tutorial2Frag);
    glDeleteShader(tutorial2Vert);
    glDeleteShader(tutorial2Frag);
    checkGlErrors(__LINE__, "detaching/deleting shaders");

    glUseProgram(program);
    checkGlErrors(__LINE__, "using program");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    checkGlErrors(__LINE__, "enabling depth testing");

    printf("%s\n", glm::to_string(glm::vec3(0.1f, 0.2f, 0.3f) + glm::vec3(0.1f, 0.2f, 0.3f)).c_str());

    int loop = 0;
    for (;;) {
        SDL_Keycode lastPressed = pollKeyEvents();

        if (lastPressed == -1 || keyPressed[SDL_GetScancodeFromKey(SDLK_ESCAPE)]) {
            break;
        }

        if (isPressed(SDLK_UP)) {
            camPolar = glm::max(camPolar - camRot, 0.0f);
        }
        if (isPressed(SDLK_DOWN)) {
            camPolar = glm::min(camPolar + camRot, glm::pi<float>());
        }
        if (isPressed(SDLK_RIGHT)) {
            camAzimuth += camRot;
        }
        if (isPressed(SDLK_LEFT)) {
            camAzimuth -= camRot;
        }
        if (isPressed(SDLK_PAGEUP)) {
            camRadius -= 3*camRot;
        }
        if (isPressed(SDLK_PAGEDOWN)) {
            camRadius += 3*camRot;
        }

        // Build MVP matrix
        float rot = loop / 100.0f;
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
        model = zRot * xRot * yRot * model;

        glm::vec3 eye = glm::vec3(camRadius * glm::cos(camPolar), camRadius * glm::sin(camPolar) * glm::sin(camAzimuth), camRadius * glm::sin(camPolar) * glm::cos(camAzimuth));
        glm::mat4 view = glm::lookAt(
            eye,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 proj = glm::perspective(45.0f, (GLfloat) windowWidth / (GLfloat) windowHeight, 1.0f, 100.0f);

        glm::mat4 mvp = proj * view * model;
        setUniformMat4(program, mvp, "mvp");
        setUniformMat4(program, model, "model");
        setUniformVec3(program, eye, "eye");
        
        glm::vec3 color = glm::vec3(1.0f, 0.566f, 0.684f);
        setUniformVec3(program, color, "color");

        float lightMag = 0.9f;
        setUniformFloat(program, 1.0f - lightMag, "ambientMag");
        setUniformFloat(program, lightMag, "lightMag");

        glm::vec3 lightDir = glm::normalize(glm::vec3(0.0, 1.0, 0.0));
        setUniformVec3(program, lightDir, "lightDir");
        glm::vec3 halfway = (lightDir + glm::normalize(eye)) / 2.0f;
        setUniformVec3(program, halfway, "halfway");
        setUniformFloat(program, 20.0f, "shininess");

        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        setUniformVec3(program, lightColor, "lightColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_QUADS, 0, teapot.verticesData.size());
        checkGlErrors(__LINE__, "drawing");

        SDL_GL_SwapWindow(gWindow);
        loop++;
    }

    glUseProgram(0);
    glDeleteProgram(program);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
    
    cleanUpSDL();
    
    delete &teapot;

	return 0;
}
