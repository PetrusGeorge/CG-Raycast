#include <algorithm>
#include <cmath>
#include <iostream>

#include "Vector3.h"

class Camera {

  public:
    Camera() {}

    Camera(const Vector3 &pos, float fov)
        : m_position(pos), m_fov(fov) {}

    void rotate(float delta_yaw, float delta_pitch) {
        m_yaw += delta_yaw;
        m_pitch += delta_pitch;

        // Evita que a camera gire na vertical
        const float max_pitch = 89.0F * M_PI / 180.0F;
        m_pitch = std::clamp(m_pitch, -max_pitch, max_pitch);

        // Apply rotation
        m_forward.x = std::cos(m_yaw) * std::cos(m_pitch);
        m_forward.y = std::sin(m_pitch);
        m_forward.z = std::sin(m_yaw) * std::cos(m_pitch);
        m_forward = m_forward.normalized();

        m_right = Vector3(0, 1, 0).cross(m_forward).normalized();
        m_up = m_forward.cross(m_right).normalized();
    }

    void move(float dx, float dy, float dz) {
        // Movimento da câmera em relação a seus vetores
        m_position = m_position + m_right * dx + m_up * dy + m_forward * dz;
    }

    Vector3 get_ray_direction(int screen_x, int screen_y, int width, int height) const {
        // Converte coordenadas de tela para coordenadas normalizadas de dispositivo
        float ndc_x = (2.0F * screen_x / width - 1.0F) * m_aspect_ratio;

        // As coordenadas Y são invertidas
        float ndc_y = 1.0F - (2.0F * screen_y / height);

        // Aplica o fov para o ângulo de visão
        const float scale = tan(m_fov * 0.5F * M_PI / 180.0F);
        ndc_x *= scale;
        ndc_y *= scale;

        // Calcula a direção do raio
        return (m_forward + m_right * ndc_x + m_up * ndc_y).normalized();
    }

    const Vector3& get_position() { return m_position; }
    void set_aspect_ratio(float aspect_ratio) { m_aspect_ratio = aspect_ratio; }

  private:
    Vector3 m_position = {0, 0, 0};
    Vector3 m_forward = {0, 0, -1};
    Vector3 m_right = {-1, 0, 0};
    Vector3 m_up = {0, 1 ,0};
    float m_fov = 60.0;
    float m_aspect_ratio = 4.0/3.0;

    float m_yaw = -M_PI / 2.0;
    float m_pitch = 0.0;
};
