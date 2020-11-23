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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 22


#define UPPER_PLATFORMS 12
#define ENEMY_COUNT 3


struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
    Entity *lasers;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameStart = false;

GLuint fontTextureID;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    state.platforms = new Entity[PLATFORM_COUNT + UPPER_PLATFORMS];
    fontTextureID = LoadTexture("font1.png");
    GLuint platformTextureID = LoadTexture("tile.png");
//    Bottom platforms
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-10 + i, -6.25f, 0);
        state.platforms[i].Update(0, NULL, NULL, NULL, NULL, 0);
    }
    
//      Higher platforms
    
    for (int i = PLATFORM_COUNT; i < PLATFORM_COUNT + UPPER_PLATFORMS; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-2 + i - PLATFORM_COUNT, -2.25f, 0);
        state.platforms[i].Update(0, NULL, NULL, NULL, NULL, 0);
    }
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("enemy.png");
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
    }
    
    state.enemies[0].position = glm::vec3(-4, -5.0f, 0);
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].jumpPower = 3.0f;
    state.enemies[1].position = glm::vec3(3, -5.0f, 0);
    state.enemies[1].aiType = WALKER;
    state.enemies[1].movement = glm::vec3(-1, 0, 0);
    state.enemies[2].position = glm::vec3(5, -1, 0);
    state.enemies[2].aiType = SHOOTER;
    
    state.lasers = new Entity[1];
    GLuint laserTextureID = LoadTexture("laser.png");
    state.lasers[0].textureID = laserTextureID;
    state.lasers[0].entityType = LASER;
    state.lasers[0].isActive = false;
//    state.lasers[0].position.y = -5.0f;
    state.lasers[0].position = glm::vec3(4.5f, -1, 0);
    
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-8.0f, -5.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0);
    state.player->speed = 1.8f;
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->height = 1.0f;
    state.player->jumpPower = 8.0f;
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
 
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
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
                        if (state.player->collidedBot){
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        gameStart = true;
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        gameStart = true;
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    if (gameStart) {
        state.player->acceleration = glm::vec3(0, -5.8f, 0);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].acceleration = glm::vec3(0, -5.8f, 0);
        }
        state.enemies[1].speed = 1.0f;
        state.lasers[0].isActive = true;
    }
    
    if (state.lasers[0].isActive) {
        state.lasers[0].speed = 1.3f;
        state.lasers[0].movement = glm::vec3(-1, 0, 0);
        state.lasers[0].Update(deltaTime, state.player, state.lasers, state.enemies, state.platforms, PLATFORM_COUNT + UPPER_PLATFORMS);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.lasers, state.enemies, state.platforms, PLATFORM_COUNT + UPPER_PLATFORMS);
    }
    state.player->Update(deltaTime, state.player, state.lasers, state.enemies, state.platforms, PLATFORM_COUNT + UPPER_PLATFORMS);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT + UPPER_PLATFORMS; i++) {
        state.platforms[i].Render(&program);
    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    if (state.lasers[0].isActive) {
        state.lasers[0].Render(&program);
    }
    state.player->Render(&program);
    if (state.player->isDead) {
        DrawText(&program, fontTextureID, "You Lose", 1.0f, 0.1f, glm::vec3(-6.0f, 0, 0));
    }
    int enemyCount = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!state.enemies[i].isDead){
            enemyCount ++;
        }
    }
    if (enemyCount == 0) {
        DrawText(&program, fontTextureID, "You Win", 1.0f, 0.1f, glm::vec3(-6.0f, 0, 0));

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
    }
    
    Shutdown();
    return 0;
}
