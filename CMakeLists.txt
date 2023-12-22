CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(MissionModule CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability

INCLUDE(CheckPIESupported)
CHECK_PIE_SUPPORTED()
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(MISSION_MODULE_VERSION 1.0.0)

FIND_PACKAGE(CMLIB
        COMPONENTS CMDEF CMUTIL
        REQUIRED
)


IF (NOT TARGET common_headers)
    ADD_SUBDIRECTORY("lib/fleet-protocol")
ENDIF ()


FILE(GLOB_RECURSE HEADERS "include/*")

ADD_LIBRARY(mission_module_hpp INTERFACE ${HEADERS})
TARGET_INCLUDE_DIRECTORIES(mission_module_hpp INTERFACE "include/")

FILE(GLOB_RECURSE SOURCES "source/*")

ADD_LIBRARY(mission_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET mission_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(mission_module_sources PUBLIC mission_module_hpp module_gateway_maintainer external_server fleet-protocol::protobuf_cpp)

ADD_LIBRARY(mission_module_gateway SHARED "source/module_manager.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(mission_module_gateway PRIVATE mission_module_hpp mission_module_sources external_server common_headers
        module_gateway_maintainer)

