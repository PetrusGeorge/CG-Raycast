#include "Renderer.h"

#include <cerrno>
#include <cmath>
#include <iostream>
#include <iterator>

// Triangle method implementations
std::optional<float> Triangle::ray_intersect(const Vector3 &ray_origin, const Vector3 &ray_dir) const {
    const float epsilon = 0.0000001;
    const Vector3 edge1 = v1 - v0;
    const Vector3 edge2 = v2 - v0;
    const Vector3 h = ray_dir.cross(edge2);
    const float a = edge1.dot(h);

    if (a > -epsilon && a < epsilon) {
        return {}; // Ray is parallel to triangle
    }

    const float f = 1.0F / a;
    const Vector3 s = ray_origin - v0;
    const float u = f * s.dot(h);

    if (u < 0.0F || u > 1.0F) {
        return {};
    }

    const Vector3 q = s.cross(edge1);
    const float v = f * ray_dir.dot(q);

    if (v < 0.0F || u + v > 1.0F) {
        return {};
    }

    const float t = f * edge2.dot(q);
    if (t < epsilon) {
        return {};
    }

    return {t};
}

Vector3 Triangle::get_normal() const {
    const Vector3 edge1 = v1 - v0;
    const Vector3 edge2 = v2 - v0;
    return edge1.cross(edge2).normalized();
}

void Renderer::init(int argc, char **argv) const {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(m_window_width, m_window_height);
    glutCreateWindow("Pure Raycast Renderer with GLUT");

    // Register static wrapper functions
    glutDisplayFunc(display_wrapper);
    glutReshapeFunc(reshape_wrapper);
    glutKeyboardFunc(keyboard_wrapper);
    glutSpecialFunc(special_keys_wrapper);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glPointSize(1.0F);

    glutMainLoop();
}

void Renderer::add_triangle(const Triangle &triangle) { m_primitives.push_back(triangle); }
void Renderer::add_object(std::vector<Triangle> object) {
    m_primitives.insert(m_primitives.end(), std::make_move_iterator(object.begin()),
                        std::make_move_iterator(object.end()));
}

// Static wrapper functions
void Renderer::display_wrapper() { Renderer::get_instance().render(); }
void Renderer::reshape_wrapper(int w, int h) { Renderer::get_instance().reshape(w, h); }
void Renderer::keyboard_wrapper(unsigned char key, int x, int y) { Renderer::get_instance().keyboard(key, x, y); }
void Renderer::special_keys_wrapper(int key, int x, int y) { Renderer::get_instance().special_keys(key, x, y); }

// Instance methods
void Renderer::render() {
    if (m_pixel_buffer.size() != m_window_width * m_window_height * 3) {
        m_pixel_buffer.resize(m_window_width * m_window_height * 3);
    }

#pragma omp parallel for
    for (int y = 0; y < m_window_height; ++y) {
        for (int x = 0; x < m_window_width; ++x) {
            const Vector3 ray_dir = m_camera.get_ray_direction(x, y, m_window_width, m_window_height);
            const Color pixel_color = trace_ray(m_camera.position, ray_dir);

            // Calculate buffer index (flip Y coordinate for OpenGL)
            const int buffer_y = m_window_height - y - 1;
            const int index = (buffer_y * m_window_width + x) * 3;

            // Store pixel in buffer
            m_pixel_buffer[index] = static_cast<GLubyte>(pixel_color.r * 255);
            m_pixel_buffer[index + 1] = static_cast<GLubyte>(pixel_color.g * 255);
            m_pixel_buffer[index + 2] = static_cast<GLubyte>(pixel_color.b * 255);
        }
    }

    // Clear and draw pixels
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(m_window_width, m_window_height, GL_RGB, GL_UNSIGNED_BYTE, m_pixel_buffer.data());
    glutSwapBuffers();
}

void Renderer::reshape(int w, int h) {
    m_window_width = w;
    m_window_height = h;
    m_camera.aspect_ratio = float(w) / float(h);

    glutPostRedisplay();
}

void Renderer::keyboard(unsigned char key, int /*x*/, int /*y*/) {
    const float speed = 0.1F;
    switch (key) {
    case 'w':
    case 'W':
        m_camera.move(0, 0, speed);
        break;
    case 's':
    case 'S':
        m_camera.move(0, 0, -speed);
        break;
    case 'a':
    case 'A':
        m_camera.move(-speed, 0, 0);
        break;
    case 'd':
    case 'D':
        m_camera.move(speed, 0, 0);
        break;
    case 'q':
    case 'Q':
        m_camera.move(0, speed, 0);
        break;
    case 'e':
    case 'E':
        m_camera.move(0, -speed, 0);
        break;
    case 27:
        exit(0);
        break; // ESC key
    }
    glutPostRedisplay();
}

void Renderer::special_keys(int key, int /*x*/, int /*y*/) {
    const float speed = 0.05F;
    switch (key) {
    case GLUT_KEY_LEFT:
        m_camera.rotate(speed, 0);
        break;
    case GLUT_KEY_RIGHT:
        m_camera.rotate(-speed, 0);
        break;
    case GLUT_KEY_UP:
        m_camera.rotate(0, speed);
        break;
    case GLUT_KEY_DOWN:
        m_camera.rotate(0, -speed);
        break;
    }
    glutPostRedisplay();
}

Color Renderer::trace_ray(const Vector3 &origin, const Vector3 &direction) {
    float closest_t = INFINITY;
    int closest_idx = -1;

    for (int i = 0; i < m_primitives.size(); i++) {
        const Triangle &triangle = m_primitives[i];
        if (auto t = triangle.ray_intersect(origin, direction)) {
            if (*t < closest_t) {
                closest_t = *t;
                closest_idx = i;
            }
        }
    }

    if (closest_idx >= 0) {
        const Triangle &closest_triangle = m_primitives[closest_idx];
        const Vector3 hit_point = origin + direction * closest_t;
        const Vector3 normal = closest_triangle.get_normal();

        const Vector3 light_pos(0.0F, 10.0F, 0.0F);
        // const Vector3 light_pos = m_camera.position;
        const Vector3 to_light = (light_pos - hit_point).normalized();

        for (int i = 0; i < m_primitives.size(); i++) {
            if (i == closest_idx) {
                continue;
            }
            const Triangle &triangle = m_primitives[i];
            if (auto t = triangle.ray_intersect(hit_point, to_light)) {
                return closest_triangle.color * 0.2F;
            }
        }
        // std::cout << normal.dot(to_light) << '\n';
        float intensity = std::max(0.0F, abs(normal.dot(to_light)));

        const float ambient = 0.2F;
        intensity = ambient + (1.0F - ambient) * intensity;

        return closest_triangle.color * intensity;
    }

    return m_background_color;
}
