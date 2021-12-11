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

using namespace boost::interprocess;


image_processor::image_processor() {

}

image_processor::~image_processor() {

}

void image_processor::run() {

    for (;;) {

        struct shm_remove
        {
            shm_remove() { shared_memory_object::remove("shared_memory_1"); }
            ~shm_remove(){ shared_memory_object::remove("shared_memory_1"); }
        } remover;

        std::cout << "Image processor started!" << std::endl;
        managed_shared_memory managed_shm {open_or_create, "shared_memory_1", (1048575)};
        auto *mtx = managed_shm.find_or_construct<interprocess_mutex>("mtx")();
        auto *cnd = managed_shm.find_or_construct<interprocess_condition>("cnd")();

        bool got_some_data = false;
        while (!got_some_data) {
            got_some_data = (managed_shm.find<char>("unprocessed").first != 0);
            if (!got_some_data) {
                sleep(2u);
            }
        }
        
        {
            scoped_lock<interprocess_mutex> lock{*mtx};
            auto input = managed_shm.find<char>("unprocessed");
            bufferstream bs(input.first, input.second);
            assert(bs.good());
      
            boost::gil::rgb16_image_t img;
            boost::gil::read_and_convert_image(bs, img, boost::gil::png_tag());

            std::cout << img.dimensions().x << " x " << img.dimensions().y << std::endl;
            auto img_processed = color_converted_view< boost::gil::gray8_pixel_t>(view(img));
            
            std::cout << "img_processed.size(): " << img_processed.size() << std::endl;

            boost::gil::write_view("temp.png",  img_processed, boost::gil::png_tag());
            std::ifstream infile("temp.png", std::ios::binary);
            infile.seekg (0, infile.end);
            auto length = infile.tellg();
            std::cout << "infile length: " << length << std::endl;
            infile.seekg (0, infile.beg);

            auto buffer = managed_shm.construct<char>("processed")[length]('\0');
            bufferstream bs2(buffer, length);

            bs2 << infile.rdbuf();
            cnd->notify_all();
        }
    }
}