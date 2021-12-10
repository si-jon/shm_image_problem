#include "shm.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <sstream>
#include <cstddef>

using namespace boost::interprocess;

Shm::Shm():
    mManagedSharedMemory {open_only, "shared_memory_1"}
    //mManagedSharedMemory {open_or_create, "shared_memory_1", 158717}
{
   //mMutex = mManagedSharedMemory.find_or_construct<interprocess_mutex>("mtx")();
   //mCond = mManagedSharedMemory.find_or_construct<interprocess_condition>("cnd")();
}

Shm::~Shm() {
    //shared_memory_object::remove("shared_memory_1");
}

void Shm::print() {
    std::cout << "Size           = " << mManagedSharedMemory.get_size() << std::endl;
    std::cout << "Free memory    = " << mManagedSharedMemory.get_free_memory() << std::endl;
    std::cout << "Named objects  = " << mManagedSharedMemory.get_num_named_objects() << std::endl;
    std::cout << "Unique objects = " << mManagedSharedMemory.get_num_unique_objects() << std::endl;
}

void Shm::dostuff2(std::istream& infile, std::ostream& outfile) {
    interprocess_mutex *mtx = mManagedSharedMemory.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition *cnd = mManagedSharedMemory.find_or_construct<interprocess_condition>("cnd")();

    scoped_lock<interprocess_mutex> lock{*mtx};

    infile.seekg (0, infile.end);
    auto length = infile.tellg();
    std::cout << "length: " << length << std::endl;
    infile.seekg (0, infile.beg);

    auto buffer = mManagedSharedMemory.construct<char>("unprocessed")[length]('\0');
    bufferstream bs(buffer, length);
    
    bs << infile.rdbuf();

    assert(bs.good());

    std::cout << "Done, notify!" << std::endl;
    cnd->notify_all();
    cnd->wait(lock);
    {
        auto output = mManagedSharedMemory.find<char>("processed");
        bufferstream bs2(output.first, output.second);

        assert(bs2.good());

        outfile << bs2.rdbuf();

        std::cout << "Done reading, notify!" << std::endl;
        cnd->notify_all();
    }
}