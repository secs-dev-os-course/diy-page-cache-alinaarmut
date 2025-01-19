cmake_minimum_required(VERSION 3.12)
project(app
        VERSION 0.0.0
        DESCRIPTION "Application module"
        LANGUAGES C CXX
)

cmake_policy(SET CMP0076 NEW)
add_executable(app
          cache.cpp
          api.cpp
          include/api.h
        main.cpp
        include/cache.h)
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR})

#target_sources(app
#        PRIVATE
#        app.c # or app.cpp)
