CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(MissionModule)

SET(CMAKE_CXX_STANDARD 20)

ADD_SUBDIRECTORY(lib/fleet-protocol)

FILE(GLOB_RECURSE mission_module_cpp_files "source/*")
ADD_LIBRARY(mission_module SHARED ${mission_module_cpp_files})
TARGET_INCLUDE_DIRECTORIES(mission_module PUBLIC "include/")
TARGET_LINK_LIBRARIES(mission_module PUBLIC fleet-protocol::common_headers
                                            module_gateway_maintainer
                                            fleet-protocol::protobuf_cpp
                                            )