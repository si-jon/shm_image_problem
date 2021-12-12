#include "image_processor.h"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include "../common/shared_memory_wrapper.h"

using namespace boost::interprocess;


image_processor::image_processor() {
}

image_processor::~image_processor() {
}

void image_processor::run() {

    std::cout << "Image processor running" << std::endl;

    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove("shared_memory_1"); }
        ~shm_remove(){ shared_memory_object::remove("shared_memory_1"); }
    } remover;
    shared_memory_wrapper shm;

    for (;;) {
        // Wait for image in shared memory
        bool exists_image_to_process = false;
        while (!exists_image_to_process) {
            exists_image_to_process = shm.named_object_exists("unprocessed");
            if (!exists_image_to_process) {
                shm.wait();
            }
        }

        std::stringstream out;
        shm.read_named_object_to_outstream(out, "unprocessed");
        shm.remove_named_object("unprocessed");

        boost::gil::rgb16_image_t img;
        try {
            boost::gil::read_and_convert_image(out, img, boost::gil::png_tag());
            auto img_view_processed = color_converted_view< boost::gil::gray8_pixel_t>(view(img));
            std::stringstream in;
            boost::gil::write_view(in,  img_view_processed, boost::gil::png_tag());
            shm.write_named_object_from_instream(in, "processed");
        }
        catch (std::exception const&  ex) {
            std::cout << "Caught exception: " << ex.what() << std::endl;
            shm.notify();
        }
    }
}