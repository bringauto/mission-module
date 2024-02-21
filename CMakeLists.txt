CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(MissionModule CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability

INCLUDE(CheckPIESupported)
CHECK_PIE_SUPPORTED()
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(MISSION_MODULE_VERSION 1.0.0)

OPTION(BRINGAUTO_SYSTEM_DEP "Enable system dependencies" OFF)

FIND_PACKAGE(CMLIB
        COMPONENTS CMDEF CMUTIL STORAGE
        REQUIRED
)
IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE("cmake/Dependencies.cmake")
ENDIF ()


FIND_PACKAGE(CPPRESTSDK REQUIRED)

FIND_PACKAGE(Protobuf 3.21.12 REQUIRED)

get_property(importTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)

FIND_PACKAGE(fleet-protocol-interface REQUIRED )
get_property(importTargetsAfter DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
list(REMOVE_ITEM importTargetsAfter ${importTargets})   # Remove those imported before
message("After ${importTargetsAfter}")

message("Fleet Protocol Targets:")
get_target_property(target1 common-headers-interface INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(target2 internal-client-interface INTERFACE_INCLUDE_DIRECTORIES)
# Add more targets as needed
message("   ${target1}")
message("   ${target2}")

ADD_SUBDIRECTORY("lib/fleet-v2-http-client")
ADD_SUBDIRECTORY("lib/fleet-protocol-cxx")


# Protobuf mission payload
SET(Protobuf_USE_STATIC_LIBS ON)
FILE(GLOB_RECURSE protobuf_mission_cpp_files "lib/protobuf-mission-module/*")
ADD_LIBRARY(protobuf_mission_lib STATIC ${protobuf_mission_cpp_files})
TARGET_INCLUDE_DIRECTORIES(protobuf_mission_lib PUBLIC "lib/protobuf-mission-module/")
TARGET_LINK_LIBRARIES(protobuf_mission_lib PUBLIC protobuf::libprotobuf)

FILE(GLOB_RECURSE HEADERS "include/*")

ADD_LIBRARY(mission_module_hpp INTERFACE ${HEADERS})
TARGET_INCLUDE_DIRECTORIES(mission_module_hpp INTERFACE "include/")
TARGET_LINK_LIBRARIES(mission_module_hpp INTERFACE protobuf_mission_lib)

FILE(GLOB_RECURSE SOURCES "source/*")

ADD_LIBRARY(mission_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET mission_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(mission_module_sources PUBLIC mission_module_hpp module-maintainer-module-gateway-interface module-maintainer-external-server-interface protobuf_mission_lib
        protobuf-cpp-interface fleet-protocol::cxx_helpers CppRestOpenAPIClient)


ADD_LIBRARY(mission_module_gateway SHARED "source/module_manager.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(mission_module_gateway PRIVATE mission_module_hpp mission_module_sources module-maintainer-external-server-interface common-headers-interface
        module-maintainer-module-gateway-interface)

ADD_LIBRARY(mission_external_server SHARED "source/external_server_api.cpp" "source/device_management.cpp")
TARGET_LINK_LIBRARIES(mission_external_server PRIVATE mission_module_hpp mission_module_sources module-maintainer-external-server-interface common-headers-interface
        module-maintainer-module-gateway-interface fleet-protocol::cxx_helpers CppRestOpenAPIClient protobuf-cpp-interface)
