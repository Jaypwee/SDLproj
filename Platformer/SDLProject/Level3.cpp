#include "Level3.h"
#define LEVEL3_WIDTH 19
#define LEVEL3_HEIGHT 8

#define ENEMY_COUNT 1
unsigned int level3_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5.0f, 0.0f, 0);
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
    
    state.enemies = new Entity[1];
    GLuint enemyTextureID = Util::LoadTexture("enemy.png");
    state.enemies[0].position = glm::vec3(14, 0, 0);
    state.enemies[0].leftBound = 2;
    state.enemies[0].rightBound = 19;
    state.enemies[0].speed = 2.5f;
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].acceleration = glm::vec3(0, -9.8f, 0);
    state.enemies[0].movement = glm::vec3(-1, 0, 0);
    
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    state.enemies[0].Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    if (state.enemies[0].isDead && !state.player->isDead) {
        Util::DrawText(program, fontTextureID, "You Win!!", 0.4f, 0.004f, glm::vec3(-2 + state.player->position.x, -2, 0));
        state.player->speed = 0;
    } else {
        state.enemies[0].Render(program);
    }
}
