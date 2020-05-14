cmake_policy(SET CMP0048 NEW)
project(fnvsoa-fork-integration-tests VERSION 0.1 LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10.2)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Threads)

add_executable(app semaphore_test.cpp)
target_link_libraries(app Boost::program_options Threads::Threads)
set_target_properties(app PROPERTIES CXX_STANDARD 14)

# vim: ts=2 sw=2 sts=0 expandtab ff=unix :
