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

    managed_shared_memory managed_shm {open_only, "shared_memory_1"};
    std::cout << "Size           = " << managed_shm.get_size() << std::endl;
    std::cout << "Free memory    = " << managed_shm.get_free_memory() << std::endl;
    std::cout << "Named objects  = " << managed_shm.get_num_named_objects() << std::endl;
    std::cout << "Unique objects = " << managed_shm.get_num_unique_objects() << std::endl;
    
    typedef allocator <char, managed_shared_memory::segment_manager> CharAllocator;
    typedef basic_string<char, std::char_traits<char>, CharAllocator> string;
    

    interprocess_mutex *mtx = managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition *cnd = managed_shm.find_or_construct<interprocess_condition>("cnd")();
    {
        scoped_lock<interprocess_mutex> lock{*mtx};
        string *input = managed_shm.find_or_construct<string>("StringInput")("", managed_shm.get_segment_manager());
        std::cout << "StringInput" << *input << std::endl;
        input->insert(input->length(), "Hello, world!");
        std::cout << *input << std::endl;

        cnd->notify_all();
        std::cout << "lock" << std::endl;
        cnd->wait(lock);


        auto output = managed_shm.find<string>("StringOutput");

        std::cout << "Input: " << *input << std::endl;
        std::cout << "Output: " << *output.first << std::endl;
        output.first->clear();
    }
    return 0;
}
