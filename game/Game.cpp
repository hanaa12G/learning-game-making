#include "Game.hpp"
#include <iostream>
#include <map>

char const* Dirt::texture_path = "/home/wsluser/Workspace/game/runtime/coarse_dirt.png";
char const* Rock::texture_path = "/home/wsluser/Workspace/game/runtime/cobblestone.png";

bool collision_check(GameObject& a, GameObject& b) {
  // ------------|
  // ------|---------------
  bool y = (a.pos.x + 0.5 < b.pos.x - 0.5 || a.pos.x - 0.5 > b.pos.x + 0.5) &&
           (a.pos.y + 0.5 < b.pos.y - 0.5 || a.pos.y - 0.5 > b.pos.y + 0.5) &&
           (a.pos.z + 0.5 < b.pos.z - 0.5 || a.pos.z - 0.5 > b.pos.z + 0.5);
  return !y;
}

extern "C" {
  void game_update(Game& game, Renderer& renderer) {
    if (!game.init) {
      game.grid = Grid(100);
      for (int i = -50; i < 50; ++i) {
        for (int j = -50; j < 50; ++j) {
          int x = i;
          int z = j;

          game.grid.at(x, 0, z) = new Dirt({x, 0, z});

          //game.objects.push_back(new Dirt({x, 0, z}));
        }
      }
      
      game.player = Player({2, 10, 2});
      game.player.camera = new Camera();
      game.player.camera->location = game.player.pos;
      game.init = true;
    }
    bool collide = false;
    for (auto object: game.grid.data) {
      if (!object) continue;
      auto dir = game.player.pos - object->pos;
      auto distance = glm::length(dir);
      if (distance > 5) {
        continue;
      }

      if (collision_check(game.player, *object)) {
        std::cout << "Collide with player " << object->pos.x << " " << object->pos.y << " " << object->pos.z << std::endl;
        collide = true;
      }
    }

    if (collide) game.player.should_move = false;
    else game.player.should_move = true;
    
    std::map<std::string, std::vector<GameObject*>> batches;
    renderer.SetActiveCamera(*game.player.camera);
    for (auto object: game.grid.data) {
      if (!object) continue;
      batches[object->Type()].push_back(object);

    }

    for (auto& batch : batches) {
      renderer.DrawBatch(batch.second);
    }
    
  }
}
