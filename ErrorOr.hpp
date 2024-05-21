#pragma once
#include <utility>



struct Error {
  virtual ~Error() {}
  virtual char const* message() const { return nullptr;}
};

template<typename ValueType, typename ErrorType = Error>
struct ErrorOr {
  
  ErrorOr(ErrorOr const&) = delete;
  ErrorOr(ErrorOr&&) = delete;
  ErrorOr& operator=(ErrorOr const& other) = delete;
  ErrorOr& operator=(ErrorOr&&) = delete;

  ErrorOr(ValueType&& v) : m_has_error {false}, m_value {std::move(v)} {
  }

  ErrorOr(): m_has_error {true} {}
  ErrorOr(ErrorType er) : m_has_error {true}, m_error {er} {}

  bool HasError() const { return m_has_error; };
  ValueType& Value() { return m_value; }
  ErrorType& Error() { return m_error; }

  ValueType&& ReleaseValue() {
    return std::move(m_value);
  }

private:
  bool m_has_error;
  ValueType m_value;
  ErrorType m_error;
};

template<typename ErrorType>
struct ErrorOr<void, ErrorType> {

  ErrorOr(ErrorOr const&) = delete;
  ErrorOr(ErrorOr&&) = delete;
  ErrorOr& operator=(ErrorOr const& other) = delete;
  ErrorOr& operator=(ErrorOr&&) = delete;

  ErrorOr(): m_has_error {false} {}
  ErrorOr(ErrorType er) : m_has_error {true}, m_error {er} {}


  bool HasError() const { return m_has_error; };
  void Value() {}
  void ReleaseValue() {}
  ErrorType& Error() { return m_error; }
private:
  bool m_has_error;
  ErrorType m_error;
};


#define TRY(action) ({ \
  auto action_res = (action); \
  if (action_res.HasError()) { \
    return action_res.Error(); \
  } \
  action_res.ReleaseValue(); \
})