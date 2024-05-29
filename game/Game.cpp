#include "Game.hpp"
#include <iostream>
#include <map>

char const* Dirt::texture_path = "/home/wsluser/Workspace/game/runtime/coarse_dirt.png";
char const* Rock::texture_path = "/home/wsluser/Workspace/game/runtime/cobblestone.png";

extern "C" {
  void game_update(Game& game, Renderer& renderer) {
    if (!game.init) {
      for (int i = -50; i < 50; ++i) {
        for (int j = -50; j < 50; ++j) {
          int x = i;
          int z = j;

          game.objects.push_back(new Dirt({x, 0, z}));
        }
      }
      
      game.player = Player({2, 0, 2});
      game.player.camera = new Camera();
      game.init = true;
    }
    
    std::map<std::string, std::vector<GameObject*>> batches;
    renderer.SetActiveCamera(*game.player.camera);
    for (auto object: game.objects) {
      batches[object->Type()].push_back(object);

    }

    for (auto& batch : batches) {
      renderer.DrawBatch(batch.second);
    }
    
  }
}
