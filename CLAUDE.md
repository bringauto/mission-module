# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mission Module (Module ID: 1) is a Fleet Protocol module for autonomous vehicle communication. It produces two shared libraries loaded at runtime by Fleet Protocol components:

- **mission-module-gateway-shared** — implements the module-gateway interface (status aggregation, command generation)
- **mission-external-server-shared** — implements the external-server interface (forwards status to Fleet HTTP API, polls for commands)

## Build Commands

Requires CMake ≥ 3.25 and CMlib.

```bash
mkdir _build && cd _build

# Build both shared libs (default)
cmake ..
make

# Build only one of them
cmake -DFLEET_PROTOCOL_BUILD_MODULE_GATEWAY=OFF ..   # external server only
cmake -DFLEET_PROTOCOL_BUILD_EXTERNAL_SERVER=OFF ..  # module gateway only

# Package
cmake -DBRINGAUTO_PACKAGE=ON ..
make && cpack
```

CMake options: `BRINGAUTO_INSTALL`, `BRINGAUTO_PACKAGE` (forces both libs + install), `BRINGAUTO_SYSTEM_DEP`, `FLEET_PROTOCOL_BUILD_MODULE_GATEWAY` (default ON), `FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER` (default ON).

## Architecture

```
source/
  device_management.cpp    — get_module_number(), is_device_type_supported() (type 0 only)
  memory_management.cpp    — allocate()/deallocate() for fleet_protocol buffers
  module_manager.cpp       — module-gateway interface dispatcher → AutonomyDevice methods
  external_server_api.cpp  — external-server interface: init/destroy, forward_status, wait_for_command, pop_command

source/bringauto/
  AutonomyDevice.cpp       — core device logic (status condition, command generation, aggregation)
  JsonHelper.cpp           — all JSON ↔ buffer/struct conversions (centralized)
  JsonValidator.cpp        — validates AutonomyStatus, AutonomyCommand, AutonomyError JSON

include/bringauto/modules/mission_module/
  Context.hpp              — state held per external-server instance (FleetApiClient, command queue, mutex)
  AutonomyDevice.hpp       — static methods used by module_manager.cpp
  Constants.hpp            — speed threshold (1.0 m/s), status send period (2900 ms)
  AutonomyState.hpp        — IDLE, DRIVE, IN_STOP, OBSTACLE, ERROR, UNKNOWN
  AutonomyCommand.hpp      — NO_ACTION, STOP, START
```

## Key Behaviours

**Status sending condition** (`send_status_condition`): status is forwarded only when state or nextStop changed, or when speed > 1.0 m/s and ≥ 2.9 s have elapsed. Not every received status triggers a forward.

**Stop removal** (`generate_command`): when the car enters IN_STOP and the current nextStop matches the first stop in the command, that stop is removed from the command automatically.

**Offline stop tracking** (`aggregate_error`): stops reached while the server is disconnected are accumulated in the error message to avoid losing that information.

**External server init**: all config is validated on `init()` — returns `nullptr` on any failure. Config keys: `api_url` (URL regex), `api_key`, `company_name`, `car_name` (alphanumeric), plus four integer rate-limiting keys.

## Message Format

All payloads are JSON serialized into `buffer` structs. The `.proto` file in `lib/protobuf-mission-module/` reflects the schema but is not used at runtime — it exists for reference and Python testing tooling.

## Tests

Integration tests in `tests/` use Python + MQTT sniffer and require a running etna environment and a locally built Module Gateway. There are no unit tests.
