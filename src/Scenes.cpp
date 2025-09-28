#include "Scenes.h"
#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

std::vector<Triangle> create_parallelepiped(
    const Vector3 &center,
    float width,
    float height,
    float depth,
    const Color &color
) {
    std::vector<Triangle> triangles;
    const float hx = width  * 0.5F;
    const float hy = height * 0.5F;
    const float hz = depth  * 0.5F;

    const Vector3 vertices[8] = {
        Vector3(center.x - hx, center.y - hy, center.z - hz),
        Vector3(center.x + hx, center.y - hy, center.z - hz),
        Vector3(center.x + hx, center.y + hy, center.z - hz),
        Vector3(center.x - hx, center.y + hy, center.z - hz),
        Vector3(center.x - hx, center.y - hy, center.z + hz),
        Vector3(center.x + hx, center.y - hy, center.z + hz),
        Vector3(center.x + hx, center.y + hy, center.z + hz),
        Vector3(center.x - hx, center.y + hy, center.z + hz)
    };

    const int faces[12][3] = {
        {0, 1, 2}, {0, 2, 3},
        {4, 6, 5}, {4, 7, 6},
        {0, 4, 5}, {0, 5, 1},
        {2, 6, 7}, {2, 7, 3},
        {0, 3, 7}, {0, 7, 4},
        {1, 5, 6}, {1, 6, 2}
    };

    triangles.reserve(12);
    for (const auto &face : faces) {
        triangles.emplace_back(vertices[face[0]],
                               vertices[face[1]],
                               vertices[face[2]],
                               color);
    }
    return triangles;
}

void Scenes::construct_cubes() {
    Renderer &render = Renderer::get_instance();

    render.add_object(create_parallelepiped(Vector3(0, -2, -2), 2.0F, 2.0F, 2.0F, Color(0.8F, 0.3F, 0.3F)));
    render.add_object(create_parallelepiped(Vector3(3, -2, -4), 1.0F, 1.0F, 1.0F, Color(0.3F, 0.3F, 0.8F)));
    render.add_object(create_parallelepiped(Vector3(-3, -1, -1), 1.5F, 1.5F, 1.5F, Color(0.8F, 0.8F, 0.3F)));

    render.add_triangle({Vector3(-10, -5, -10), Vector3(10, -5, -10), Vector3(0, -5, 10), Color(0.4F, 0.6F, 0.4F)});

    render.add_light(Light({0.0, 10.0, 0.0}, 0.1));
    render.add_light(Light({0.0, -4.5, 0.0}, {1.0, 0.0, 1.0}));
}

void Scenes::construct_towers() {
    Renderer &render = Renderer::get_instance();

    const float floor_size = 10.0F;
    const float floor_y = -5.0F;
    const Color floor_color (1.0F, 1.0F, 1.0F);

    render.add_triangle({Vector3(-floor_size/2, floor_y, -floor_size/2),
                        Vector3(floor_size/2, floor_y, -floor_size/2),
                        Vector3(floor_size/2, floor_y, floor_size/2),
                        floor_color});
    render.add_triangle({Vector3(-floor_size/2, floor_y, -floor_size/2),
                        Vector3(floor_size/2, floor_y, floor_size/2),
                        Vector3(-floor_size/2, floor_y, floor_size/2),
                        floor_color});

    const float tower_w = 1.5F;
    const float tower_h = 6.0F;
    const float tower_d = 1.5F;

    Vector3 tower_pos[4] = {
        Vector3(0.0F, floor_y + tower_h/2,  floor_size/3),
        Vector3(0.0F, floor_y + tower_h/2,  -floor_size/3),
        Vector3(floor_size/3, floor_y + tower_h/2, 0.0F),
        Vector3(-floor_size/3, floor_y + tower_h/2, 0.0F)
    };

    Color tower_color[4] = {
        Color(1.0F, 0.0F, 0.0F),
        Color(0.0F, 1.0F, 0.0F),
        Color(0.0F, 0.0F, 1.0F),
        Color(1.0F, 1.0F, 1.0F)
    };

    for (int i = 0; i < 4; ++i) {
        render.add_object(create_parallelepiped(tower_pos[i], tower_w, tower_h, tower_d, tower_color[i]));
    }

    Vector3 light_pos[4] = {
        Vector3(0.0F, floor_y + tower_h/2,  floor_size/2),
        Vector3(0.0F, floor_y + tower_h/2,  -floor_size/2),
        Vector3(floor_size/2, floor_y + tower_h/2, 0.0F),
        Vector3(-floor_size/2, floor_y + tower_h/2, 0.0F)
    };

    Color light_color[4] = {
        Color(0.8F, 0.25F, 0.25F),
        Color(0.25F, 0.8F, 0.25F),
        Color(0.25F, 0.25F, 0.8F),
        Color(1.0F, 1.0F, 1.0F)
    };

    render.add_light(Light({0.0, -4.5, 0.0}, 0.2));
    for (int i = 0; i < 4; ++i) {
        render.add_light(Light({light_pos[i]}, light_color[i]));
    }
}

void Scenes::load_obj(const char* path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path);

    if (!err.empty()) {
        std::cout << "Erro: " << err << std::endl;
        exit(1);
    }

    Renderer &render = Renderer::get_instance();

    for (const auto& shape : shapes) {
        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
            Vector3 triangle_vertices[3];
            for (int j = 0; j < 3; j++) {
                tinyobj::index_t idx = shape.mesh.indices[i + j];
                float x = attrib.vertices[3 * idx.vertex_index + 0];
                float y = attrib.vertices[3 * idx.vertex_index + 1];
                float z = attrib.vertices[3 * idx.vertex_index + 2];
                triangle_vertices[j] = Vector3(x, y, z);
            }

            Color triangle_color(1, 1, 1);

            if (!shape.mesh.material_ids.empty()) {
                int material_id = shape.mesh.material_ids[i / 3];
                if (material_id >= 0 && material_id < materials.size()) {
                    const auto& mat = materials[material_id];
                    triangle_color = Color(
                        mat.diffuse[0],
                        mat.diffuse[1],
                        mat.diffuse[2]
                    );
                }
            }

            render.add_triangle({
                triangle_vertices[0],
                triangle_vertices[1],
                triangle_vertices[2],
                triangle_color
            });
        }
    }
    render.add_light(Light({0.0, 10.0, 0.0}));
    render.set_camera(Camera({0.0, 0.0, 10.0}, 60.0));
}
