CMAKE_MINIMUM_REQUIRED(VERSION 3.25 FATAL_ERROR)
PROJECT(MissionModule CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability

INCLUDE(CheckPIESupported)
CHECK_PIE_SUPPORTED()
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(MISSION_MODULE_VERSION 1.2.0)

OPTION(BRINGAUTO_INSTALL "Configure install" OFF)
OPTION(BRINGAUTO_PACKAGE "Configure package creation" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "System dependencies are used if switched to ON, packager used if OFF" OFF)

IF (BRINGAUTO_PACKAGE AND NOT BRINGAUTO_INSTALL)
    SET(BRINGAUTO_INSTALL ON
            CACHE BOOL
            "Installation forced by BRINGAUTO_PACKAGE creation"
            FORCE
    )
ENDIF ()

FIND_PACKAGE(CMLIB
        COMPONENTS CMDEF CMUTIL STORAGE
        REQUIRED
)
IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE("cmake/Dependencies.cmake")
ENDIF ()

FIND_PACKAGE(CPPRESTSDK REQUIRED)
FIND_PACKAGE(Protobuf 3.21.12 REQUIRED)
FIND_PACKAGE(fleet-protocol-interface 2.0.0 REQUIRED)
FIND_PACKAGE(fleet-protocol-cxx-helpers-static 1.1.1 REQUIRED)

ADD_SUBDIRECTORY("lib/fleet-v2-http-client")

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

FILE(GLOB_RECURSE SOURCES "source/bringauto/*")

ADD_LIBRARY(mission_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET mission_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(mission_module_sources PUBLIC
        mission_module_hpp
        fleet-protocol-interface::common-headers-interface
        fleet-protocol-cxx-helpers-static::fleet-protocol-cxx-helpers-static
        protobuf_mission_lib
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP mission-module-gateway
        TYPE SHARED
        SOURCES "source/module_manager.cpp" "source/memory_management.cpp" "source/device_management.cpp"
        VERSION ${MISSION_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(mission-module-gateway-shared PRIVATE
        mission_module_sources
        fleet-protocol-interface::module-maintainer-module-gateway-interface
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP mission-external-server
        TYPE SHARED
        SOURCES "source/external_server_api.cpp" "source/memory_management.cpp" "source/device_management.cpp"
        VERSION ${MISSION_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(mission-external-server-shared PRIVATE
        mission_module_sources
        fleet-protocol-interface::module-maintainer-external-server-interface
        fleet-http-client-static
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP mission-module
        TYPE INTERFACE
        VERSION ${MISSION_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(mission-module-interface INTERFACE
        mission-module-gateway-shared
        mission-external-server-shared
)
IF (BRINGAUTO_INSTALL)
    CMDEF_INSTALL(
            TARGET mission-module-gateway-shared
            NAMESPACE mission-module-interface::
    )
    CMDEF_INSTALL(
            TARGET mission-external-server-shared
            NAMESPACE mission-module-interface::
    )
    CMDEF_INSTALL(
            TARGET mission-module-interface
            NAMESPACE mission-module-interface::
    )
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    CMDEF_PACKAGE(
            MAIN_TARGET mission-module-interface
            VERSION ${MISSION_MODULE_VERSION}
    )
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "BringAuto s.r.o. <maintainers@bringauto.com>")
    INCLUDE(CPack)
ENDIF ()