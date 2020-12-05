cmake_minimum_required(VERSION 3.5)
project(barcode_recognition_app C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add COMPILER specific flags
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # using visual studio
    message(STATUS "Found compiler Visual Studio")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG} "/Od /TC /Zc:preprocessor")
    set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE} "/O2 /TC /Zc:preprocessor")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # using GCC
    message(STATUS "Found compiler GCC")
    set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG} "-std=c99 -fgnu89-inline -pg -D_DEBUG -Wall -Wno-int-conversion -Wno-parentheses -Wno-unused-variable -Wno-unknown-pragmas -Wno-unused-result -Wno-format -Wno-format-extra-args")
    set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE} "-std=c99 -O2 -fgnu89-inline -Wno-unused-result")
else ()
    message(FATAL "Unsupported compiler")
endif()

set(CMAKE_C_FLAGS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG})
set(CMAKE_C_FLAGS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE})

# set the output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bins")

set(CMAKE_BUILD_TYPE Release)

# set header and source files for the test
set(IMLAB_LIBRARY_DIRECTORY "../../imlab_library/library")

include_directories(${IMLAB_LIBRARY_DIRECTORY}/include/)
link_directories(${IMLAB_LIBRARY_DIRECTORY}/lib/)

# get all .c files under each sub folder
file(GLOB test_files *.c)

#for each c file in the samples directory
foreach(test_target ${test_files})

    #get the test source file name from the path
    get_filename_component(test_source_name ${test_target} NAME)
    message(STATUS ${test_source_name})

    # create an executable_name via omitting the posix
    string(REPLACE ".c" "" executable_name ${test_source_name})
    add_executable(${executable_name} ${test_source_name})

    # Make sure YourLib is linked to each app
    target_link_libraries(${executable_name} imlab)
    if(UNIX)
        target_link_libraries(${executable_name} m)
    endif(UNIX)
endforeach(test_target ${test_samples})
