#pragma once

#include "Geometry/vector2.hpp"
#include <glm/glm.hpp>

class Camera {
public:
    enum class ProjectionType { Orthographic, Perspective };

    Camera() noexcept = default;
    ~Camera() noexcept = default;

    // Projection Setup
    void setOrthographic(float screenWidth, float screenHeight, float zNear = -1.0f, float zFar = 1.0f) noexcept;
    void setPerspective(float fovRadians, float aspect, float zNear, float zFar) noexcept;

    // Transform Modifiers
    void setPosition(Vec2f position) noexcept;
    void setZoom(float zoom) noexcept;
    void setViewportNormalized(float x, float y, float width, float height) noexcept;

    // Getters
    [[nodiscard]] Vec2f getPosition() const noexcept { return m_position; }
    [[nodiscard]] float getZoom() const noexcept { return m_zoom; }
    [[nodiscard]] const glm::mat4& getProjectionMatrix() const noexcept { return m_projection; }
    [[nodiscard]] const glm::mat4& getViewMatrix() const noexcept;

    // Render Commands
    void applyViewport(int windowWidth, int windowHeight) const noexcept;

private:
    void updateViewMatrix() const noexcept;

    Vec2f m_position{0.0f, 0.0f};
    float m_zoom = 1.0f;
    
    float m_screenWidth = 1920.0f;
    float m_screenHeight = 1080.0f;

    float m_viewportX = 0.0f;
    float m_viewportY = 0.0f;
    float m_viewportW = 1.0f;
    float m_viewportH = 1.0f;

    ProjectionType m_projType = ProjectionType::Orthographic;
    
    mutable glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};
    mutable bool m_viewDirty = true;
};