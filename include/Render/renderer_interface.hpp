#pragma once

#include "Render/Buffer/gl_alias.hpp"

class PipelineState{};
class IRenderer{
public:
    virtual ~IRenderer() = default;

    virtual void sendData() = 0;
    virtual void drawCall() = 0;
private:
    ShaderID m_shader_id;
    PipelineState m_pipeline_state;
};