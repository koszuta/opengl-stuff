#include <stdio.h>
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
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 640;

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;
// Images to load
SDL_Surface* surfaces[TEXTURE_ENUM_LENGTH];

// Current displayed texture
SDL_Texture* gTextures[TEXTURE_ENUM_LENGTH];

// OpenGL Context
SDL_GLContext glContext;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

unsigned long int keyPressed[KEY_ENUM_LENGTH];
unsigned long int keysPressed = 0;

void cleanUp() {
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

boolean checkGlErrors() {
    GLenum glError;
    while ((glError = glGetError()) != 0) {
        printf("Error: %d\n", glError);
    }
    return glError;
}

int main(int argc, char* args[]) {
    OBJModel teapot("models\\utah-teapot\\utah-teapot.obj");
    
    printf("\nPolygons: %d\n\n", teapot.polygonCount);

    printf("Vertices: %d\n", (int)teapot.verticesData.size() / 3);
    printf("Face 1, Vertex 1: %f\n", teapot.verticesData.front());
    printf("Face 33, Vertex 2: %f\n", teapot.verticesData.at(100));
    printf("Face 2016, Vertex 3: %f\n\n", teapot.verticesData.back());
    
    printf("TexCoords: %d\n", (int)teapot.texCoordsData.size() / 2);
    printf("Face 1, TexCoord 1: %f\n", teapot.texCoordsData.front());
    printf("Face 33, TexCoord 2: %f\n", teapot.texCoordsData.at(100));
    printf("Face 2016, TexCoord 2: %f\n\n", teapot.texCoordsData.back());

    printf("Normals: %d\n", (int)teapot.normalsData.size() / 3);
    printf("Face 1, Normal 1: %f\n", teapot.normalsData.front());
    printf("Face 33, Normal 2: %f\n", teapot.normalsData.at(100));
    printf("Face 2016, Normal 3: %f\n\n", teapot.normalsData.back());
    
    // Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        cleanUp();
        return -1;
	}

    // Create window
    gWindow = SDL_CreateWindow("OpenGL 4.6", 1, 31, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (!gWindow) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        cleanUp();
        return -1;
    }
    
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        cleanUp();
        return -1;
    }

    // Create GL Context
    glContext = SDL_GL_CreateContext(gWindow);
    if (!glContext) {
        printf("Couldn't create GL Context! SDL_image Error: %s\n", SDL_GetError());
        cleanUp();
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
            cleanUp();
            return -1;
        }
    }

    glewInit();

    // IDs for our vertex array and (2) vertex buffer objects
    GLuint vao, vbo[2];

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBOs
    glGenBuffers(2, vbo);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, teapot.verticesData.size() * sizeof(GLfloat), teapot.verticesData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &(teapot.verticesData));
    glEnableVertexAttribArray(0);

    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, teapot.normalsData.size() * sizeof(GLfloat), teapot.normalsData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, &(teapot.normalsData));
    glEnableVertexAttribArray(1);

    // // TexCoord buffer
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    // glBufferData(GL_ARRAY_BUFFER, teapot.polygonCount() * 2 * sizeof(GLfloat), teapot.texCoordsData(), GL_STATIC_DRAW);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(2);

    // Quad element buffer
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , vbo[3]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER , teapot.facesCount() * sizeof(GLfloat), teapot.facesData(), GL_STATIC_DRAW);
    // glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(3);
    
    checkGlErrors();

    GLuint program = glCreateProgram();

    // Read shaders
    std::stringstream buffer;
    std::ifstream vertStream("shaders\\tutorial2.vert");
    buffer << vertStream.rdbuf();
    const GLchar* tutorial2VertSource = (const GLchar*) buffer.str().c_str();
    // const GLint tutorial2VertSize = (const GLint) buffer.str().length();

    std::ifstream fragStream("shaders\\tutorial2.frag");
    buffer << fragStream.rdbuf();
    const GLchar* tutorial2FragSource = (const GLchar*) buffer.str().c_str();
    // const GLint tutorial2FragSize = (const GLint) buffer.str().length();

    GLuint tutorial2Vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint tutorial2Frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(tutorial2Vert, 1, &tutorial2VertSource, 0);
    glShaderSource(tutorial2Frag, 1, &tutorial2FragSource, 0);

    glCompileShader(tutorial2Vert);
    glCompileShader(tutorial2Frag);

    glAttachShader(program, tutorial2Vert);
    glAttachShader(program, tutorial2Frag);

    glLinkProgram(program);

    //Set up MVP
    glm::mat4 model = glm::mat4();
    GLint uniModel = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
  		glm::vec3(0.0f, 1.0f, 0.0f)
    );

    GLint uniView = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);
    GLint uniProj = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    bool loop = true;
    while (loop) {
        pollKeyEvents();

        if (keyPressed[KEY_ESCAPE] || keyPressed[KEY_QUIT]) {
            loop = false;
        }

        glDrawArrays(GL_QUADS, 0, teapot.verticesData.size());

        SDL_GL_SwapWindow(gWindow);
    }

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    // glDetachShader(shaderprogram, vertexshader);
    // glDetachShader(shaderprogram, fragmentshader);
    // glDeleteProgram(shaderprogram);
    // glDeleteShader(vertexshader);
    // glDeleteShader(fragmentshader);
    glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(1, &vao);
    glUseProgram(0);
    
    delete &teapot;

    cleanUp();
	return 0;
}
