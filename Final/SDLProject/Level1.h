#include "Scene.h"
class Level1 : public Scene {

public:
 void Initialize() override;
 void Update(float deltaTime, Mix_Chunk *crashSound) override;
 void Render(ShaderProgram *program) override;
};
