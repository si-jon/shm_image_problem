#include <fstream>
#include <iostream>
#include <sstream>
#include "shm.h"

/*
    Todo:
        Read and load image
        Load shared memory section
        Add loaded image to section
        Wait until image has been processed
        Read processed image from section
        Save processed image

    Basic:
        Create shared memory section 1
        Write a string to it
        Open shared memory section 2
        Read the string and print it
        Clear memory section 2
        Remove shared memory section 1
*/

int main(int, char**) {
    std::cout << "Image reader started!" << std::endl;

    //std::string filename("example_image.png");
    //boost::gil::rgb8_image_t img(640, 480);

    // write data into image

    //boost::gil::write_view(filename, view(img), boost::gil::png_tag());

    std::ifstream in("example_image_rgb16.png", std::ios::binary);
    std::ofstream out("example_image_rgb16_greyscale.png", std::ios_base::out  | std::ios::binary);

 //   std::stringstream temp(std::ios_base::in  | std::ios::binary);
 //   temp << img_buff.rdbuf();
    if (in.is_open()) {
        Shm shm;
        shm.print();
        shm.dostuff2(in, out);
    }
/*
    std::stringstream out_buff(std::ios::out | std::ios::binary);
    boost::gil::rgb16_image_t src;
    boost::gil::write_view(out_buff, view(src), boost::gil::png_tag());
    

    std::stringstream in_buff(std::ios::in | std::ios::binary);
    in_buff << out_buff.rdbuf();
*/
    //img_buff.seekg (0, img_buff.beg);
    //in.seekg(0, in.beg);
    /*
    boost::gil::rgb16_image_t img;
    try {
        std::cout << "Read image" << std::endl;
        boost::gil::read_image(img_buff, img, boost::gil::png_tag());
        //boost::gil::read_and_convert_image(img_buff, img, boost::gil::png_tag());

    }
    catch (const std::ios_base::failure &ib_f){
        std::cout << "what: " << ib_f.what() << std::endl;
        return 1;
    }

    std::cout << img.dimensions().x << " x " << img.dimensions().y << std::endl;

    std::ofstream out("example_image_rgb16_greyscale.png", std::ios::binary);
    boost::gil::write_view(out, view(img), boost::gil::png_tag());*/

/*
    // Funkar fint!

    std::string filename2("example_image_rgb16.png");
    std::cout << "filename = " << filename2 << std::endl;
    boost::gil::rgb16_image_t img2;
    boost::gil::read_image(filename2, img2, boost::gil::png_tag());

    std::cout << img2.dimensions().x << " x " << img2.dimensions().y << std::endl;

    boost::gil::rgb16_image_t ccv_image(img2.dimensions());
    auto test = color_converted_view< boost::gil::gray8_pixel_t>(view(img2));
    

    std::string out_filename("grayscale.png");
    boost::gil::write_view(out_filename, test, boost::gil::png_tag());
  */  



    // Shm shm;
    // shm.print();
    // shm.dostuff();

    return 0;
}
