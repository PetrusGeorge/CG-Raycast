#include <GL/glut.h>
#include "Renderer.h"
#include "Scenes.h"

int main(int argc, char **argv) {
    if (argc >= 2) {
        std::string command = argv[1];

        if (command == "obj" && argc == 3) {
            Scenes::load_obj(argv[2]);
        } else if (command == "towers") {
            Scenes::construct_towers();
        } else if (command == "walls") {
            Scenes::construct_walls();
        } else if (command == "cubes") {
            Scenes::construct_cubes();
        } else {
            std::cout << "Uso: " << argv[0] << " <cena> " << std::endl;
            std::cout << "Comandos disponíveis:" << std::endl;
            std::cout << "  obj <arquivo>    - Carrega cena de arquivo OBJ" << std::endl;
            std::cout << "  towers           - Constrói cena com torres" << std::endl;
            std::cout << "  walls            - Constrói cena com paredes" << std::endl;
            std::cout << "  cubes            - Constrói cena com cubos" << std::endl;
            return 1;
        }
    } else {
        Scenes::construct_cubes();
    }

    Renderer::get_instance().init(argc, argv);
    return 0;
}
