#include "Renderer.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

// Função que checa a interseção de um raio com um triangulo
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

// Retorna normal de um triangulo sem se preocupar com o sentido
Vector3 Triangle::get_normal() const {
    const Vector3 edge1 = v1 - v0;
    const Vector3 edge2 = v2 - v0;
    return edge1.cross(edge2).normalized();
}

// Inicializa o renderizador com o cenário presente
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

// Funções para criar o cenário
void Renderer::set_ambient(float ambient) { m_ambient = ambient; }
void Renderer::set_camera(Camera camera) { m_camera = camera; }
void Renderer::add_triangle(const Triangle &triangle) { m_primitives.push_back(triangle); }
void Renderer::add_object(std::vector<Triangle> object) {
    m_primitives.insert(m_primitives.end(), std::make_move_iterator(object.begin()),
                        std::make_move_iterator(object.end()));
}
void Renderer::add_light(const Light& light) { m_lights.push_back(light); }
void Renderer::add_lights(std::vector<Light> lights) {
    m_lights.insert(m_lights.end(), std::make_move_iterator(lights.begin()),
                        std::make_move_iterator(lights.end()));
}

void Renderer::display_wrapper() { Renderer::get_instance().render(); }
void Renderer::reshape_wrapper(int w, int h) { Renderer::get_instance().reshape(w, h); }
void Renderer::keyboard_wrapper(unsigned char key, int x, int y) { Renderer::get_instance().keyboard(key, x, y); }
void Renderer::special_keys_wrapper(int key, int x, int y) { Renderer::get_instance().special_keys(key, x, y); }

void Renderer::render() {
    // Redimensiona o buffer da imagem caso haja redimensionamento da tela
    if (m_pixel_buffer.size() != m_window_width * m_window_height * 3) {
        m_pixel_buffer.resize(m_window_width * m_window_height * 3);
    }

// Paraleliza o for externo com OpenMP
#pragma omp parallel for
    // Percorre cada pixel da tela calculando o raio e sua interseção
    for (int x = 0; x < m_window_width; x++) {
        for (int y = 0; y < m_window_height; y++) {
            const Vector3 ray_dir = m_camera.get_ray_direction(x, y, m_window_width, m_window_height);
            const Color pixel_color = trace_ray(m_camera.position, ray_dir);

            // Calcula o index para buffer
            const int buffer_y = m_window_height - y - 1;
            const int index = (buffer_y * m_window_width + x) * 3;

            m_pixel_buffer[index] = static_cast<GLubyte>(pixel_color.r * 255);
            m_pixel_buffer[index + 1] = static_cast<GLubyte>(pixel_color.g * 255);
            m_pixel_buffer[index + 2] = static_cast<GLubyte>(pixel_color.b * 255);
        }
    }

    // Desenha a imagem na tela
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
        break;
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

    // Checa interseção do raio com todas as primitivas e escolhe a mais próxima
    for (int i = 0; i < m_primitives.size(); i++) {
        const Triangle &triangle = m_primitives[i];
        if (auto t = triangle.ray_intersect(origin, direction)) {
            if (*t < closest_t) {
                closest_t = *t;
                closest_idx = i;
            }
        }
    }

    // Caso não haja interseção no passo anterior retorna cor de fundo padrão
    if (closest_idx == -1) {
        return m_background_color;
    }

    const Triangle &closest_triangle = m_primitives[closest_idx];

    // Ponto exato de interseção com o triângulo mais próximo
    const Vector3 hit_point = origin + direction * closest_t;

    Vector3 normal = closest_triangle.get_normal();
    // Aponta normal para direção da camera
    if (normal.dot(direction) > 0) {
        normal = normal * -1;
    }

    std::vector<float> intensities;
    // Avalia o impacto de cada luz na intensidade do raio
    for (auto& light : m_lights) {
        bool hit = false;
        Vector3 to_light = (light.pos - hit_point);
        const float light_t = to_light.length();
        to_light = to_light.normalized();

        // Shadow ray, checa colisão a partir do ponto de interseção até a luz
        // caso haja um triângulo no caminho o raio é uma sombra para aquela luz
        for (int i = 0; i < m_primitives.size(); i++) {
            // Evita checar com o próprio triangulo
            if (i == closest_idx) {
                continue;
            }
            
            const Triangle &triangle = m_primitives[i];
            if (auto t = triangle.ray_intersect(hit_point, to_light)) {
                // Caso o ponto de interseção seja após o ponto de posição da luz
                // essa interseção não deve ser contada
                if (t > light_t) {
                    continue;
                }
                hit = true;
                break;
            }
        }

        const float intensity = normal.dot(to_light);
        
        // Caso o produto seja menor que 0 a luz esta no lado contrario ao triângulo
        // e portanto não deve interferir na intensidade
        if (!hit && intensity > 0) {
            intensities.push_back(intensity);
        } else {
            intensities.push_back(0);
        }
    }

    // Calulo final da cor, considerando luz ambiente, a cor do objeto e a cor da luz e sua intensidade
    Color result = closest_triangle.color * m_ambient;
    for (int i = 0; i < m_lights.size(); i++) {
        result = result + (m_lights[i].color * closest_triangle.color * (intensities[i] * (1.0F - m_ambient)));
    }
    result.saturate(); // Evita overflow

    return result;
}
