#include <stdio.h>
#include <string>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>

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
// The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;
// Images to load
SDL_Surface* surfaces[TEXTURE_ENUM_LENGTH];

// The window renderer
SDL_Renderer* gRenderer = nullptr;
// Current displayed texture
SDL_Texture* gTextures[TEXTURE_ENUM_LENGTH];

// OpenGL Context
SDL_GLContext glContext;

unsigned long int keyPressed[KEY_ENUM_LENGTH];
unsigned long int keysPressed = 0;

void cleanUp() {
    // Deallocate surface
    for (int i = 0; i < TEXTURE_ENUM_LENGTH; i++) {
        SDL_DestroyTexture(gTextures[i]);
        gTextures[i] = nullptr;
    }

    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture(std::string path) {
    SDL_Texture* texture;
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else {
		// Convert texture to surface pixels
		texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
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

int main(int argc, char* args[]) {
    // Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        cleanUp();
        return 0;
	}

    // Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", 1, 31, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (!gWindow) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        cleanUp();
        return 0;
    }

    // Create renderer for window
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        cleanUp();
        return 0;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        cleanUp();
        return 0;
    }

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    // Load media
    gTextures[TEXTURE_DEFAULT] = loadTexture("img/background.jpg");
    gTextures[TEXTURE_UP] = loadTexture("img/up.bmp");
    gTextures[TEXTURE_DOWN] = loadTexture("img/down.bmp");
    gTextures[TEXTURE_LEFT] = loadTexture("img/left.bmp");
    gTextures[TEXTURE_RIGHT] = loadTexture("img/right.bmp");
    for (int i = 0; i < TEXTURE_ENUM_LENGTH; i++) {
        if (!gTextures[i]) {
            cleanUp();
            return 0;
        }
    }

    SDL_Texture* currentTexture = gTextures[TEXTURE_DEFAULT];
    bool quit = false;
    while (!quit) {
        Key lastPressed = pollKeyEvents();

        if (keyPressed[KEY_ESCAPE] || keyPressed[KEY_QUIT]) {
            quit = true;
        }

        switch (lastPressed) {
            case NONE:
                currentTexture = gTextures[TEXTURE_DEFAULT];
                break;
            case KEY_UP:
                currentTexture = gTextures[TEXTURE_UP];
                break;
            case KEY_DOWN:
                currentTexture = gTextures[TEXTURE_DOWN];
                break;
            case KEY_LEFT:
                currentTexture = gTextures[TEXTURE_LEFT];
                break;
            case KEY_RIGHT:
                currentTexture = gTextures[TEXTURE_RIGHT];
                break;
            default:
                break;
        }
        
        SDL_RenderCopy(gRenderer, currentTexture, NULL, NULL);

        if (keyPressed[KEY_SPACE]) {
            SDL_SetRenderDrawColor(gRenderer, 0xAD, 0x73, 0xFF, 0xFF);
            SDL_Rect rect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            SDL_RenderFillRect(gRenderer, &rect);
            SDL_Rect outlineRect = { SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3 };
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xD0, 0x00, 0xFF);        
            SDL_RenderDrawRect(gRenderer, &outlineRect);
        }

        SDL_RenderPresent(gRenderer);
    }

    cleanUp();
	return 0;
}
