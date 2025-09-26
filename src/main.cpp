#include <GL/glut.h>
#include <cmath>

#include "Renderer.h"

void construct_scene() {
    // Função auxiliar para criar cubos
    auto create_cube = [](const Vector3 &center, float size, const Color &color) {
        std::vector<Triangle> triangles;
        const float half_size = size / 2.0F;

        const Vector3 vertices[8] = {Vector3(center.x - half_size, center.y - half_size, center.z - half_size),
                                     Vector3(center.x + half_size, center.y - half_size, center.z - half_size),
                                     Vector3(center.x + half_size, center.y + half_size, center.z - half_size),
                                     Vector3(center.x - half_size, center.y + half_size, center.z - half_size),
                                     Vector3(center.x - half_size, center.y - half_size, center.z + half_size),
                                     Vector3(center.x + half_size, center.y - half_size, center.z + half_size),
                                     Vector3(center.x + half_size, center.y + half_size, center.z + half_size),
                                     Vector3(center.x - half_size, center.y + half_size, center.z + half_size)};

        const int faces[12][3] = {
            {0, 1, 2}, {0, 2, 3}, // front
            {4, 6, 5}, {4, 7, 6}, // back
            {0, 4, 5}, {0, 5, 1}, // bottom
            {2, 6, 7}, {2, 7, 3}, // top
            {0, 3, 7}, {0, 7, 4}, // left
            {1, 5, 6}, {1, 6, 2}  // right
        };

        triangles.reserve(12);
        for (const auto &face : faces) {
            triangles.emplace_back(vertices[face[0]], vertices[face[1]], vertices[face[2]], color);
        }
        return triangles;
    };

    Renderer &render = Renderer::get_instance();

    // Constroi cubos para cena
    render.add_object(create_cube(Vector3(0, -2, -2), 2.0F, Color(0.8F, 0.3F, 0.3F)));
    render.add_object(create_cube(Vector3(3, -2, -4), 1.0F, Color(0.3F, 0.3F, 0.8F)));
    render.add_object(create_cube(Vector3(-3, -1, -1), 1.5F, Color(0.8F, 0.8F, 0.3F)));

    // Chão
    render.add_triangle({Vector3(-10, -5, -10), Vector3(10, -5, -10), Vector3(0, -5, 10), Color(0.4F, 0.6F, 0.4F)});

    // Luzes
    render.add_light(Light({0.0, 10.0, 0.0}));
    // render.add_light(Light({0.0, 10.5, 0.0}, {1.0, 0.0, 1.0}));
}

int main(int argc, char **argv) {

    construct_scene();
    Renderer::get_instance().init(argc, argv);
}
