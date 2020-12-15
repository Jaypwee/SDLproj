#include "Scene.h"
class MainMenu : public Scene {

public:
 void Initialize() override;
 void Update(float deltaTime, Mix_Chunk *crashSound) override;
 void Render(ShaderProgram *program) override;
};
