#include "Render/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

void Camera::setOrthographic(float screenWidth, float screenHeight, float zNear, float zFar) noexcept {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    
    // Inverting bottom (screenHeight) and top (0.0f) to switch to Top-Left 2D space
    m_projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, zNear, zFar);
    m_projType = ProjectionType::Orthographic;
}

void Camera::setPerspective(float fovRadians, float aspect, float zNear, float zFar) noexcept {
    m_projection = glm::perspective(fovRadians, aspect, zNear, zFar);
    m_projType = ProjectionType::Perspective;
}

void Camera::setPosition(Vec2f position) noexcept {
    m_position = position;
    m_viewDirty = true;
}

void Camera::setZoom(float zoom) noexcept {
    m_zoom = glm::max(zoom, 0.001f); // Protect against divide-by-zero or inversion artifacts
    m_viewDirty = true;
}

void Camera::setViewportNormalized(float x, float y, float width, float height) noexcept {
    m_viewportX = x;
    m_viewportY = y;
    m_viewportW = width;
    m_viewportH = height;
}

const glm::mat4& Camera::getViewMatrix() const noexcept {
    if (m_viewDirty) {
        updateViewMatrix();
    }
    return m_view;
}

void Camera::applyViewport(int windowWidth, int windowHeight) const noexcept {
    int x = static_cast<int>(m_viewportX * windowWidth);
    int y = static_cast<int>(m_viewportY * windowHeight);
    int w = static_cast<int>(m_viewportW * windowWidth);
    int h = static_cast<int>(m_viewportH * windowHeight);
    glViewport(x, y, w, h);
}

void Camera::updateViewMatrix() const noexcept {
    // 1. Create a forward transform matrix representing the camera's location in space
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, 0.0f));
    
    if (m_projType == ProjectionType::Orthographic) {
        transform = glm::scale(transform, glm::vec3(m_zoom, m_zoom, 1.0f));
    }
    
    // 2. The View Matrix is the exact inverse of where the camera is located
    m_view = glm::inverse(transform);
    m_viewDirty = false;
}