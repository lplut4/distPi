cmake_minimum_required(VERSION 3.10)

project(ExampleMicroservice)

include_directories(../Dependencies/redis-plus-plus/src)
include_directories(../Dependencies/redis/deps)
include_directories(../Dependencies/redis-plus-plus/src/sw/redis++/cxx11)
include_directories(../Dependencies/redis-plus-plus/src/sw/redis++/no_tls)
include_directories(../Dependencies/protobuf/src)
include_directories(../DataModel/out/cpp)

if (UNIX)
	include_directories(/usr/local/include)
endif (UNIX)

add_library(mylib OBJECT ../DataModel/out/cpp/TimeSpec.pb.cc ../DataModel/out/cpp/LogMessage.pb.cc)
add_executable(ExampleMicroservice ExampleMicroservice/ExampleMicroservice.cpp)

if (UNIX)
	target_link_libraries(ExampleMicroservice PUBLIC pthread)
	target_link_libraries(ExampleMicroservice PUBLIC atomic)
	target_link_libraries(ExampleMicroservice PUBLIC mylib)
	target_link_libraries(ExampleMicroservice PUBLIC uuid)
endif (UNIX)

find_library(PROTOBUF_LIBRARY libprotobuf.a HINTS ../Dependencies/protobuf/src/.libs/)
target_link_libraries(ExampleMicroservice PUBLIC ${PROTOBUF_LIBRARY})

find_library(LIB_REDIS_PP libredis++.a HINTS ../Dependencies/redis-plus-plus/build/)
target_link_libraries(ExampleMicroservice PUBLIC ${LIB_REDIS_PP})

find_library(LIB_HIREDIS libhiredis.a HINTS ../Dependencies/redis/deps/hiredis/)
target_link_libraries(ExampleMicroservice PUBLIC ${LIB_HIREDIS})