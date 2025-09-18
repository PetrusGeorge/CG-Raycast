#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include <GL/glut.h>
#include <optional>
#include <vector>

struct Color {
    float r, g, b;

    Color() : r(0), g(0), b(0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float scalar) const { return {r * scalar, g * scalar, b * scalar}; }

    Color operator+(const Color &other) const { return {r + other.r, g + other.g, b + other.b}; }
};

struct Triangle {
    Vector3 v0, v1, v2;
    Color color;

    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color)
        : v0(v0), v1(v1), v2(v2), color(color) {}

    std::optional<float> ray_intersect(const Vector3 &ray_origin, const Vector3 &ray_dir) const;
    Vector3 get_normal() const;
};

class Renderer {
  private:
    static Renderer &instance; // Static instance for callbacks

    std::vector<Triangle> m_primitives;
    Camera m_camera;
    int m_window_width = 800;
    int m_window_height = 600;
    std::vector<GLubyte> m_pixel_buffer;
    Color m_background_color;

    Renderer() {};

    // Static callback wrappers
    static void display_wrapper();
    static void reshape_wrapper(int w, int h);
    static void keyboard_wrapper(unsigned char key, int x, int y);
    static void special_keys_wrapper(int key, int x, int y);

    // Instance methods
    void render();
    Color trace_ray(const Vector3 &origin, const Vector3 &direction);

    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);
    void special_keys(int key, int x, int y);

  public:
    Renderer(Renderer &&) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    void operator=(const Renderer &) = delete;

    ~Renderer() = default;

    static Renderer &get_instance() {
        static Renderer instance;
        return instance;
    }

    void init(int argc, char **argv) const;
    void add_triangle(const Triangle &triangle);
    void add_object(std::vector<Triangle> object);
};

#endif
