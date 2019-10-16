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
int windowWidth = 1200;
int windowHeight = 675;
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

unsigned long int keyPressed[KEY_ENUM_LENGTH];
unsigned long int keysPressed = 0;

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

Key pollKeyEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // printf("Event type %d\n", e.type);
        switch (e.type) {
            case SDL_QUIT:
                printf("X clicked...\n");
                keyPressed[KEY_QUIT] = true;
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
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (!keyPressed[KEY_UP]) {
                            printf("Up pressed...\n");
                            keyPressed[KEY_UP] = ++keysPressed;
                        }
                        break;
                    case SDLK_DOWN:
                        if (!keyPressed[KEY_DOWN]) {
                            printf("Down pressed...\n");
                            keyPressed[KEY_DOWN] = ++keysPressed;
                        }
                        break;
                    case SDLK_LEFT:
                        if (!keyPressed[KEY_LEFT]) {
                            printf("Left pressed...\n");
                            keyPressed[KEY_LEFT] = ++keysPressed;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!keyPressed[KEY_RIGHT]) {
                            printf("Right pressed...\n");
                            keyPressed[KEY_RIGHT] = ++keysPressed;
                        }
                        break;
                    case SDLK_SPACE:
                        if (!keyPressed[KEY_SPACE]) {
                            printf("Space pressed...\n");
                            keyPressed[KEY_SPACE] = ++keysPressed;
                        }
                        break;
                    case SDLK_ESCAPE:
                        if (!keyPressed[KEY_ESCAPE]) {
                            printf("Esc pressed...\n");
                            keyPressed[KEY_ESCAPE] = ++keysPressed;
                        }
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        printf("Up released...\n");
                        keyPressed[KEY_UP] = 0;
                        break;
                    case SDLK_DOWN:
                        printf("Down released...\n");
                        keyPressed[KEY_DOWN] = 0;
                        break;
                    case SDLK_LEFT:
                        printf("Left released...\n");
                        keyPressed[KEY_LEFT] = 0;
                        break;
                    case SDLK_RIGHT:
                        printf("Right released...\n");
                        keyPressed[KEY_RIGHT] = 0;
                        break;
                    case SDLK_SPACE:
                        printf("Space released...\n");
                        keyPressed[KEY_SPACE] = 0;
                        break;
                    case SDLK_ESCAPE:
                        printf("Esc released...\n");
                        keyPressed[KEY_ESCAPE] = 0;
                        break;
                }
        }
    }
    Key lastPressed = NONE;
    unsigned long int highestPress = 0;
    for (int key = NONE; key < KEY_ENUM_LENGTH; key++) {
        if (keyPressed[key] > highestPress) {
            highestPress = keyPressed[key];
            lastPressed = static_cast<Key>(key);
        }
    }
    return lastPressed;
}

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

    float loop = 1.0f;
    do {
        pollKeyEvents();

        if (keyPressed[KEY_ESCAPE] || keyPressed[KEY_QUIT]) {
            loop = 0.0f;
        }

        // Build MVP matrix
        float rot = loop / 100.0f;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 xRot = glm::mat4(
            glm::vec4(1.0, 0.0, 0.0, 0.0),
            glm::vec4(0.0, glm::cos(rot), -glm::sin(rot), 0.0),
            glm::vec4(0.0, glm::sin(rot), glm::cos(rot), 0.0),
            glm::vec4(0.0, 0.0, 0.0, 1.0)
        );
        glm::mat4 yRot = glm::mat4(
            glm::vec4(glm::cos(rot), glm::sin(rot), 0.0, 0.0),
            glm::vec4(0.0, 1.0, 0.0, 0.0),
            glm::vec4(-glm::sin(rot), 0.0, glm::cos(rot), 0.0),
            glm::vec4(0.0, 0.0, 0.0, 1.0)
        );
        glm::mat4 zRot = glm::mat4(
            glm::vec4(glm::cos(rot), -glm::sin(rot), 0.0, 0.0),
            glm::vec4(glm::sin(rot), glm::cos(rot), 0.0, 0.0),
            glm::vec4(0.0, 0.0, 1.0, 0.0),
            glm::vec4(0.0, 0.0, 0.0, 1.0)
        );
        model = zRot * yRot * xRot * model;

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 20.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 proj = glm::perspective(90.0f, (GLfloat) windowWidth / (GLfloat) windowHeight, 1.0f, 100.0f);

    
        glm::mat4 mvp = proj * view * model;
        
        GLint mvpLocation = glGetUniformLocation(program, "mvp");
        checkGlErrors(__LINE__, "getting location of uniform \"mvp\"");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        checkGlErrors(__LINE__, "creating uniform \"mvp\"");
        
        glm::vec3 color = glm::vec3(1.0, 0.5, 0.75);

        GLint colorLocation = glGetUniformLocation(program, "color");
        checkGlErrors(__LINE__, "getting location of uniform \"color\"");
        glUniform3fv(colorLocation, 1, glm::value_ptr(color));
        checkGlErrors(__LINE__, "creating uniform \"color\"");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_QUADS, 0, teapot.verticesData.size());
        checkGlErrors(__LINE__, "drawing");

        SDL_GL_SwapWindow(gWindow);
    } while (loop++);

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
