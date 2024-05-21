#pragma once
#include "ErrorOr.hpp"
#include "Object.hpp"

struct ShaderError final: public Error {
  enum Type {
    CompileShader,
    InvalidName
  };
  ShaderError() {}
  ShaderError(Type type, char const* format);

  inline char const* message() const override {
    return m_message;
  }

private:
  Type m_type;
  char m_message[1024];
};

struct ShaderProgram {
  static ErrorOr<ShaderProgram> create();

  NON_COPYABLE(ShaderProgram);

  ShaderProgram(ShaderProgram&& other) {
    program_id = other.program_id;
    m_vertex_shader = other.m_vertex_shader;
    m_fragment_shader = other.m_fragment_shader;

    other.program_id = 0;
    other.m_vertex_shader = 0;
    other.m_fragment_shader = 0;
  }
  ShaderProgram& operator=(ShaderProgram&& other) {
    program_id = other.program_id;
    m_vertex_shader = other.m_vertex_shader;
    m_fragment_shader = other.m_fragment_shader;

    other.program_id = 0;
    other.m_vertex_shader = 0;
    other.m_fragment_shader = 0;
    return *this;
  }
  ShaderProgram() = default;
  ~ShaderProgram();
  ErrorOr<void, ShaderError> UseVertexShader(char const* path);
  ErrorOr<void, ShaderError> UseFragmentShader(char const* path);
  ErrorOr<void, ShaderError> Enable();
  void Use();

  template<typename ...Ts>
  ErrorOr<void, ShaderError> SetUniformValue(char const* name, Ts ...ts) {
    SetUniformValue_(name, ts...);
  }

  template<typename T>
  ErrorOr<void, ShaderError> SetUniformMatrix(char const* name, T m, bool transpose = false, unsigned _ = T::length());

public:
  int program_id { 0 };
  int m_vertex_shader { 0 };
  int m_fragment_shader { 0 };

private:

  ErrorOr<void, ShaderError> SetUniformValue_(char const*, float, float, float, float);
};

