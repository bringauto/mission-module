# Mission module

Module for missions. This module connects Fleet Management and autonomy.

### Config

Some configuration is required when running this module. Example config:

```
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

- api_url : URL of fleet http api ([project repository](https://gitlab.bringauto.com/bring-auto/fleet-protocol-v2/http-api/fleet-v2-http-api))
- api_key : generated in fleet http api (script/new_admin.py)
- company_name, car_name : used to identify car in fleet http api
- max_requests_threshold_count, max_requests_threshold_period_ms, delay_after_threshold_reached_ms, retry_requests_delay_ms : explained in [http client README](./lib/fleet-v2-http-client/README.md)

### Build

```
mkdir _build && cd _build
cmake -DCMAKE_CXX_FLAGS="-I/usr/local/opt/openssl/include" -DCMAKE_MODULE_LINKER_FLAGS="-L/usr/local/opt/openssl/lib" ..
make
```
