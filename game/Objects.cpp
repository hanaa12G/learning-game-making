#include "Objects.hpp"

GameObject2D::GameObject2D()
{
    m_meta = new ObjectMetadata();
}

GameObject2D::GameObject2D(GameObject2D const& other) : GameObject2D()
{
    if (other.m_collision_body) m_collision_body = new CollisionBody2D(*other.m_collision_body);
    if (other.m_visual_body) m_visual_body = new VisualBody2D(*other.m_visual_body);
    if (other.m_input_func) m_input_func = new InputFunc(*other.m_input_func);
    if (other.m_update_func) m_update_func = new UpdateFunc(*other.m_update_func);
}

GameObject2D& GameObject2D::operator=(GameObject2D const& other)
{
    if (other.m_collision_body) {
        delete m_collision_body;
        m_collision_body = new CollisionBody2D(*other.m_collision_body);
    } 
    if (other.m_visual_body) {
        delete m_visual_body;
        m_visual_body = new VisualBody2D(*other.m_visual_body);
    }

    if (other.m_input_func) {
        delete m_input_func;
        m_input_func = new InputFunc(*other.m_input_func);
    }

    if (other.m_update_func) {
        delete m_update_func;
        m_update_func = new UpdateFunc(*other.m_update_func);
    }
    return *this;
}

GameObject2D::GameObject2D(GameObject2D&& other) noexcept
{
    m_position = std::move(other.m_position);
    std::swap(m_meta, other.m_meta);
    std::swap(m_collision_body, other.m_collision_body);
    std::swap(m_visual_body, other.m_visual_body);
    std::swap(m_input_func, other.m_input_func);
    std::swap(m_update_func, other.m_update_func);
}

GameObject2D& GameObject2D::operator=(GameObject2D&& other) noexcept
{
    m_position = std::move(other.m_position);
    std::swap(m_meta, other.m_meta);
    std::swap(m_collision_body, other.m_collision_body);
    std::swap(m_visual_body, other.m_visual_body);
    std::swap(m_input_func, other.m_input_func);
    std::swap(m_update_func, other.m_update_func);
    return *this;
}

GameObject2D::~GameObject2D()
{
    delete m_meta;
    delete m_collision_body;
    delete m_visual_body;
    delete m_input_func;
    delete m_update_func;
}

bool GameObject2D::operator==(GameObject2D const& other) const
{
    return m_meta == other.m_meta and
        m_collision_body == other.m_collision_body and
        m_visual_body == other.m_visual_body and
        m_input_func == other.m_input_func and
        m_update_func == other.m_update_func and
        m_position == other.m_position;
}

std::string GameObject2D::get_id() const
{
    if (m_meta) {
        return m_meta->get_id();
    }
    return "";
}

std::string GameObject2D::get_name() const
{
    if (m_meta) {
        return m_meta->get_name();
    }
    return "";
}

void GameObject2D::set_name(std::string name)
{
    if (m_meta) {
        return m_meta->set_name(name);
    }
}

CollisionBody2D& GameObject2D::get_collision_body()
{
    assert(m_collision_body);
    return *m_collision_body;
}

void GameObject2D::set_collision_body(CollisionBody2D body)
{
    m_collision_body = new CollisionBody2D(body);
}

VisualBody2D& GameObject2D::get_visual_body()
{
    assert(m_visual_body);
    return *m_visual_body;
}

void GameObject2D::set_visual_body(VisualBody2D v)
{
    m_visual_body = new VisualBody2D(std::move(v));
}


void GameObject2D::process_inputs(GameInput const& inputs, float elapsed)
{
    if (m_input_func) (*m_input_func)(*this, inputs, elapsed);
}

void GameObject2D::load_inputs(InputFunc func)
{
    if (not m_input_func) {
        delete m_input_func;
    }

    m_input_func = new InputFunc(func);
}

void GameObject2D::on_update(UpdateFunc func)
{
    if (m_update_func) delete m_update_func;
    m_update_func = new UpdateFunc(func);
}

void GameObject2D::update(float elapsed)
{
    if (m_update_func) (*m_update_func)(*this, elapsed);
}


void GameObject2D::set_position(glm::vec2 pos)
{
    m_position = pos;
    if (m_collision_body) m_collision_body->set_position(m_position);
    if (m_visual_body) m_visual_body->set_position(m_position);
}

glm::vec2 GameObject2D::get_position() const {
    return m_position;
}