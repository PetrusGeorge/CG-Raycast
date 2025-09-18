#include <cmath>

#include "Vector3.h"

struct Camera {
    Vector3 position;
    Vector3 forward;
    Vector3 up;
    Vector3 right;
    float fov;
    float aspect_ratio;

    float yaw;   // Rotation around Y axis (left/right)
    float pitch; // Rotation around X axis (up/down)

    Camera()
        : position(0, 0, 5), forward(0, 0, -1), up(0, 1, 0), fov(60.0), aspect_ratio(1.0),
          yaw(atan2f(forward.z, forward.x)), pitch(asinf(forward.y)) {
        update_vectors();
    }

    Camera(const Vector3 &pos, const Vector3 &look_at, const Vector3 &up_vec, float field_of_view, float aspect)
        : position(pos), forward((look_at - pos).normalized()), up(up_vec.normalized()), fov(field_of_view),
          aspect_ratio(aspect), yaw(atan2f(forward.z, forward.x)), pitch(asinf(forward.y)) {}

    void update_vectors() {
        // Calculate forward vector from yaw and pitch
        forward.x = std::cos(yaw) * std::cos(pitch);
        forward.y = std::sin(pitch);
        forward.z = std::sin(yaw) * std::cos(pitch);
        forward = forward.normalized();

        // Calculate right and up vectors
        // TODO: isso aqui ta errado, tem que aplicar a rotacao mesmo
        right = Vector3(0, 1, 0).cross(forward).normalized();
        up = forward.cross(right).normalized();
    }

    void rotate(float delta_yaw, float delta_pitch) {
        yaw += delta_yaw;
        pitch += delta_pitch;

        // Clamp pitch to avoid flipping
        const float max_pitch = 89.0F * M_PI / 180.0F;
        pitch = std::max(-max_pitch, std::min(max_pitch, pitch));

        update_vectors();
    }

    void move(float dx, float dy, float dz) {
        // Move relative to camera orientation
        position = position + right * dx + up * dy + forward * dz;
    }

    Vector3 get_ray_direction(int screen_x, int screen_y, int width, int height) const {
        // Convert screen coordinates to normalized device coordinates
        float ndc_x = (2.0F * screen_x / width - 1.0F) * aspect_ratio;
        float ndc_y = 1.0F - (2.0F * screen_y / height);

        // Scale by field of view
        const float scale = tan(fov * 0.5F * M_PI / 180.0F);
        ndc_x *= scale;
        ndc_y *= scale;

        // Calculate ray direction in world space
        return (forward + right * ndc_x + up * ndc_y).normalized();
    }
};
