cmake_minimum_required(VERSION 3.10)

project(ExampleMicroservice)

include_directories(.)
include_directories(../DataModel/out/cpp)
include_directories(../Dependencies/redis-plus-plus/src)
include_directories(../Dependencies/redis/deps)
include_directories(../Dependencies/redis-plus-plus/src/sw/redis++/cxx11)
include_directories(../Dependencies/redis-plus-plus/src/sw/redis++/no_tls)
include_directories(../Dependencies/protobuf/src)
include_directories(../ExampleMicroservice/MicroserviceLib)
    
if (UNIX)
    include_directories(/usr/local/include)
endif (UNIX)

add_executable(ExampleMicroservice ExampleMicroservice/ExampleMicroservice.cpp)

file(GLOB DataModel ../DataModel/out/cpp/*.pb.cc)
add_library(MicroserviceLib OBJECT ${DataModel} MicroserviceLib/impl/Microservice.cpp)
target_link_libraries(ExampleMicroservice PUBLIC MicroserviceLib)

if (UNIX)
    target_link_libraries(ExampleMicroservice PUBLIC pthread)
    target_link_libraries(ExampleMicroservice PUBLIC atomic)
    target_link_libraries(ExampleMicroservice PUBLIC uuid)
    find_library(PROTOBUF_LIBRARY libprotobuf.a HINTS ../Dependencies/protobuf/src/.libs/)
    find_library(LIB_REDIS_PP libredis++.a HINTS ../Dependencies/redis-plus-plus/build/)
    find_library(LIB_HIREDIS libhiredis.a HINTS ../Dependencies/redis/deps/hiredis/)
endif (UNIX)

if (MSVC)
    find_library(PROTOBUF_LIBRARY libprotobuf.lib HINTS ../Dependencies/protobuf/cmake/build/Release/)
    find_library(LIB_REDIS_PP redis++_static.lib HINTS ../Dependencies/redis-plus-plus/build/Release/)
    find_library(LIB_HIREDIS hiredis.lib HINTS ../Dependencies/redis/deps/hiredis/build/Release/)
endif(MSVC)

target_link_libraries(ExampleMicroservice PUBLIC ${PROTOBUF_LIBRARY})
target_link_libraries(ExampleMicroservice PUBLIC ${LIB_REDIS_PP})
target_link_libraries(ExampleMicroservice PUBLIC ${LIB_HIREDIS})
