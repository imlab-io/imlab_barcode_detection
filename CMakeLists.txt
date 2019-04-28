cmake_minimum_required(VERSION 3.5)
project(barcode_recognition_app C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set copiler flags for debug and release
set(CMAKE_C_FLAGS_DEBUG "-std=c99 -fgnu89-inline -pg -D_DEBUG -Wall -Wno-int-conversion -Wno-parentheses -Wno-unused-variable -Wno-unknown-pragmas -Wno-unused-result -Wno-format -Wno-format-extra-args")
set(CMAKE_C_FLAGS_RELEASE "-std=c99 -fgnu89-inline -O2")

# set the output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bins")

set(CMAKE_BUILD_TYPE Release)

# set header and source files for the test
set(IMLAB_LIBRARY_DIRECTORY "../../imlab_library")

include_directories(${IMLAB_LIBRARY_DIRECTORY}/include/)
link_directories(${IMLAB_LIBRARY_DIRECTORY}/lib/)

# get all .c files under each sub folder
file(GLOB test_files *.c)

#for each c file in the samples directory
foreach(test_target ${test_files})

    #get the test source file name from the path
    get_filename_component(test_source_name ${test_target} NAME)
    message(STATUS  ${test_source_name})

    # create an executable_name via omitting the posix
    string(REPLACE ".c" "" executable_name ${test_source_name})
    add_executable(${executable_name} ${test_source_name})

    # Make sure YourLib is linked to each app
    target_link_libraries(${executable_name} imlab)
    if(UNIX)
        target_link_libraries(${executable_name} m)
    endif(UNIX)
endforeach(test_target ${test_samples})
