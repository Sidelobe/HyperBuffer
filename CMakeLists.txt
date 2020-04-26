cmake_minimum_required(VERSION 3.13)

#  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
#  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
#  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
###########################################################

# Utillity
# Recursively get all .h/.hpp header from the target's sources and assign their paths as include directories
function(assign_include_dirs_from_sources _target)
    get_target_property(_sources ${_target} SOURCES)
    list(FILTER _sources INCLUDE REGEX .+\\.h.*$)
    set(_include_dirs)
    foreach(_headers IN ITEMS ${_sources})
        get_filename_component(_header_path "${_headers}" PATH)
        list(APPEND _include_dirs ${_header_path})
    endforeach()
    list(REMOVE_DUPLICATES _include_dirs)
    target_include_directories(${_target} PRIVATE ${_include_dirs})
endfunction()

# build target with code coverage information
function(generate_code_coverage_data _target)
    target_compile_options(${_target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-fprofile-arcs -ftest-coverage>
            $<$<COMPILE_LANGUAGE:C>:-fprofile-arcs -ftest-coverage>)
    target_link_options(${_target} PRIVATE -fprofile-arcs -ftest-coverage)
endfunction()

#### MAIN
project(HyperBuffer)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SOURCE TARGET
file(GLOB_RECURSE source "source/*.h*" "source/*.c*")

set(LIB_NAME "HyperBuffer")

# TEST TARGET
set (TEST_NAME "${LIB_NAME}Test")
file(GLOB_RECURSE source_test "test/tests/*.c*" "test/tests/*.c*")
add_executable(${TEST_NAME} ${source} ${source_test})
assign_include_dirs_from_sources(${TEST_NAME})
generate_code_coverage_data(${TEST_NAME})

# Add external-utils
target_sources(${TEST_NAME} PRIVATE "test/main.cpp")
target_include_directories(${TEST_NAME} PRIVATE test/external-utils)

# Add Memory Sentinel
add_subdirectory(test/external-utils/MemorySentinel)
target_include_directories(${TEST_NAME} PRIVATE test/external-utils/MemorySentinel/source)
target_link_libraries(${TEST_NAME} MemorySentinel)

# Compiler Setitings
target_compile_options(${TEST_NAME} PRIVATE
    # clang/GCC warnings
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    #-Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wmisleading-indentation # warn if identation implies blocks where blocks do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
    -Wnull-dereference # warn if a null dereference is detected
    -Wuseless-cast # warn if you perform a cast to the same type
    #-Wdouble-promotion # warn if float is implicit promoted to double
    >
    # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:
    "/W4" >
)



## ENABLE THE USE OF CTEST 
include("test/external-utils/catch2/ParseAndAddCatchTests.cmake")
#include(CTest) # this will generate lots of additional targets
enable_testing()
ParseAndAddCatchTests(${TEST_NAME})
