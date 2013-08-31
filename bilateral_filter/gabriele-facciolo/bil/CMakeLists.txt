cmake_minimum_required(VERSION 2.8)


# SETUP A MAKE CLEAN TO CLEAN EVERYTHING
#
#
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES 
      "CMakeCache.txt;CMakeFiles;Makefile;cmake_install.cmake")


# IIO
if (NOT TARGET IIOLIB)
   include_directories("iio")
   add_subdirectory(iio)
endif() 
SET( LIBS ${LIBS} IIOLIB m)


# OPENMP
FIND_PACKAGE( OpenMP )
if(OPENMP_FOUND)
   message("OPENMP FOUND")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -Wall")


add_executable (bilateral_filter bilateral_filter.c main.c)
target_link_libraries(bilateral_filter ${LIBS} )
