#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
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
                collidedTop = true;
            } else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBot = true;
                if (object->entityType == ENEMY) {
                    object->isDead = true;
                    object->position = glm::vec3(20.0f, 20.0f, 0);
                }
            }
        }
    }
}


void Entity::CheckCollisionX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            } else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::AIJumper() {
    if (collidedBot == true) {
        jump = true;
    }
}

void Entity::AIWalker() {
    if (position.x < -8) {
        movement = glm::vec3(1,0,0);
    } else if (position.x > 8){
        movement = glm::vec3(-1,0,0);
    }
}

void Entity::AIShooter(Entity *lasers) {
    if (lasers[0].position.x + 5 < position.x) {
        lasers[0].position = position;
        isActive = true;
    }
    
}

void Entity::AI(Entity *player, Entity *lasers) {
    switch(aiType) {
        case WALKER:
            AIWalker();
            break;
        case JUMPER:
            AIJumper();
            break;
        case SHOOTER:
            AIShooter(lasers);
            break;
    }
}



void Entity::Update(float deltaTime, Entity *player, Entity *lasers, Entity *enemies, Entity *platforms, int platformCount)
{
    if (!isActive) return;
    
    if (entityType == ENEMY) {
        AI(player, lasers);
    }
    
    collidedTop = false;
    collidedBot = false;
    collidedRight = false;
    collidedLeft = false;
    
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    
    velocity += acceleration * deltaTime;
    
    
    
    position.y += velocity.y * deltaTime;
    if (entityType == PLAYER) {
        CheckCollisionY(enemies, 3);
        if (collidedBot) {
            jump = true;
        } else if (collidedTop) {
            isDead = true;
        }
    }
    CheckCollisionY(platforms, platformCount);
    
    position.x += velocity.x * deltaTime;
    if (entityType == PLAYER) {
        CheckCollisionX(enemies, 3);
        if (collidedRight || collidedLeft) {
            isDead = true;
        }
    }
    CheckCollisionX(platforms, platformCount);
    
    if (entityType == PLAYER) {
        if (CheckCollision(&lasers[0])) {
            isDead = true;
        }
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isDead) return;
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
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
