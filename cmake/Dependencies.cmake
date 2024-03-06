SET(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH OFF CACHE BOOL "Prevent using system paths for finding libraries")

BA_PACKAGE_LIBRARY(protobuf      v4.21.12)
BA_PACKAGE_LIBRARY(fleet-protocol-interface             v2.0.0 PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(fleet-protocol-cxx-helpers-static    v1.1.1)