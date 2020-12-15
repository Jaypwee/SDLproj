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
#include <SDL_mixer.h>

enum EntityType { PLAYER, PLATFORM, ENEMY, LASER };

enum AIType { PATROL, YPATROL, SPEEDO };

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    float speed;
    
    float width = 1;
    float height = 1;
    
    bool isDead = false;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    float leftBound = 0.0f;
    float rightBound = 0.0f;
    
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBot = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionX(Entity *objects, int objectCount);
    void CheckCollisionY(Entity *objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
    void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map, Mix_Chunk *crashSound);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player);
    void AIPatrol();
    void AIPatrolY();
    void AISpeedo();
};
