#pragma once
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
#include "Map.h"

enum EntityType { PLAYER, PLATFORM, ENEMY, LASER };

enum AIType { WALKER, JUMPER, SHOOTER };

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;
    
    float width = 1;
    float height = 1;
    
    bool jump = false;
    bool isTouchingGround = false;
    bool isDead = false;
    float jumpPower = 0;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    float leftBound = 0.0f;
    float rightBound = 0.0f;
    
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBot = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedBotEnemy = false;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionX(Entity *objects, int objectCount);
    void CheckCollisionY(Entity *objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
    void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player);
    void AIWalker();
    void AIJumper();
};
