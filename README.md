TODO describe steps of implementing module

Add submodule fleet-protocol
```
git submodule add git@github.com:bringauto/fleet-protocol.git lib/fleet-protocol

```
in add subdirectory in CMakeLists.txt and link libraries to target:
```
ADD_SUBDIRECTORY(lib/fleet-protocol)
TARGET_LINK_LIBRARIES(<target> PUBLIC fleet-protocol::common_headers
                                      module_gateway_maintainer
                                      )
```

Implement files `device_management.cpp`, `memory_management.cpp`, `module_manager.cpp` and put them to directory `source/`

### Recommended structure
In `bringauto/modules/<module_name>` create class `<module_name>` with all constants (Module number, device_type numbers).

In `bringauto/modules/<module_name>/devices` create classes for each device type. Each class should implement all `module_manager.h` functions, that can be used in `module_manager.cpp` for clearer code.

