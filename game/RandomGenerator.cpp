#include "RandomGenerator.hpp"
#include <random>

#include <iostream>

struct RandomGeneratorUseStd::Implementation {
  std::random_device m_rd;
  std::mt19937 m_gen;

  Implementation() : m_gen(m_rd()) {
  
  }
};

RandomGeneratorUseStd::RandomGeneratorUseStd()
{
  m_internal = new Implementation();
}

RandomGeneratorUseStd::~RandomGeneratorUseStd()
{
  delete m_internal;
}

void RandomGeneratorUseStd::generateFloat(float* values, int count)
{
  std::cout << "RandomGenerator: [WARNING] not implement yet" << std::endl;
}

void RandomGeneratorUseStd::generateFloat(float* values, int count, float minimum, float maximum)
{
  std::uniform_real_distribution<> dis(minimum, maximum);

  for (int i = 0; i < count; ++i) {
    values[i] = dis(m_internal->m_gen);
  }
}