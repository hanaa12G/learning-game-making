#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ErrorOr.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "OpenGLCamera.hpp"
#include "game/Game.hpp"


ErrorOr<ShaderProgram> SetupShaderProgram(char const* vertex_shader_path, char const* fragment_shader_path) {
  auto shader_1 = TRY(ShaderProgram::create());
  TRY(shader_1.UseVertexShader(vertex_shader_path));
  TRY(shader_1.UseFragmentShader(fragment_shader_path));

  return std::move(shader_1);

}
struct OpenGLRenderer : public Renderer {

  virtual void SetActiveCamera(Camera& camera) override {
    this->camera.direction = camera.direction;
    this->camera.location = camera.location;
    this->camera.up = camera.up;
  }

  virtual void Draw(GameObject& obj) override {
    if (drawable_resources.count(obj.Type()) == 0) {
      LoadResource(obj);
    }

    DrawCube(obj);
  }

  virtual void DrawBatch(std::vector<GameObject*>& obj) override {
    if (obj.empty()) return;
    if (drawable_resources.count(BatchNameOf(*obj.front())) == 0) {
      LoadBatchResource(*obj.front());
    }
    auto& resource = drawable_resources[BatchNameOf(*obj.front())];
    resource.program.Use();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resource.texture);
    

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);

    resource.program.SetUniformMatrix("projection", projection);
    resource.program.SetUniformMatrix("view", camera.GetViewMatrix());
    std::vector<glm::mat4> transform_list;
    int count = 0;
    while (count < obj.size()) {
      int i = 0;
      for (i = 0; i < 1000 && count + i < obj.size(); ++i) {
        GameObject* game_object = obj[count + i];

        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, game_object->pos);
        std::string name = std::string("transform") + "[" + std::to_string(i) + "]";
        resource.program.SetUniformMatrix(name.c_str(), trans);
      }
      //std::cout << "Draw " << i << "object" << std::endl;
      glBindVertexArray(resource.vao);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 36, i);
      count += i;
    }

  }

  void DrawCube(GameObject& obj) {
    auto& resource = drawable_resources[obj.Type()];
    resource.program.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resource.texture);


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);
    resource.program.SetUniformMatrix("projection", projection);
    resource.program.SetUniformMatrix("view", camera.GetViewMatrix());
    
    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, obj.pos);

    resource.program.SetUniformMatrix("transform", trans);

    glBindVertexArray(resource.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  void LoadBatchResource(GameObject& obj) {
    auto& resource = drawable_resources[BatchNameOf(obj)];
    resource.program = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader_batch.glsl",
       "/home/wsluser/Workspace/game/runtime/fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int image_width, image_height, nr_channel;
    unsigned char* image_data = stbi_load(obj.Texture(), &image_width, &image_height, &nr_channel, 0);

    if (image_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      fprintf(stderr, "Can't open image\n");
    }
    stbi_image_free(image_data);
    resource.texture =  texture;

    auto vertices = ModelConstruction::Cube(
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {1.0, 0.0, 1.0},
      {0.0, 0.0, 1.0},
      {0.0, 1.0, 0.0},
      {1.0, 1.0, 0.0},
      {1.0, 1.0, 1.0},
      {0.0, 1.0, 1.0}
    );
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    


    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;
  }

  void LoadResource(GameObject& obj) {
    auto& resource = drawable_resources[obj.Type()];
    resource.program = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader.glsl",
      "/home/wsluser/Workspace/game/runtime/fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int image_width, image_height, nr_channel;
    unsigned char* image_data = stbi_load(obj.Texture(), &image_width, &image_height, &nr_channel, 0);

    if (image_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      fprintf(stderr, "Can't open image\n");
    }
    stbi_image_free(image_data);
    resource.texture =  texture;

    auto vertices = ModelConstruction::Cube(
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {1.0, 0.0, 1.0},
      {0.0, 0.0, 1.0},
      {0.0, 1.0, 0.0},
      {1.0, 1.0, 0.0},
      {1.0, 1.0, 1.0},
      {0.0, 1.0, 1.0}
    );
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    


    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;

  }


  std::string BatchNameOf(GameObject& obj) {
    return std::string(obj.Type()) + "_batch";
  }


  struct DrawableResource {
    ShaderProgram program;
    GLuint texture;
    std::vector<float> model_data;
    GLuint vao;
  };

  std::map<std::string, DrawableResource> drawable_resources;
  OpenGLCamera camera;
  //ShaderProgram shader_program;
};

void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userparam) {
  fprintf(stderr, "GL: message=%s\n", message);
}

int initGL(GLuint& program) {
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(OpenGLDebugMessageCallback, 0);
  return 0;
}

int init(SDL_Window **window, GLuint& shader_program) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Can't init SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


  *window =
    SDL_CreateWindow("game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Can't create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(*window);
  glewExperimental = GL_TRUE;
  GLenum glerror = glewInit();
  if (glerror != GLEW_OK) {
    fprintf(stderr, "Initialize GLEW %s\n", glewGetErrorString(glerror));
    return 1;
  }

  if (SDL_GL_SetSwapInterval(1) < 0) {
    fprintf(stderr, "Set vsync: %s\n", SDL_GetError());
  }

  if (0 != initGL(shader_program)) {
    fprintf(stderr, "Initialize OpenGL failed\n");
    return 1;
  }

  return 0;
}




int main() {

  SDL_Window *window;
  GLuint shader_program = 0;
  if (0 != init(&window, shader_program)) {
    return 1;
  }

  SDL_Event e;
  bool quit = false;

  float degree = 0.0f;
  float move_x = 0.0f;
  float move_y = 0.0f;

  bool mouse_down = false;
  int start_x = 0;
  int start_y = 0;

  Game game;
  OpenGLRenderer renderer;
  
  // renderer.shader_program = SetupShaderProgram("runtime/vertext_shader.glsl", "runtime/fragment_shader.glsl").ReleaseValue();

  while (!quit) {
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT)
      quit = true;
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        quit = true;
      else {
        switch (e.key.keysym.sym) {
          case 'a':
            game.player.Move(glm::vec3 {-0.1, 0.0, 0.0 });
            break;
          case 'd':
            game.player.Move(glm::vec3 {0.1, 0.0, 0.0 });
            break;
          case 'w':
            game.player.Move(glm::vec3 {0.0, 0.1, 0.0 });
            break;
          case 's':
            game.player.Move(glm::vec3 {0.0, -0.1, 0.0 });
            break;
          case 'q':
            game.player.Move(glm::vec3 {0.0, 0.0, 0.1 });
            break;
          case 'e':
            game.player.Move(glm::vec3 {-0.0, 0.0, -0.1 });
            break;
        }
      }
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
      mouse_down = true;
      start_x = e.button.x;
      start_y = e.button.y;
    }
    else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
      mouse_down = false;
      move_x = 0.0f;
      move_y = 0.0f;
    }
    else if (e.type == SDL_MOUSEMOTION && mouse_down) {
      int x = e.button.x;
      int y = e.button.y;

      int dx = x - start_x;
      int dy = y - start_y;

      start_x = x;
      start_y = y;

      game.player.Rotate(dx * 5.0, dy * 5.0);

    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game_update(game, renderer);

    SDL_GL_SwapWindow(window);
  }

  SDL_Quit();
  return 0;
}
