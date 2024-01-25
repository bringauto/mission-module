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

### Config

Some configuration is required when running this module. Example config:

```
"config": {
    "api_url": "http://localhost:8080",
    "api_key": "oNCNILeJgYkuzZRMhCKPKnXTlLqwqe",
    "company_name": "bringauto",
    "car_name": "vvu"
}
```

- api_url : URL of fleet http api ([project repository](https://gitlab.bringauto.com/bring-auto/fleet-protocol-v2/http-api/fleet-v2-http-api))
- api_key : generated in fleet http api (script/new_admin.py)
- company_name, car_name : used to identify car in fleet http api 

### Build

```
mkdir _build && cd _build
cmake -DCPPREST_ROOT=/usr -DCMAKE_CXX_FLAGS="-I/usr/local/opt/openssl/include" -DCMAKE_MODULE_LINKER_FLAGS="-L/usr/local/opt/openssl/lib" ..
make
```
