#include "Objects.hpp"

GameObject2D::GameObject2D()
{
    m_meta = new ObjectMetadata();
}

GameObject2D::~GameObject2D()
{
    delete m_meta;
    delete m_collision_body;
    delete m_visual_body;
    delete m_input_func;
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

void GameObject2D::process_inputs(GameInput const& inputs)
{
    if (m_input_func) (*m_input_func)(*this, inputs);
}

void GameObject2D::load_inputs(InputFunc func)
{
    if (not m_input_func) {
        delete m_input_func;
    }

    m_input_func = new InputFunc(func);
}