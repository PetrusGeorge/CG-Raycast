#ifndef SCENES_H
#define SCENES_H

#include "Renderer.h"
#include <vector>

namespace Scenes {
    void construct_cubes();
    void construct_towers();
    void construct_walls();
    void load_obj(const char* path);
};

#endif
