#pragma once


#define NON_COPYABLE(x) \
  x(x const&) = delete; \
  x& operator=(x const&) = delete;

#define NON_MOVABLE(x) \
  x(x&&) = delete; \
  x& operator=(x&&) = delete;