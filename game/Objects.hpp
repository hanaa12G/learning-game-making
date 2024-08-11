#pragma once
#include <glm/vec2.hpp>
// #include <memory>
#include <variant>
#include <string>
#include <functional>

struct CollisionBody2D;
struct VisualBody2D;
struct ObjectMetadata;
struct GameInput;
struct GameObject2D;

using InputFunc = std::function<void (GameObject2D& obj, GameInput const&)>;

struct GameObject2D {

    GameObject2D();

    GameObject2D(GameObject2D const&);
    GameObject2D& operator=(GameObject2D const&);

    GameObject2D(GameObject2D&&) = default;
    GameObject2D& operator=(GameObject2D&&) = default;

    ~GameObject2D();
    
    glm::vec2 m_origin;
    ObjectMetadata* m_meta { nullptr };
    CollisionBody2D* m_collision_body { nullptr };
    VisualBody2D* m_visual_body { nullptr };
    InputFunc* m_input_func { nullptr };


    // Meta
    std::string get_name() const;
    std::string get_id() const;

    // CollisionBody2D

    // Input
    void process_inputs(GameInput const& inputs);
    void load_inputs(InputFunc func);

};

struct ObjectMetadata {
    std::string m_id;
    std::string m_name;

    std::string get_name() const;
    std::string get_id() const;
};



struct CollisionShape2DBox {
    glm::vec2 m_origin;
    float m_width;
    float m_height;


    glm::vec2 top_left() const { return glm::vec2 { m_origin.x - m_width / 2, m_origin.y + m_height / 2 }; }
    glm::vec2 bottom_right() const { return glm::vec2 { m_origin.x + m_width / 2, m_origin.y - m_height / 2}; }
};

struct CollisionShape2DSphere {
    glm::vec2 m_origin;
    float m_radius;
};

struct CollisionShape2DCapsule {

};

using CollisionShape2D = std::variant<CollisionShape2DBox,
    CollisionShape2DSphere,
    CollisionShape2DCapsule>;

struct CollisionBody2D {
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    glm::vec2 m_orientation;

    CollisionShape2D m_shape;
};

struct VisualBody2D {};