#include <iostream>
#include "image_reader.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return 1;
    }

    std::string file_path_in = argv[1];
    image_reader reader {};
    reader.read(file_path_in);

    return 0;
}
