#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameStart = false;
bool gameOver = false;

GLuint fontTextureID;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[2];

Mix_Music *music;
Mix_Chunk *crashSound;

int life = 3;
float leftBoundary = 0;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
    leftBoundary = 0;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Galactica", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("edm-music.mp3");
    Mix_PlayMusic(music, -1);
    crashSound = Mix_LoadWAV("crash.wav");
    
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTextureID = Util::LoadTexture("font1.png");
    
    sceneList[0] = new MainMenu();
    sceneList[1] = new Level1();

    SwitchToScene(sceneList[0]);
    
}


void ProcessInput() {

    if (gameStart) currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        break;
                    case SDLK_RETURN:
                        if (!gameStart || gameOver) {
                            SwitchToScene(sceneList[1]);
                            gameStart = true;
                            gameOver = false;
                        }
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] && !gameOver && gameStart && currentScene->state.player->position.x > 0.3f + leftBoundary) {
        currentScene->state.player->movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && !gameOver && gameStart && currentScene->state.player->position.x < 9.7 + leftBoundary) {
        currentScene->state.player->movement.x = 1.0f;
    }
    if (keys[SDL_SCANCODE_UP] && !gameOver && gameStart) {
        currentScene->state.player->movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && !gameOver && gameStart) {
        currentScene->state.player->movement.y = -1.0f;
    }
    
    if (gameStart) {
        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
        }
    }
    

}

#define FIXED_TIMESTEP 0.0166666f
#define SPEED_CONSTANT 0.4f
float lastTicks = 0.0f;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    
    while (deltaTime >= FIXED_TIMESTEP) {
        if (gameStart && !gameOver) {
            leftBoundary += SPEED_CONSTANT * FIXED_TIMESTEP;
        }
        currentScene->Update(FIXED_TIMESTEP, crashSound);
        
        deltaTime -= FIXED_TIMESTEP;
    }

    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
//    if (gameStart) {
//        currentScene->state.player->acceleration = glm::vec3(0, -4.8f, 0);
////        for (int i = 0; i < ENEMY_COUNT; i++) {
////            state.enemies[i].acceleration = glm::vec3(0, -4.8f, 0);
////        }
////        state.enemies[1].speed = 1.0f;
////        state.lasers[0].isActive = true;
//    }
//
////    if (state.lasers[0].isActive) {
////        state.lasers[0].speed = 1.3f;
////        state.lasers[0].movement = glm::vec3(-1, 0, 0);
////        state.lasers[0].Update(deltaTime, state.player, state.lasers, state.enemies, state.map);
////    }
////
////    for (int i = 0; i < ENEMY_COUNT; i++) {
////        state.enemies[i].Update(deltaTime, state.player, state.lasers, state.enemies, state.map);
////    }
//    currentScene->state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
    viewMatrix = glm::mat4(1.0f);

    if (gameStart) {
        if (leftBoundary < 70.5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5 - leftBoundary, 3.75, 0));
        } else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-75.5, 3.75, 0));
        }
        
    }

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);

    if (gameStart && currentScene->state.player->isDead) {
        gameOver = true;
        currentScene->state.player->isActive = false;
        Util::DrawText(&program, fontTextureID, "You LOSE", 0.4f, 0.004f, glm::vec3(4 + leftBoundary, -2.75f, 0));
        Util::DrawText(&program, fontTextureID, "Press Return To Try Again", 0.3f, 0.002f, glm::vec3(2 + leftBoundary,-4,0));
    }
    
    else if (gameStart && currentScene->state.player->position.x > 78) {
        gameOver = true;
        currentScene->state.player->isActive = false;
        Util::DrawText(&program, fontTextureID, "You WIN", 0.4f, 0.004f, glm::vec3(73, -2.75f, 0));
        Util::DrawText(&program, fontTextureID, "Press Return To Try Again", 0.3f, 0.002f, glm::vec3(71.5,-4,0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        
//        Render();
    }
    
    Shutdown();
    return 0;
}
