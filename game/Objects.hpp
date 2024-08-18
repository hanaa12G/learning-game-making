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

using InputFunc = std::function<void (GameObject2D& obj, GameInput const&, float elapsed)>;

struct GameObject2D {

    GameObject2D();

    GameObject2D(GameObject2D const&);
    GameObject2D& operator=(GameObject2D const&);

    GameObject2D(GameObject2D&&) noexcept;
    GameObject2D& operator=(GameObject2D&&) noexcept;

    ~GameObject2D();
    
    glm::vec2 m_position {0.0f, 0.0f};
    ObjectMetadata* m_meta { nullptr };
    CollisionBody2D* m_collision_body { nullptr };
    VisualBody2D* m_visual_body { nullptr };
    InputFunc* m_input_func { nullptr };


    // Meta
    std::string get_name() const;
    void set_name(std::string);
    std::string get_id() const;

    // CollisionBody2D

    // VisualBody2D
    VisualBody2D& get_visual_body();
    void set_visual_body(VisualBody2D);

    // Input
    void process_inputs(GameInput const& inputs, float elapsed);
    void load_inputs(InputFunc func);


    void set_position(glm::vec2 pos);
    glm::vec2 get_position() const;
};

struct ObjectMetadata {
    std::string m_id {};
    std::string m_name {};

    std::string get_name() const { return m_name; }
    void set_name(std::string name) { m_name = name; }
    std::string get_id() const { return m_id; }
};



struct CollisionShape2DBox {
    glm::vec2 m_origin {0.0f, 0.0f};
    float m_width {};
    float m_height {};


    glm::vec2 top_left() const { return glm::vec2 { m_origin.x - m_width / 2, m_origin.y + m_height / 2 }; }
    glm::vec2 bottom_right() const { return glm::vec2 { m_origin.x + m_width / 2, m_origin.y - m_height / 2}; }
};

struct CollisionShape2DSphere {
    glm::vec2 m_origin {0.0f, 0.0f};
    float m_radius {};
};

struct CollisionShape2DCapsule {

};

using CollisionShape2D = std::variant<CollisionShape2DBox,
    CollisionShape2DSphere,
    CollisionShape2DCapsule>;

struct CollisionBody2D {
    glm::vec2 m_origin {0.0f, 0.0f};
    glm::vec2 m_position {0.0f, 0.0f};
    glm::vec2 m_velocity {0.0f, 0.0f};
    glm::vec2 m_orientation {0.0f, 0.0f};

    CollisionShape2D m_shape {};

    void set_position(glm::vec2 pos) { m_position = pos + m_origin; } 
};

struct VisualBody2D {
    enum class Type {
        Texture2D
    };

    Type m_type {Type::Texture2D};
    glm::vec2 m_origin {0.0f, 0.0f};
    glm::vec2 m_position {0.0f, 0.0f};
    float m_width {0.0f};
    float m_height {0.0f};
    float m_scaling {1.0f};
    std::string m_texture_path {};

    std::string texture_path() const { return m_texture_path; }
    std::string key() const { return texture_path(); }

    void set_position(glm::vec2 pos) { m_position = pos + m_origin; }
    void set_texture_path(std::string path) { m_texture_path = path; }
    void set_scaling(float scaling) { m_scaling = scaling; }
};