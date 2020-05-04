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
project(HyperBuffer
        VERSION 1.0.1
        DESCRIPTION "A header-only C++ data structure to manage multi-dimensional data"
        HOMEPAGE_URL "https://github.com/Sidelobe/Hypbuffer")

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CODE_COVERAGE OFF CACHE BOOL "Build with instrumentation and code coverage")

# SOURCE TARGET --- "interface", since we have a header-only library
file(GLOB_RECURSE source "source/*.h*")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${source})
target_include_directories(${PROJECT_NAME} INTERFACE ${source})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)

# TEST TARGET
set (TEST_NAME "${PROJECT_NAME}Test")
file(GLOB_RECURSE source_test "test/tests/*.c*")
file(GLOB main_file "test/main.cpp" "test/TestCommon.hpp")
list(APPEND source_test ${main_file})
add_executable(${TEST_NAME} ${source_test})
assign_include_dirs_from_sources(${TEST_NAME})
target_include_directories(${TEST_NAME} PRIVATE "source")
target_link_libraries(${TEST_NAME} PUBLIC ${PROJECT_NAME})

# create source groups
source_group("Sources" FILES ${source})
source_group("Tests" FILES ${source_test})

if(CODE_COVERAGE)
    generate_code_coverage_data(${TEST_NAME})
endif(CODE_COVERAGE)

# Add external-utils
target_include_directories(${TEST_NAME} SYSTEM PRIVATE test/external-utils)

# Add Memory Sentinel
add_subdirectory(test/external-utils/MemorySentinel)
target_include_directories(${TEST_NAME} SYSTEM PRIVATE test/external-utils/MemorySentinel/source)
target_link_libraries(${TEST_NAME} PRIVATE MemorySentinel)

# Compiler Settings
target_compile_options(${TEST_NAME} PRIVATE
    # clang/GCC warnings
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra # reasonable and standard
    -Wreturn-type # "control reaches end of non-void function"
    -Wunreachable-code
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    -Wcast-align # warn for potential performance problem casts
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wnull-dereference # warn if a null dereference is detected
    -Wold-style-cast # warn for c-style casts
    -Wimplicit-fallthrough # warn when you forget a 'break' in a  switch 
    -Wunused-variable
    -Wconversion # warn on type conversions that may lose data
    -Wno-sign-conversion # don't warn when implicit conversion changes signedness
    -Wno-sign-compare # deactivate warnings about comparisons of different sign integers (enabled by Wextra)
    >
    # additional warnings for clang only
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
    -Wdocumentation
    >
    # additional warnings for GCC
    $<$<CXX_COMPILER_ID:GNU>:
    -Wmisleading-indentation # warn if identation implies blocks where blocks do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wuseless-cast # warn if you perform a cast to the same type
    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted    
    >

    # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /D_USE_MATH_DEFINES
    /D_ITERATOR_DEBUG_LEVEL=0
    /experimental:external /external:anglebrackets # treat all #include <...> as "external" headers, don't issue warnings
    >
)

# Relax certain warnings for Test Files
file(GLOB_RECURSE files "test/tests/*")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(${files} PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast -Wno-cast-align")
elseif (MSVC)
    set_source_files_properties(${files} PROPERTIES COMPILE_FLAGS "/W0")
endif()


## ENABLE THE USE OF CTEST 
include("test/external-utils/catch2/ParseAndAddCatchTests.cmake")
#include(CTest) # this will generate lots of additional targets
enable_testing()
ParseAndAddCatchTests(${TEST_NAME})
