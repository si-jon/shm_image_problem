#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace boost::interprocess;

/*
    Todo:
        Create shared memory section
        Wait until data is in shared memory
            Read image from shared memory
            Process image
            Add processed image to shared section
        When closed
            Remove shared memory section

    Basic:
        Create shared memory section
        Wait until data is in shared memory
            Read string
            Change string
            Add string to shared memory
        When closed
            Remove shared memory section
*/

int main(int, char**) {

    for (;;) {

        struct shm_remove
        {
            shm_remove() { shared_memory_object::remove("shared_memory_1"); }
            ~shm_remove(){ shared_memory_object::remove("shared_memory_1"); }
        } remover;

        std::cout << "Image processor started!" << std::endl;

        managed_shared_memory managed_shm {open_or_create, "shared_memory_1", (158717 * 3)};
        std::cout << "Size           = " << managed_shm.get_size() << std::endl;
        std::cout << "Free memory    = " << managed_shm.get_free_memory() << std::endl;
        std::cout << "Named objects  = " << managed_shm.get_num_named_objects() << std::endl;
        std::cout << "Unique objects = " << managed_shm.get_num_unique_objects() << std::endl;
        interprocess_mutex *mtx = managed_shm.find_or_construct<interprocess_mutex>("mtx")();
        interprocess_condition *cnd = managed_shm.find_or_construct<interprocess_condition>("cnd")();

        {
            scoped_lock<interprocess_mutex> lock{*mtx};
            std::cout << "Notify!" << std::endl;
            cnd->notify_all();
            cnd->wait(lock);
            auto input = managed_shm.find<char>("unprocessed");
            bufferstream bs(input.first, input.second);
            assert(bs.good());

            boost::gil::rgb16_image_t img;
            boost::gil::read_image(bs, img, boost::gil::png_tag());

            std::cout << img.dimensions().x << " x " << img.dimensions().y << std::endl;
            auto img_processed = color_converted_view< boost::gil::gray8_pixel_t>(view(img));
            std::cout << "img_processed.size(): " << img_processed.size() << std::endl;


            bool hax = true;
            if (hax) {
                std::cout << "HAX!!" << std::endl;

                boost::gil::write_view("temp.png",  img_processed, boost::gil::png_tag());
                std::ifstream infile("temp.png", std::ios::binary);
                infile.seekg (0, infile.end);
                auto length = infile.tellg();
                std::cout << "infile length: " << length << std::endl;
                infile.seekg (0, infile.beg);

                auto buffer = managed_shm.construct<char>("processed")[length]('\0');
                bufferstream bs2(buffer, length);

                bs2 << infile.rdbuf();
            }
            else {
                std::stringstream out_buffer(std::ios_base::out | std::ios_base::binary);
                boost::gil::write_view(out_buffer, img_processed, boost::gil::png_tag());

                
                std::stringstream in_buffer(std::ios_base::in | std::ios_base::binary);
                in_buffer << out_buffer.rdbuf();

                in_buffer.seekg (0, in_buffer.end);
                auto length = in_buffer.tellg();
                std::cout << "in_buffer length: " << length << std::endl;
                in_buffer.seekg (0, in_buffer.beg);
            
                auto buffer = managed_shm.construct<char>("processed")[length]('\0');
                bufferstream bs2(buffer, length);

                bs2 << in_buffer.rdbuf();
            }

            std::cout << "Done, notify!" << std::endl;
            cnd->notify_all();
            cnd->wait(lock);


        }
    }

    return 0;
}
