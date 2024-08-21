#pragma once
#include "ShaderProgram.hpp"
#include "game/Game.hpp"
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "game/Debug.cpp"

struct StbImageDeleter {
  void operator()(unsigned char *ptr) const { stbi_image_free(ptr); }
};

struct Renderer2DOpenGL : public Renderer2D {
  struct Resource {
    GLuint m_texture { 0 };
    GLuint m_vao { 0 };
    std::vector<float> m_model_data;
    ShaderProgram m_program;

    Resource() = default;

    ~Resource() {
      glDeleteTextures(1, &m_texture);
      glDeleteVertexArrays(1, &m_vao);
    }

  };

  std::unordered_map<std::string, Resource> m_resources {};
  unsigned m_screen_width { 0u };
  unsigned m_screen_height { 0u };

  void set_screen_size(unsigned width, unsigned height) override {
    m_screen_width = width;
    m_screen_height = height;
  }

  virtual void draw(VisualBody2D const &body) override {
    if (body.m_type == VisualBody2D::Type::Texture2D) {
      draw_texture(body);
    }
    
  }

  void draw_texture(VisualBody2D const &body) {
    if (m_resources.find(body.key()) == 0)
      generate_texture(body);

    Resource& resource = m_resources[body.key()];
    glm::mat4 trans(1.0f);

    glm::vec3 object_screen_position = {
      body.m_position.x / (m_screen_width / 2.0f),
      body.m_position.y / (m_screen_height / 2.0f),
      0.0f
    };

    std::cout << "Renderer2DOpenGL: [INFO] In game position: " << body.m_position
      << ", screen position: " << object_screen_position
      << std::endl;
    trans = glm::translate(trans, object_screen_position);
    glm::mat4 projection = glm::ortho(-1.0f * 0.2f, 1.0f * 0.2f, -1.0f * 0.2f, 1.0f * 0.2f, -1.0f, 1.0f);
    
    // glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    float scaling = 32.0 / 640.0f;
    trans = glm::scale(trans, glm::vec3(scaling, scaling, 1.0f));
    
    auto& program = resource.m_program;
    
    // glEnable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resource.m_texture);
    program.Use();
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program.SetUniformMatrix("transform", trans);
    program.SetUniformMatrix("projection", projection);
    glBindVertexArray(resource.m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

  }

  void generate_texture(VisualBody2D const &body) {
    Resource& resource = m_resources[body.key()];
    glGenTextures(1, &resource.m_texture);
    glBindTexture(GL_TEXTURE_2D, resource.m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    std::string texture_path = body.texture_path();

    if (std::filesystem::exists(texture_path)) {

      int image_width, image_height, nr_channel;
      stbi_set_flip_vertically_on_load(true);
      auto texture_data = std::unique_ptr<unsigned char, StbImageDeleter>(
          stbi_load(texture_path.c_str(), &image_width, &image_height,
                    &nr_channel, 0),
          StbImageDeleter{});

      if (texture_data) {
        std::cout << "Renderer2DOpenGL: [INFO] Loaded image" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                   image_width, image_height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, texture_data.get());
        glGenerateMipmap(GL_TEXTURE_2D);
      }
    }

    resource.m_program = SetupShaderProgram(
        "runtime/2d_vertex_shader.glsl",
        "runtime/2d_fragment_shader.glsl").ReleaseValue();
    resource.m_program.Enable();

    auto vertices = std::vector<float> {
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    resource.m_model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, resource.m_model_data.size() * sizeof(float), resource.m_model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.m_vao = vao;
  }

  ErrorOr<ShaderProgram> SetupShaderProgram(char const* vertex_shader_path, char const* fragment_shader_path) {
    auto shader_1 = TRY(ShaderProgram::create());
    TRY(shader_1.UseVertexShader(vertex_shader_path));
    TRY(shader_1.UseFragmentShader(fragment_shader_path));

    return std::move(shader_1);
  }

};
