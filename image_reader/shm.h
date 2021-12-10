#ifndef INCLUDE_SHM_H
#define INCLUDE_SHM_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

class Shm {

  public:
    Shm();
    ~Shm();

    void print();
    void dostuff2(std::istream& infile, std::ostream& outfile);
  
  private:
    boost::interprocess::shared_memory_object mShmObject;
    boost::interprocess::managed_shared_memory mManagedSharedMemory;
    boost::interprocess::interprocess_mutex *mMutex;
    boost::interprocess::interprocess_condition *mCond;
};

#endif