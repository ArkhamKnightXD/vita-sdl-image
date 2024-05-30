#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_GameController* controller = NULL;

SDL_Rect rectangle = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 32, 32};

const int SPEED = 600; 
const int FRAME_RATE = 60;
 
SDL_Texture* sprite;

SDL_Texture* LoadSprite(const char* file, SDL_Renderer* renderer)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, file);
    return texture;
}

void RenderSprite(SDL_Texture* sprite, SDL_Renderer* renderer, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(sprite, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(renderer, sprite, NULL, &dest);
}

void quitGame() {
    SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void handleEvents() {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {
            
            quitGame();
            exit(0);
        }
    }
}
 
void update(float deltaTime) {

    SDL_GameControllerUpdate();

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && rectangle.y > 0) {
        rectangle.y -= SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && rectangle.y < SCREEN_HEIGHT - rectangle.h) {
        rectangle.y += SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && rectangle.x > 0) {
        rectangle.x -= SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && rectangle.x < SCREEN_WIDTH - rectangle.w) {
        rectangle.x += SPEED * deltaTime;
    }
}

void render() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    RenderSprite(sprite, renderer, 30, 30);

    SDL_RenderFillRect(renderer, &rectangle);

    SDL_RenderPresent(renderer);  
}

void capFrameRate(Uint32 frameStartTime) {

    Uint32 frameTime = SDL_GetTicks() - frameStartTime;
    
    if (frameTime < 1000 / FRAME_RATE) {
        SDL_Delay(1000 / FRAME_RATE - frameTime);
    }
}

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        return -1;
    }

    if ((window = SDL_CreateWindow("RedRectangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL) {
        return -1;
    }

    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
        return -1;
    }

    sprite = LoadSprite("sprites/sprite.png", renderer);

    if (SDL_NumJoysticks() < 1) {
        printf("No game controllers connected!\n");
        return -1;
    } 
    else {

        controller = SDL_GameControllerOpen(0);
        if (controller == NULL) {

            printf("Unable to open game controller! SDL Error: %s\n", SDL_GetError());
            return -1;
        }
    }

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime;
    float deltaTime;

    while (1) {

        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        handleEvents();
        update(deltaTime);
        render();

        capFrameRate(currentFrameTime);
    }

    quitGame();
    return 0;
}