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
 
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameStart = false;
float lastTicks = 0.0f;

ShaderProgram program;
glm::mat4 viewMatrix, ballMatrix, leftPaddleMatrix, rightPaddleMatrix, earthMatrix, sunMatrix, jupiterMatrix, projectionMatrix;

GLuint pongTextureID;

glm::vec3 movement = glm::vec3(0, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
glm::vec3 left_paddle_position = glm::vec3(0, 0, 0);
glm::vec3 left_paddle_movement = glm::vec3(0, 0, 0);
glm::vec3 right_paddle_position = glm::vec3(0, 0, 0);
glm::vec3 right_paddle_movement = glm::vec3(0, 0, 0);

float velocity = 5.0f;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("HW1 - An Orbit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::mat4(1.0f);
    rightPaddleMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::translate(leftPaddleMatrix, glm::vec3(-9.25f, 0.0f, 0.0f));
    rightPaddleMatrix = glm::translate(rightPaddleMatrix, glm::vec3(9.25f, 0.0f, 0.0f));
    
    sunMatrix = glm::mat4(1.0f);
    sunMatrix = glm::scale(sunMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
    earthMatrix = glm::mat4(1.0f);
    earthMatrix = glm::translate(earthMatrix, glm::vec3(0.0f, -2.5f, 0.0f));
    jupiterMatrix = glm::mat4(1.0f);
    jupiterMatrix = glm::translate(jupiterMatrix, glm::vec3(0.0f, -5.0f, 0.0f));
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    pongTextureID = LoadTexture("pong_ball.png");
    movement = glm::vec3(1.0f, 1.0f, 0.0f);
}

void ProcessInput() {
    
    left_paddle_movement = glm::vec3(0);
    right_paddle_movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
//                         Start the game
                        gameStart = true;
                        break;
            }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) {
        left_paddle_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        left_paddle_movement.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        right_paddle_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        right_paddle_movement.y = -1.0f;
    }

}
int sunTicks = 0;
//
//void Update() {
//    float ticks = (float)SDL_GetTicks() / 1000.0f;
//    float deltaTime = ticks - lastTicks;
//    lastTicks = ticks;
//    if (sunTicks < 200) {
//        sunTicks += 1;
//    } else if (sunTicks < 236) {
//        sunMatrix = glm::rotate(sunMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//        sunTicks += 1;
//    } else {
//        sunTicks = 0;
//    }
////    modelMatrix = glm::mat4(1.0f);
//    earthMatrix = glm::rotate(earthMatrix, glm::radians(0.2f), glm::vec3(0.0f, 0.0f, 1.0f));
//    earthMatrix = glm::translate(earthMatrix, glm::vec3(0.6f * deltaTime, 0.0f, 0.0f));
//
//    jupiterMatrix = glm::rotate(jupiterMatrix, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));
//    jupiterMatrix = glm::translate(jupiterMatrix, glm::vec3(0.5f * deltaTime, 0.0f, 0.0f));
//}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    if (gameStart) {
        ball_movement = movement;
        ball_movement = glm::normalize(ball_movement);
    }
    
    ball_position = ball_movement * velocity * deltaTime;
    ballMatrix = glm::translate(ballMatrix, ball_position);

    left_paddle_position = left_paddle_movement * velocity * deltaTime;
    leftPaddleMatrix = glm::translate(leftPaddleMatrix, left_paddle_position);
    
    right_paddle_position = right_paddle_movement * velocity * deltaTime;
    rightPaddleMatrix = glm::translate(rightPaddleMatrix, right_paddle_position);
}

//void DrawEarth() {
//    program.SetModelMatrix(earthMatrix);
//    glBindTexture(GL_TEXTURE_2D, earthTextureID);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//void DrawSun() {
//    program.SetModelMatrix(sunMatrix);
//    glBindTexture(GL_TEXTURE_2D, sunTextureID);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//void DrawJupiter() {
//    program.SetModelMatrix(jupiterMatrix);
//    glBindTexture(GL_TEXTURE_2D, jupiterTextureID);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}

void DrawBall() {
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, pongTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawLeft() {
    program.SetModelMatrix(leftPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, pongTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawRight() {
    program.SetModelMatrix(rightPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, pongTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    DrawBall();
    
    float verticesPaddle[] = { -0.25, -2.25, 0.25, -2.25, 0.25, 2.25, -0.25, -2.25, 0.25, 2.25, -0.25, 2.25 };

    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesPaddle);
    glEnableVertexAttribArray(program.positionAttribute);
    
//    DrawEarth();
//    DrawSun();
//    DrawJupiter();
    DrawLeft();
    DrawRight();
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
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
