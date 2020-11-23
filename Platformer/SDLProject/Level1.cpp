#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define ENEMY_COUNT 2
unsigned int level1_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level1::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3.0f, 0.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.8f, 0);
    state.player->speed = 2.2f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->height = 1.0f;
    state.player->jumpPower = 6.5f;
    
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
    
    state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("enemy.png");
    
    state.enemies[0].position = glm::vec3(11, -2.0f, 0);
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].jumpPower = 2.0f;
    state.enemies[1].position = glm::vec3(6, -2.0f, 0);
    state.enemies[1].aiType = WALKER;
    state.enemies[1].movement = glm::vec3(-1, 0, 0);
    state.enemies[1].leftBound = 2;
    state.enemies[1].rightBound = 7;
    state.enemies[1].speed = 1.6f;
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].acceleration = glm::vec3(0, -9.8f, 0);
        state.enemies[i].isDead = false;
    }
}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    }
}
void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    int deadCount = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
        if (state.enemies[i].isDead) {
            deadCount++;
        }
    }
    
    if (deadCount == 2) state.nextScene = 2;
    
}
