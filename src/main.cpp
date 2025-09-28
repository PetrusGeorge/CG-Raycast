#include <GL/glut.h>
#include "Renderer.h"
#include "Scenes.h"

int main(int argc, char **argv) {
    if (argc == 2) {
        Scenes::load_obj(argv[1]);
    } else {
        Scenes::construct_towers();
    }

    Renderer::get_instance().init(argc, argv);
    return 0;
}
