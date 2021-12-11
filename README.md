# Image in shared memory problem

## What is this?
This was an assignment for a technical interview that I've completed. The project is not really completed but it is in functional condition.

### Description
> Using modern C++ and a build system of your choice, please implement the following:
> 1. A “image_reader” process that receives a path to an image via a command line argument reads it and uses shared memory to pass it to another process
> 2. A “image_processor” process that remains idle until there is a new shared image. When that happens it turns the image into grayscale and shares it back with the “image_reader”.
> 3. Once the “image_reader” has received the grayscale image it writes it to the disk, with the same filename as the original one, but with “_grayscale” appended to the filename before the file extension. Then, the “image_reader” is expected to print out the path to the grayscale image and exit.
> 4. The “image_processor” should remain idle and in a usable state until it is manually killed.

### Limitations

* Currently only PNG format supported
* Max size of image is roughly 40MB

## How to build
Currently only been built for Linux.

### Prerequisits
In order to build the project the following is needed:
* gcc
* CMake
* libpng

Versions used during development:
* gcc: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
* CMake: version 3.16.3
* libpng-dev
    * To install: `sudo apt install libpng-dev`

### Build it
The project is built using CMake. 

```
$ git clone git@github.com:si-jon/shm_image_problem.git
$ cd shm_image_problem/
$ mkdir build
$ cd build/
$ cmake ..
$ make
```

## How to run
After successfully built, the processes are run from terminal. From build directory, first start the `image_processor` without any arguments.
```
$ ./image_processor 
Image processor running
```

Leaving `image_processor` running, open up a new terminal in the build directory. Start the `image_reader` with the path to image file to convert. 

```
$ ./image_reader path/to/image.png 
Read image: path/to/image.png 
Write image: path/to/image_grayscale.png
```

## Comment on code

### To do
* Support for more image formats
* Better abstraction of shared memory manager
* Processor should be in charge of shared memory, create it and delete it
