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
glm::mat4 viewMatrix, ballMatrix, leftPaddleMatrix, rightPaddleMatrix, projectionMatrix;

GLuint pongTextureID;

glm::vec3 movement = glm::vec3(0, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
glm::vec3 left_paddle_position = glm::vec3(-9.25f, 0, 0);
glm::vec3 left_paddle_movement = glm::vec3(0, 0, 0);
glm::vec3 right_paddle_position = glm::vec3(9.25f, 0, 0);
glm::vec3 right_paddle_movement = glm::vec3(0, 0, 0);

float velocity = 7.5f;
float ballDimension = 0.25f;
float paddleHeight = 4.5f;
float paddleWidth = 0.25f;

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
    if (gameStart) {
        if (keys[SDL_SCANCODE_W] && left_paddle_position.y + paddleHeight < 10.0f) {
            left_paddle_movement.y = 1.0f;
        }
        else if (keys[SDL_SCANCODE_S] && left_paddle_position.y - paddleHeight > -10.0f) {
            left_paddle_movement.y = -1.0f;
        }
        if (keys[SDL_SCANCODE_UP] && right_paddle_position.y + paddleHeight < 10.0f ) {
            right_paddle_movement.y = 1.0f;
        }
        else if (keys[SDL_SCANCODE_DOWN] && right_paddle_position.y - paddleHeight > -10.0f) {
            right_paddle_movement.y = -1.0f;
        }
    }
    

}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    if (gameStart) {
        ball_movement = movement;
        ball_movement = glm::normalize(ball_movement);
    }
    
    ballMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::mat4(1.0f);
    rightPaddleMatrix = glm::mat4(1.0f);
    
    ball_position += ball_movement * velocity * deltaTime;
    if (ball_position.y > 7.5f - (ballDimension / 2)) {
        movement.y = -1.0f;
    } else if (ball_position.y < -7.5f + (ballDimension / 2)) {
        movement.y = 1.0f;
    }
    if (ball_position.x > 9.125f) {
        if ((right_paddle_position.y + (paddleHeight / 2) > ball_position.y) && (right_paddle_position.y - (paddleHeight / 2) < ball_position.y)) {
            movement.x = -1.0f;
        }
    }
    else if (ball_position.x < -9.125f) {
        if ((left_paddle_position.y + (paddleHeight / 2) > ball_position.y) && (left_paddle_position.y - (paddleHeight / 2) < ball_position.y)){
            movement.x = 1.0f;
        }
    }
    
    ballMatrix = glm::translate(ballMatrix, ball_position);

    if (ball_position.x < -9.3f || ball_position.x > 9.3f) {
        gameStart = false;
        ballMatrix = glm::mat4(1.0f);
        ball_position = glm::vec3(0.0f);
        ball_movement = glm::vec3(0.0f);
        velocity = 7.0f;
        left_paddle_position.y = 0.0f;
        right_paddle_position.y = 0.0f;
        left_paddle_movement.y = 0.0f;
        right_paddle_movement.y = 0.0f;
    }
    left_paddle_position += left_paddle_movement * velocity * deltaTime;
    leftPaddleMatrix = glm::translate(leftPaddleMatrix, left_paddle_position);
    right_paddle_position += right_paddle_movement * velocity * deltaTime;
    rightPaddleMatrix = glm::translate(rightPaddleMatrix, right_paddle_position);
    

//    Game becomes faster and faster!
    velocity *= 1.0001;
}

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
