#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include <GL/glut.h>
#include <algorithm>
#include <optional>
#include <vector>

// Cores e suas operações
struct Color {
    float r, g, b;

    Color() : r(0), g(0), b(0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(float scalar) const { return {r * scalar, g * scalar, b * scalar}; }
    Color operator/(float scalar) const { return {r / scalar, g / scalar, b / scalar}; }

    Color operator+(const Color &other) const { return {r + other.r, g + other.g, b + other.b}; }
    Color operator*(const Color &other) const { return {r * other.r, g * other.g, b * other.b}; }

    void saturate() {
        r = std::clamp(r, 0.0F, 1.0F);
        g = std::clamp(g, 0.0F, 1.0F);
        b = std::clamp(b, 0.0F, 1.0F);
    };
};

// Primitiva escolhida para interseção de raios
struct Triangle {
    Vector3 v0, v1, v2;
    Color color;

    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color)
        : v0(v0), v1(v1), v2(v2), color(color) {}

    std::optional<float> ray_intersect(const Vector3 &ray_origin, const Vector3 &ray_dir) const;
    Vector3 get_normal() const;
};

struct Light {
  Vector3 pos;
  Color color{1,1,1};
  float attenuation_factor{0};

  Light(Vector3 pos) : pos(pos) {};
  Light(Vector3 pos, Color color) : pos(pos), color(color) {};
  Light(Vector3 pos, float attenuation_factor) : pos(pos), attenuation_factor(attenuation_factor) {};
  Light(Vector3 pos, Color color, float attenuation_color) : pos(pos), color(color), attenuation_factor(attenuation_factor) {};
};

// Renderer é uma classe para utilizar o raycasting
// devido a a natureza do OpenGL a classe é instanciada apenas
// uma vez (Singleton)
class Renderer {
  private:
    static Renderer &instance;

    std::vector<Triangle> m_primitives;
    std::vector<Light> m_lights;
    Camera m_camera;
    int m_window_width = 800;
    int m_window_height = 600;
    float m_ambient = 0.2;
    std::vector<GLubyte> m_pixel_buffer;
    Color m_background_color;

    Renderer() {};

    // Funções estaticas para o FreeGLUT
    static void display_wrapper();
    static void reshape_wrapper(int w, int h);
    static void keyboard_wrapper(unsigned char key, int x, int y);
    static void special_keys_wrapper(int key, int x, int y);

    void render();
    Color trace_ray(const Vector3 &origin, const Vector3 &direction);

    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);
    void special_keys(int key, int x, int y);

  public:
    // Deleta construtores para o padrão Singleton
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
    void set_ambient(float ambient);
    void set_camera(Camera camera);
    void add_triangle(const Triangle &triangle);
    void add_object(std::vector<Triangle> object);
    void add_light(const Light& light);
    void add_lights(std::vector<Light> light);
};

#endif
