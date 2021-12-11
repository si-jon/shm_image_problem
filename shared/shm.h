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
        void write(std::istream& infile);
        void read(std::ostream& outfile);
  
    private:
        boost::interprocess::managed_shared_memory m_shared_segment;
        boost::interprocess::interprocess_mutex *m_mutex;
        boost::interprocess::interprocess_condition *m_cond_read_and_write;
};

#endif