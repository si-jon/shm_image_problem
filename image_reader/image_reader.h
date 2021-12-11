#ifndef INCLUDE_IMAGE_READER_H
#define INCLUDE_IMAGE_READER_H

#include <string>

class image_reader {

    public:
        image_reader();
        ~image_reader();
        
        int read(const std::string& file_path);
};

#endif