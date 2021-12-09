#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

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
    typedef allocator <char, managed_shared_memory::segment_manager> CharAllocator;
    typedef basic_string<char, std::char_traits<char>, CharAllocator> string;

    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove("shared_memory_1"); }
        ~shm_remove(){ shared_memory_object::remove("shared_memory_1"); }
    } remover;

    std::cout << "Image processor started!" << std::endl;

    managed_shared_memory managed_shm {open_or_create, "shared_memory_1", 1024};
    std::cout << "Size           = " << managed_shm.get_size() << std::endl;
    std::cout << "Free memory    = " << managed_shm.get_free_memory() << std::endl;
    std::cout << "Named objects  = " << managed_shm.get_num_named_objects() << std::endl;
    std::cout << "Unique objects = " << managed_shm.get_num_unique_objects() << std::endl;
    interprocess_mutex *mtx = managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition *cnd = managed_shm.find_or_construct<interprocess_condition>("cnd")();
    for (;;) {
        auto input = managed_shm.find<string>("StringInput");
        if (input.second == 0) {
            std::cout << "Sleep 1s" << std::endl;
            sleep(1);
            continue;
        }

        {
            scoped_lock<interprocess_mutex> lock{*mtx};
            std::cout << "input: " << *input.first << std::endl;

            auto output = managed_shm.find_or_construct<string>("StringOutput")("", managed_shm.get_segment_manager());
            output->insert(0, *input.first);
            output->insert(output->length(), " Added information!");
            std::cout << "output: " << *output << std::endl;
            input.first->clear();
            cnd->notify_all();
            cnd->wait(lock);
        }
    }

    return 0;
}
