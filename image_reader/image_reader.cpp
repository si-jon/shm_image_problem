#include "image_reader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <experimental/filesystem>
#include "../shared/shm.h"

image_reader::image_reader()  {

}

image_reader::~image_reader() {

}

int image_reader::read(const std::string& file_path_in) {

    std::cout << "Read image: " << file_path_in << std::endl;
    std::ifstream in(file_path_in, std::ios::binary);
    if (!in.is_open()) {
        std::cout << file_path_in << " could not be opened" << std::endl;
        return 1;
    }

    Shm shm;
    shm.write(in);

    std::experimental::filesystem::path inpath {file_path_in};
    std::string filename = inpath.stem().string() + "_grayscale";

    auto file_path_out = inpath;
    file_path_out.replace_filename(filename);


    std::ofstream out(file_path_out, std::ios_base::out  | std::ios::binary);
    shm.read(out);

    std::cout << "Write image: " << file_path_out.string() << std::endl;
    return 0;
}