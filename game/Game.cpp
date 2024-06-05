#include "Game.hpp"
#include <chrono>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
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

void update_player_from_input(Player& player, GameInput& input) {
  float running_duration = 1.0f;

  if (input.buttons[GameInput::ButtonUp].is_down) {
  std::cout << input.buttons[GameInput::ButtonUp].duration() << std::endl;
    if (input.buttons[GameInput::ButtonUp].duration() >= running_duration) {
      player.velocity = glm::normalize(player.camera->direction) * 10.0f;
    }
    else {
      player.velocity = glm::normalize(player.camera->direction) * 1.0f;
    }
    player.acceleration = 0.0f;
  }
  else if (!input.buttons[GameInput::ButtonUp].is_down) {
    if (glm::length(player.velocity) >= 0) {
      player.acceleration = 10.0f;
      player.UpdateVelocity();
    }
  }
}

extern "C" {
  void game_update(Game& game, Renderer& renderer, GameInput& input, float elapsed_time) {
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
      game.player.velocity = glm::vec3(0);
      game.player.velocity_set_timestamp = chrono::high_resolution_clock::now();
      game.init = true;
    }
    update_player_from_input(game.player, input);
    if (glm::length(game.player.velocity) > 0) {
      auto time = elapsed_time;
      auto accelerator = -game.player.acceleration* glm::normalize(game.player.velocity);
      auto new_velocity = game.player.velocity + accelerator * time;
      std::cout << new_velocity.x << " " << new_velocity.y << " " << new_velocity.z << std::endl;
      auto movement = game.player.velocity * time;
      game.player.Move(movement);

      auto new_velocity_normalized = glm::normalize(new_velocity);
      auto old_velocity_normalized = glm::normalize(game.player.velocity);

      if (glm::dot(new_velocity_normalized, old_velocity_normalized) == -1) {
        game.player.velocity = glm::vec3(0);
      } else {
        game.player.velocity = new_velocity;
      }
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
