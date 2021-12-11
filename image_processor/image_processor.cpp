#include "image_processor.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include "../shared/shm.h"

using namespace boost::interprocess;


image_processor::image_processor() {

}

image_processor::~image_processor() {

}

void image_processor::run() {

    std::cout << "Image processor started!" << std::endl;
    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove("shared_memory_1"); }
        ~shm_remove(){ shared_memory_object::remove("shared_memory_1"); }
    } remover;

    Shm shm;

    for (;;) {
        bool got_some_data = false;
        while (!got_some_data) {
            got_some_data = shm.named_object_exists("unprocessed");
            if (!got_some_data) {
                sleep(2u);
            }
        }

        std::stringstream out;
        shm.read_named_object_to_outstream(out, "unprocessed");
        shm.remove_named_object("unprocessed");

        boost::gil::rgb16_image_t img;
        boost::gil::read_and_convert_image(out, img, boost::gil::png_tag());
        auto img_view_processed = color_converted_view< boost::gil::gray8_pixel_t>(view(img));
        std::stringstream in;
        boost::gil::write_view(in,  img_view_processed, boost::gil::png_tag());
        shm.write_named_object_from_instream(in, "processed");
    }
}