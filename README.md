# Introduction

The Mission Module is a core module of the BringAuto in-house [Fleet Protocol](https://github.com/bringauto/fleet-protocol) for communication between a car and a cloud.

It provides communication between

1. Autonomy device - the driving component of the car.
2. Cloud - a component providing interface giving the end user to control the car's mission.

For a detailed description of the inner workings, see the [Mission Module documentation](./docs/mission_module.md).

# Identification and supported devices

Module number/ID: `1`.

## Autonomy Device

The device drives the car according to the current mission.

### Device identification

- device type: 0
- device role: `driving`

# Dependencies

- [CMakeLib](https://github.com/cmakelib/cmakelib)

# Build

```bash
mkdir _build && cd _build
cmake -DCMAKE_BUILD_TYPE=Release [-DBRINGAUTO_INSTALL=ON] [-DBRINGAUTO_PACKAGE=ON] ..
make
```

# Configuration

Configuration is required. Configuration file with example values:

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
