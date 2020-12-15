#include "Level1.h"
#define LEVEL1_WIDTH 81
#define LEVEL1_HEIGHT 8

#define ENEMY_COUNT 3
unsigned int level1_data[] =
{
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 0, 0, 0, 0, 56, 56, 56, 56, 56, 56, 56, 56, 0, 0, 0, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 56, 56, 56, 56, 56, 56, 56, 56, 0, 56, 0, 0, 0, 0, 56, 0, 0, 0, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 0, 0, 0, 0, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 56, 56, 56, 56, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 56, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 0, 56, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 56, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 0, 0, 0, 0, 0, 0, 0, 56, 56, 56, 0, 0, 0, 0, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 56, 56, 56, 0, 56, 0, 0, 0, 56, 0, 0, 0, 56, 0, 0, 0, 0, 56, 0, 56, 0, 56, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 0, 0, 0, 0, 0, 0, 0, 56, 56, 56, 0, 0, 0, 0, 0, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 56, 0, 56, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
};

void Level1::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 8);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3.0f, -3.75f, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.2f;
    state.player->isActive = true;
    state.player->textureID = Util::LoadTexture("player.png");
    
    state.enemies = new Entity[ENEMY_COUNT];
    
    state.enemies[0].position = glm::vec3(16, -4.0f, 0);
    state.enemies[0].speed = 1.0f;
    state.enemies[0].movement = glm::vec3(1,0,0);
    state.enemies[0].aiType = PATROL;
    state.enemies[0].leftBound = 14.5f;
    state.enemies[0].rightBound = 22.5f;
    state.enemies[1].position = glm::vec3(29, -1.0f, 0);
    state.enemies[1].aiType = YPATROL;
    state.enemies[1].movement = glm::vec3(0, -1, 0);
    state.enemies[1].speed = 1.1f;
    state.enemies[1].leftBound = -1;
    state.enemies[1].rightBound = -6;
    
    state.enemies[2].position = glm::vec3(71, -1.0f, 0);
    state.enemies[2].speed = 18.5f;
    state.enemies[2].aiType = SPEEDO;
    state.enemies[2].leftBound = 68.5;
    state.enemies[2].rightBound = 73.5;
    state.enemies[2].movement = glm::vec3(-1,0,0);
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = Util::LoadTexture("enemy.png");
        state.enemies[i].isDead = false;
    }
}
void Level1::Update(float deltaTime, Mix_Chunk *crashSound) {
    state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map, crashSound);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map, crashSound);
    }
}
void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}
