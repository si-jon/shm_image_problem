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
    m_shared_segment {open_or_create, "shared_memory_1", (1048575)}
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

long int get_stream_length(std::istream& in) {
    in.seekg (0, in.end);
    long int length = in.tellg();
    in.seekg (0, in.beg);
    return length;
}

void Shm::write_from_instream(std::istream& in){

    try {
        scoped_lock<interprocess_mutex> lock{*m_mutex};
        auto length = get_stream_length(in);
        auto buffer = m_shared_segment.construct<char>("unprocessed")[length]('\0');
        bufferstream buff_stream(buffer, length);
        buff_stream << in.rdbuf();
        assert(buff_stream.good());
        m_cond_read_and_write->notify_all();
        m_cond_read_and_write->wait(lock);
    } 
    catch (std::exception const&  ex) {
        std::cout << ex.what() << std::endl;
    }
}

void Shm::read_to_outstream(std::ostream& out){
    scoped_lock<interprocess_mutex> lock{*m_mutex};

    auto output = m_shared_segment.find<char>("processed");
    bufferstream buff_stream(output.first, output.second);
    assert(buff_stream.good());
    out << buff_stream.rdbuf();
}

void Shm::write_named_object_from_instream(std::istream& in, const char* name){

    try {
        scoped_lock<interprocess_mutex> lock{*m_mutex};
        auto length = get_stream_length(in);
        auto buffer = m_shared_segment.construct<char>(name)[length]('\0');
        bufferstream buff_stream(buffer, length);
        buff_stream << in.rdbuf();
        assert(buff_stream.good());
        m_cond_read_and_write->notify_all();
        m_cond_read_and_write->wait(lock);
    } 
    catch (std::exception const&  ex) {
        std::cout << ex.what() << std::endl;
    }
}

void Shm::read_named_object_to_outstream(std::ostream& out, const char* name){
    scoped_lock<interprocess_mutex> lock{*m_mutex};

    auto output = m_shared_segment.find<char>(name);
    bufferstream buff_stream(output.first, output.second);
    assert(buff_stream.good());
    out << buff_stream.rdbuf();
}

bool Shm::named_object_exists(const char *name) {
    return (m_shared_segment.find<char>(name).first != 0);
}

void Shm::remove_named_object(const char *name) {
    scoped_lock<interprocess_mutex> lock{*m_mutex};
    m_shared_segment.destroy<char>(name);
}