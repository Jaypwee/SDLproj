#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    
    return false;
}

void Entity::CheckCollisionY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTopEnemy = true;
            } else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBotEnemy = true;
                if (object->entityType == ENEMY) {
                    object->isDead = true;
                    object->position = glm::vec3(20.0f, 20.0f, 0);
                }
            }
        }
    }
}


void Entity::CheckCollisionX(Entity *objects, int objectCount) {
    collidedLeft = false;
    collidedRight = false;
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            } else if (velocity.x <= 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::AIWalker() {
    if (position.x < leftBound) {
        movement = glm::vec3(1,0,0);
    } else if (position.x > rightBound){
        movement = glm::vec3(-1,0,0);
    }
}

void Entity::AI(Entity *player) {
    switch(aiType) {}
}

void Entity::CheckCollisionsY(Map *map)
{
     // Probes for tiles
     glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
     glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
     glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

     glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
     glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
     glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

     float penetration_x = 0;
     float penetration_y = 0;
     if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
         position.y -= penetration_y;
         velocity.y = 0;
         collidedTop = true;
     }
     else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
         position.y -= penetration_y;
         velocity.y = 0;
         collidedTop = true;
     }
     else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
         position.y -= penetration_y;
         velocity.y = 0;
         collidedTop = true;
     }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBot = true;
     }
     else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBot = true;
     }
     else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBot = true;
     }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    collidedLeft = false;
    collidedRight = false;
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

#define SPEED_CONSTANT 0.4f
void Entity::Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map)
{
    if (!isActive) return;
    
    if (entityType == ENEMY) {
        AI(player);
    }
    
    collidedTop = false;
    collidedBot = false;
    collidedRight = false;
    collidedLeft = false;
    collidedBotEnemy = false;
    
    velocity = movement * speed;
    velocity.x += SPEED_CONSTANT;
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    if (entityType == PLAYER) {
        if (position.y < -8.5f) {
            isDead = true;
            return;
        }
        CheckCollisionY(enemies, enemyCount);
        if (collidedBotEnemy || collidedTopEnemy) {
            isDead = true;
        }
    }
    
    position.x += velocity.x * deltaTime; // Move on X
    
    CheckCollisionsX(map);
    if (entityType == PLAYER) {
        CheckCollisionX(enemies, enemyCount);
        if (collidedRight || collidedLeft) {
            isDead = true;
            return;
        }
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::Render(ShaderProgram *program) {
    if (isDead) return;
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
