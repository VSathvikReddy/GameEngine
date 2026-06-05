#pragma once

#include "Geometry/vector2.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class Camera {
public:
    enum class ProjectionType { Orthographic, Perspective };

    Camera() noexcept = default;
    ~Camera() noexcept = default;

    // 👑 Top-Left 2D Adjustment: 0.0f is passed to top, and screen height is passed to bottom.
    void setOrthographic(float screenWidth, float screenHeight, float zNear = -1.0f, float zFar = 1.0f) noexcept {
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;
        
        // Swapping bottom and top flips the OpenGL Y-Axis to point downwards natively
        m_projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, zNear, zFar);
        m_projType = ProjectionType::Orthographic;
    }

    void setPerspective(float fovRadians, float aspect, float zNear, float zFar) noexcept {
        m_projection = glm::perspective(fovRadians, aspect, zNear, zFar);
        m_projType = ProjectionType::Perspective;
    }

    void setPosition(Vec2f position) noexcept {
        m_position = position;
        m_viewDirty = true;
    }

    void setZoom(float zoom) noexcept {
        m_zoom = glm::max(zoom, 0.001f);
        m_viewDirty = true;
    }

    void setViewportNormalized(float x, float y, float width, float height) noexcept {
        m_viewportX = x;
        m_viewportY = y;
        m_viewportW = width;
        m_viewportH = height;
    }

    [[nodiscard]] Vec2f getPosition() const noexcept { return m_position; }
    [[nodiscard]] float getZoom() const noexcept { return m_zoom; }

    [[nodiscard]] const glm::mat4& getProjectionMatrix() const noexcept { return m_projection; }
    
    [[nodiscard]] const glm::mat4& getViewMatrix() const noexcept {
        if (m_viewDirty) {
            updateViewMatrix();
        }
        return m_view;
    }

    void applyViewport(int windowWidth, int windowHeight) const noexcept {
        int x = static_cast<int>(m_viewportX * windowWidth);
        int y = static_cast<int>(m_viewportY * windowHeight);
        int w = static_cast<int>(m_viewportW * windowWidth);
        int h = static_cast<int>(m_viewportH * windowHeight);
        glViewport(x, y, w, h);
    }

private:
    void updateViewMatrix() const noexcept {
        // 2D Camera Strategy: Translate to position first
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, 0.0f));
        
        if (m_projType == ProjectionType::Orthographic) {
            transform = glm::scale(transform, glm::vec3(m_zoom, m_zoom, 1.0f));
        }
        
        m_view = glm::inverse(transform);
        m_viewDirty = false;
    }

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