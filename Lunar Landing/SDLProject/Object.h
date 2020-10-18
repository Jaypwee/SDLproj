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

class Object {
public:
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    float width = 2.0f;
    float height = 2.0f;
    
    glm::vec3 position;
    
    Object();
    
    
    void Update(float deltaTime);
    void Render(ShaderProgram *program, float vertices[]);
};
