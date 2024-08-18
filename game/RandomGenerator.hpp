#pragma once

#include "Macros.hpp"

struct RandomGenerator
{
  NON_COPYABLE(RandomGenerator);
  NON_MOVABLE(RandomGenerator);

  RandomGenerator() = default;
  virtual ~RandomGenerator() {}

  virtual void generateFloat(float* values, int count) = 0;
  virtual void generateFloat(float* values, int count, float minimum, float maximum) = 0;
};


struct RandomGeneratorUseStd : public RandomGenerator
{
  RandomGeneratorUseStd();
  ~RandomGeneratorUseStd();
  
  void generateFloat(float* values, int count) override;
  void generateFloat(float* values, int count, float minimum, float maximum) override;
private:

  struct Implementation;
  Implementation* m_internal {nullptr};
};