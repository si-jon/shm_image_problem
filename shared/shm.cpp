#include "shm.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <sstream>
#include <cstddef>

using namespace boost::interprocess;

Shm::Shm():
    m_shared_segment {open_only, "shared_memory_1"}
{
    m_mutex = m_shared_segment.find_or_construct<interprocess_mutex>("mtx")();
    m_cond_read_and_write = m_shared_segment.find_or_construct<interprocess_condition>("cnd")();
}

Shm::~Shm() {
}

void Shm::print() {
    std::cout << "Size           = " << m_shared_segment.get_size() << std::endl;
    std::cout << "Free memory    = " << m_shared_segment.get_free_memory() << std::endl;
    std::cout << "Named objects  = " << m_shared_segment.get_num_named_objects() << std::endl;
    std::cout << "Unique objects = " << m_shared_segment.get_num_unique_objects() << std::endl;
}

void Shm::write(std::istream& infile){

    infile.seekg (0, infile.end);
    auto length = infile.tellg();
    infile.seekg (0, infile.beg);

    try {
        scoped_lock<interprocess_mutex> lock{*m_mutex};
        auto buffer = m_shared_segment.construct<char>("unprocessed")[length]('\0');
        bufferstream buff_stream(buffer, length);
        buff_stream << infile.rdbuf();
        assert(buff_stream.good());
        m_cond_read_and_write->notify_all();
        m_cond_read_and_write->wait(lock);
    } 
    catch (std::exception const&  ex) {
        std::cout << ex.what() << std::endl;
    }
}

void Shm::read(std::ostream& outfile){
    scoped_lock<interprocess_mutex> lock{*m_mutex};

    auto output = m_shared_segment.find<char>("processed");
    bufferstream buff_stream(output.first, output.second);
    assert(buff_stream.good());
    outfile << buff_stream.rdbuf();
}
