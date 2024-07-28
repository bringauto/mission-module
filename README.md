# Introduction

The Mission Module is a core module of the BringAuto in-house [Fleet Protocol](https://github.com/bringauto/fleet-protocol) for communication between a car and a cloud.

It provides communication between

1. Fleet Management - the application giving control over the car's mission to the end user,
2. Autonomy device - the driving component of the car.

For a detailed description of the inner workings, see the [Mission Module documentation](./docs/mission_module.md).

## Identification and supported devices

Module number/ID: `1`.

### Device list

| **Device Name** | **Device Type** | **Device Roles** | Comment                                                     |
|-----------------|-----------------|------------------|-------------------------------------------------------------|
| autonomy        | 0               | driving          | The device drives the car according the mission. |

## Dependencies

- [CMakeLib](https://github.com/cmakelib/cmakelib)

If BRINGAUTO_SYSTEM_DEP=ON is set the dependencies described by [cmake/Dependencies.cmake](cmake/Dependencies.cmake)
need to be installed as part of the system.

## Build

```bash
mkdir _build && cd _build
cmake -DCMAKE_BUILD_TYPE=Release [-DBRINGAUTO_INSTALL=ON] [-DBRINGAUTO_PACKAGE=ON] [-DBRINGAUTO_SYSTEM_DEP=ON] ..
make
make install # in case of INSTALL feature on
cpack # in case of PACKAGE feature on
```

## Configuration

External Server Module Configuration is required as:

```json
"config": {
    "api_url": "http://localhost:8080",
    "api_key": "StaticAccessKeyToBeUsedByDevelopersOnEtna",
    "company_name": "bringauto",
    "car_name": "virtual_vehicle",
    "max_requests_threshold_count": "10",
    "max_requests_threshold_period_ms": "5000",
    "delay_after_threshold_reached_ms": "5000",
    "retry_requests_delay_ms": "200"
}
```

- `api_url`: URL of the Fleet Protocol HTTP API (the [project repository](https://github.com/bringauto/fleet-protocol-http-api))
- `api_key`: generated in Fleet Protocol HTTP API (script/new_admin.py)
- `company_name`, `car_name`: used to identify the car in Fleet Protocol HTTP API
- `max_requests_threshold_count`, `max_requests_threshold_period_ms`, `delay_after_threshold_reached_ms`, `retry_requests_delay_ms`: explained in [HTTP client README](./lib/fleet-v2-http-client/README.md)
