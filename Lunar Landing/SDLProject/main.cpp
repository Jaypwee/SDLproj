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
#include <vector>

#include "Object.h"
#include "Entity.h"


struct GameState {
    Entity *player;
    Object *leftWall;
    Object *rightWall;
    Object *bottom;
    Object *platform;
    Object *topObstacle;
    Object *bottomObstacle;
    Object *box1;
    Object *box2;
    
    Object *obstacles;
};

GameState state;

GLuint fontTextureID;
int flag;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath, bool isRepeat = false) {
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

    if (isRepeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

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
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 960, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 1920);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -15.0f, 15.0f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTextureID = LoadTexture("font1.png");
   
    state.obstacles = new Object[7];
    
    // Initialize Game Objects
    state.obstacles[0].textureID = LoadTexture("tile.png", true);
    state.obstacles[0].position = glm::vec3(-9.0f, 0, 0);
    state.obstacles[0].modelMatrix = glm::translate(state.obstacles[0].modelMatrix,glm::vec3(-9.0f, 0, 0));
    state.obstacles[0].height = 30.0f;
    
    state.obstacles[1].textureID = LoadTexture("tile.png", true);
    state.obstacles[1].position = glm::vec3(9.0f, 0, 0);
    state.obstacles[1].modelMatrix = glm::translate(state.obstacles[1].modelMatrix, glm::vec3(9.0f, 0, 0));
    state.obstacles[1].height = 30.0f;
    
    state.obstacles[2].textureID = LoadTexture("tile.png", true);
    state.obstacles[2].position = glm::vec3(-2.0f, -14.0f, 0);
    state.obstacles[2].modelMatrix = glm::translate(state.obstacles[2].modelMatrix, glm::vec3(-2.0f, -14.0f, 0));
    state.obstacles[2].width = 12.0f;
    
    state.platform = new Object();
    state.platform->textureID = LoadTexture("platform.png", true);
    state.platform->position = glm::vec3(6.0f, -14.0f, 0);
    state.platform->modelMatrix = glm::translate(state.platform->modelMatrix, glm::vec3(6.0f, -14.0f, 0));
    state.platform->width = 4.0f;
    
    state.obstacles[3].textureID = LoadTexture("box.png", true);
    state.obstacles[3].position = glm::vec3(-4.0f, 8.0f, 0);
    state.obstacles[3].modelMatrix = glm::translate(state.obstacles[3].modelMatrix, glm::vec3(-4.0f, 8.0f, 0));
    state.obstacles[3].width = 8.0f;
    
    state.obstacles[4].textureID = LoadTexture("box.png", true);
    state.obstacles[4].position = glm::vec3(4.0f, -6.0f, 0);
    state.obstacles[4].modelMatrix = glm::translate(state.obstacles[4].modelMatrix, glm::vec3(4.0f, -6.0f, 0));
    state.obstacles[4].width = 8.0f;
    
    state.obstacles[5].textureID = LoadTexture("box.png", true);
    state.obstacles[5].position = glm::vec3(1.0f, 1.0f, 0);
    state.obstacles[5].modelMatrix = glm::translate(state.obstacles[5].modelMatrix, glm::vec3(1.0f, 1.0f, 0));

    state.obstacles[6].textureID = LoadTexture("box.png", true);
    state.obstacles[6].position = glm::vec3(2.0f, -2.0f, 0);
    state.obstacles[6].modelMatrix = glm::translate(state.obstacles[6].modelMatrix, glm::vec3(2.0f, -2.0f, 0));
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 16.5f, 0);
    state.player->movement = glm::vec3(0);
    state.player->textureID = LoadTexture("spaceship.png");
    
    
    

}

void ProcessInput() {
    
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
                        // Some sort of action
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        
        state.player->acceleration.x = -2.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
    
        state.player->acceleration.x = 2.0f;
    }
    else {
        if (state.player->velocity.x > 0) {
            state.player->acceleration.x = -0.99f;
        } else if (state.player->velocity.x < 0) {
            state.player->acceleration.x = 0.99f;
        } else {
            state.player->acceleration.x = 0;
        }
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks; lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        flag = state.player->Update(FIXED_TIMESTEP, state.platform, state.obstacles, 7);
        deltaTime -= FIXED_TIMESTEP;
    }
   accumulator = deltaTime;
}



float wallVertices[] = { -1.0, -15.0, 1.0, -15.0, 1.0, 15.0, -1.0, -15.0, 1.0, 15.0, -1.0, 15.0 };
float bottomVertices[] = {-6.0, -1.0, 6.0, -1.0, 6.0, 1.0, -6.0, -1.0, 6.0, 1.0, -6.0, 1.0 };
float platformVertices[] = {-2.0, -1.0, 2.0, -1.0, 2.0, 1.0, -2.0, -1.0, 2.0, 1.0, -2.0, 1.0 };
float obstacleVertices[] = {-4.0, -1.0, 4.0, -1.0, 4.0, 1.0, -4.0, -1.0, 4.0, 1.0, -4.0, 1.0 };
float vertices[] = {-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player->Render(&program);
    state.obstacles[0].Render(&program, wallVertices);
    state.obstacles[1].Render(&program, wallVertices);
    state.obstacles[2].Render(&program, bottomVertices);
    state.platform->Render(&program, platformVertices);
    state.obstacles[3].Render(&program, obstacleVertices);
    state.obstacles[4].Render(&program, obstacleVertices);
    state.obstacles[5].Render(&program, vertices);
    state.obstacles[6].Render(&program, vertices);
    
    if (flag == 1) {
        DrawText(&program, fontTextureID, "Mission Failed", 1.0f, 0.1f, glm::vec3(-8.0f, 0, 0));
    }
    else if (flag == 2) {
        DrawText(&program, fontTextureID, "Mission Successful!", 1.0f, 0.1f, glm::vec3(-9.0f, 0, 0));
    }
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
