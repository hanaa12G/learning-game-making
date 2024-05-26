#pragma once
#include <iostream>

namespace sample_models {
  namespace CubeWithBrickTexture {
  

    static float vertices[] = {
       0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.333f, // E
       0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.25f, 0.333f, // A
       0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.25f, 0.666f, // D
       0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.666f, // H

       0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.25f, 0.333f, // A
       1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.333f, // B 
       1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.666f, // C
       0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.25f, 0.666f, // D

       0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.25f, 0.0f, // E(2)
       1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.0f, // F
       1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.333f, // B 
       0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.25f, 0.333f, // A
       
       0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.25f, 0.666f, // D
       1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.666f, // C
       1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, // G
       0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.25f, 1.0f, // H(2)

       1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.333f, // B 
       1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.75f, 0.333f, // F(2)
       1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.75f, 0.666f, // G(2)
       1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.666f, // C

       1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.75f, 0.333f, // F(2)
       0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.333f, // E(3)
       0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.666f, // H(2)
       1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.75f, 0.666f, // G(2)

    };

    static unsigned indices[] = {
      0, 1, 2,
      0, 2, 3,

      4, 5, 6,
      4, 6, 7, 

      8, 9, 10,
      8, 10, 11,

      12, 13, 14,
      12, 14, 15,

      16, 17, 18,
      16, 18, 19,

      20, 21, 22,
      20, 22, 23
    };
  }

  namespace Cube {
  

    static float vertices[] = {
       0.0f, 0.0f, 1.0f, 0.0f, 0.333f, // E
       0.0f, 0.0f, 0.0f, 0.25f, 0.333f, // A
       0.0f, 1.0f, 0.0f, 0.25f, 0.666f, // D
       0.0f, 1.0f, 1.0f, 0.0f, 0.666f, // H

       0.0f, 0.0f, 0.0f, 0.25f, 0.333f, // A
       1.0f, 0.0f, 0.0f, 0.5f, 0.333f, // B 
       1.0f, 1.0f, 0.0f, 0.5f, 0.666f, // C
       0.0f, 1.0f, 0.0f, 0.25f, 0.666f, // D

       0.0f, 0.0f, 1.0f, 0.25f, 0.0f, // E(2)
       1.0f, 0.0f, 1.0f, 0.5f, 0.0f, // F
       1.0f, 0.0f, 0.0f, 0.5f, 0.333f, // B 
       0.0f, 0.0f, 0.0f, 0.25f, 0.333f, // A
       
       0.0f, 1.0f, 0.0f, 0.25f, 0.666f, // D
       1.0f, 1.0f, 0.0f, 0.5f, 0.666f, // C
       1.0f, 1.0f, 1.0f, 0.5f, 1.0f, // G
       0.0f, 1.0f, 1.0f, 0.25f, 1.0f, // H(2)

       1.0f, 0.0f, 0.0f, 0.5f, 0.333f, // B 
       1.0f, 0.0f, 1.0f, 0.75f, 0.333f, // F(2)
       1.0f, 1.0f, 1.0f, 0.75f, 0.666f, // G(2)
       1.0f, 1.0f, 0.0f, 0.5f, 0.666f, // C

       1.0f, 0.0f, 1.0f, 0.75f, 0.333f, // F(2)
       0.0f, 0.0f, 1.0f, 1.0f, 0.333f, // E(3)
       0.0f, 1.0f, 1.0f, 1.0f, 0.666f, // H(2)
       1.0f, 1.0f, 1.0f, 0.75f, 0.666f, // G(2)

    };

  }

  namespace Plane {
  
    static float vertices[] = {
      -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

      -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
  }
}

#include <vector>

struct ModelConstruction {

  struct Point {
    float x;
    float y;
    float z;
  };

  struct Texture {
    float tx;
    float ty;
  };


  struct PointAndTexture {
    float x;
    float y;
    float z;
    float tx;
    float ty;

    PointAndTexture(Point p, Texture t) : x {p.x}, y {p.y}, z{p.z}, tx{t.tx}, ty{t.ty} {}
  };


  static std::vector<float> Triangle(
    PointAndTexture p1,
    PointAndTexture p2,
    PointAndTexture p3
  ) {
    return std::vector<float> {
      p1.x, p1.y, p1.z, p1.tx, p1.ty,
      p2.x, p2.y, p2.z, p2.tx, p2.ty,
      p3.x, p3.y, p3.z, p3.tx, p3.ty
    };
  }

  static std::vector<float> Rectangle(
    Point p1,
    Point p2,
    Point p3,
    Point p4
  ) {
    auto triangle_1 = Triangle(
      PointAndTexture(p1, {0.0, 0.0}), 
      PointAndTexture(p2, {0.0, 1.0}),
      PointAndTexture(p3, {1.0, 1.0}));

    auto triangle_2 = Triangle(
      PointAndTexture(p1, {0.0, 0.0}), 
      PointAndTexture(p3, {1.0, 1.0}),
      PointAndTexture(p4, {1.0, 0.0}));
    triangle_1.insert(triangle_1.end(), triangle_2.begin(), triangle_2.end());
    std::cout << triangle_1.size() << std::endl;
    return triangle_1;
  }

  static std::vector<float> Cube(Point p1, Point p2, Point p3, Point p4, Point p5, Point p6, Point p7, Point p8)
  {
    auto rec_1 = Rectangle(p1, p2, p3, p4);
    auto rec_2 = Rectangle(p1, p5, p6, p2);
    auto rec_3 = Rectangle(p3, p7, p6, p2);
    auto rec_4 = Rectangle(p4, p8, p7, p3);
    auto rec_5 = Rectangle(p1, p5, p8, p4);
    auto rec_6 = Rectangle(p5, p6, p7, p8);

    auto res = rec_1;
    res.insert(res.end(), rec_2.begin(), rec_2.end());
    res.insert(res.end(), rec_3.begin(), rec_3.end());
    res.insert(res.end(), rec_4.begin(), rec_4.end());
    res.insert(res.end(), rec_5.begin(), rec_5.end());
    res.insert(res.end(), rec_6.begin(), rec_6.end());
    return res;
  }

};
