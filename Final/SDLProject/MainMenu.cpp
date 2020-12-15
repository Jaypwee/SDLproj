#include "MainMenu.h"

void MainMenu::Initialize() {
    state.nextScene = -1;
}
void MainMenu::Update(float deltaTime, Mix_Chunk *crashSound) {
    
}

void MainMenu::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Galactica", 0.4f, 0.004f, glm::vec3(-2, 0, 0));
    Util::DrawText(program, fontTextureID, "Press Enter", 0.4f, 0.004f, glm::vec3(-2, -1, 0));
}
