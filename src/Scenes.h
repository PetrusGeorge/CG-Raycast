#ifndef SCENES_H
#define SCENES_H

#include "Renderer.h"
#include <vector>

class Scenes {
public:
    static void construct_cubes();
    static void construct_towers();
    static void load_obj(const char* path);
};

#endif
