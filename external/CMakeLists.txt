### libjpg
#add_library(libjpg INTERFACE)
#target_include_directories(libjpg INTERFACE jpeg-9d)

### libpng
#add_library(libpng INTERFACE)
#target_include_directories(libjpg INTERFACE libpng-1.6.37)

### Boost
add_library(boost INTERFACE)
target_include_directories(boost INTERFACE boost_1_77_0)
target_compile_definitions(boost INTERFACE BOOST_DATE_TIME_NO_LIB -lpng)

