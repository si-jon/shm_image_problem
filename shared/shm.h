#ifndef INCLUDE_SHM_H
#define INCLUDE_SHM_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <iostream>

class Shm {

    public:
        Shm();
        ~Shm();

        void print();
        void write_from_instream(std::istream& in);
        void read_to_outstream(std::ostream& out);

        void write_named_object_from_instream(std::istream& in, const char* name);
        void read_named_object_to_outstream(std::ostream& out, const char* name);

        bool named_object_exists(const char* name);

        void remove_named_object(const char* name);
  
    private:
        boost::interprocess::managed_shared_memory m_shared_segment;
        boost::interprocess::interprocess_mutex *m_mutex;
        boost::interprocess::interprocess_condition *m_cond_read_and_write;
};

#endif