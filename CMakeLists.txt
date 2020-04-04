cmake_minimum_required(VERSION 3.9)

# HYPERBUFFER

###########################################################
# MAIN
project(HyperBuffer)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source "source/*.h*" "source/*.c*")
set (LIB_NAME "${CMAKE_PROJECT_NAME}")

# TEST TARGET
set (TEST_NAME "${LIB_NAME}Test")
file(GLOB_RECURSE source_test "test/*.h*" "test/*.c*")
add_executable(${TEST_NAME} ${source} ${source_test})

target_include_directories(${TEST_NAME} PRIVATE source)
target_include_directories(${TEST_NAME} PRIVATE test)
target_include_directories(${TEST_NAME} PRIVATE test/catch2)

get_target_property(allsources ${TEST_NAME} SOURCES)
foreach(_source IN ITEMS ${allsources})
    if (IS_ABSOLUTE "${_source}")
        file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
    else()
       set(_source_rel "${_source}")
    endif()
    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

target_compile_options(${TEST_NAME} PRIVATE
    # clang/GCC warnings
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a
    # parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
    # non-virtual destructor. This helps catch hard to
    # track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual
    # function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wmisleading-indentation # warn if identation implies blocks where blocks
    # do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were
    # probably wanted
    -Wnull-dereference # warn if a null dereference is detected
    -Wuseless-cast # warn if you perform a cast to the same type
    -Wdouble-promotion # warn if float is implicit promoted to double
    >
    # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:
    "/Wall" >
)

## ENABLE THE USE OF CTEST 
include("test/catch2/ParseAndAddCatchTests.cmake")
#include(CTest) # this will generate lots of additional targets
enable_testing()
ParseAndAddCatchTests(${TEST_NAME})
