# Supported devices

The ONLY supported device is the Autonomy Device. Device ID currently has the following field values:
- module ID: 1,
- device type: 0,
- device role: `driving`.

The last field `device name` is not prescribed, as it is only informative and optional.


# Messages


## Status payload

Always contains
- state (State enum value)

If state is "DRIVE" or "IN_STOP", it must contain
- next stop (Station)

Optionally, the status contains
- telemetry (Telemetry)

Example
```json
{
    "state": "DRIVE",
    "telemetry": {
        "position": {
            "latitude": 45.8137528,
            "longitude": 15.9870608,
            "altitude": 120.7
        },
        "speed": 7.2,
        "fuel": 0.62
    }
    "nextStop": {
        "name": "Ribnjak",
        "position": {
            "latitude": 45.815011,
            "longitude": 15.981919,
            "altitude": 125.3
        }
    }
}
```

## Status Error payload

Always contains
- finished stops (list of Station)

Example:
```json
{
    "finishedStops": [
        {"name": "Trg Eugena Kvaternika", "position": {"latitude": 45.8144669, "longitude": 15.9965289, "altitude": 115.1}},
        {"name": "Vlaška", "position": {"latitude": 45.8139306, "longitude": 15.9913147, "altitude": 118.4}}
    ]
}
```

## Command payload

Always contains
- action (Action enum value)
- stop (Station)
- stations (list of Station)
- route (string)

Example of command payload when car received the mission:
```json
{
    "action": "START",
    "stations": [
        {"name": "Ribnjak", "position": {"latitude": 45.815011, "longitude": 15.981919, "altitude": 125.3}},
        {"name": "Trg hrvatskih velikana", "position": {"latitude": 45.8120758, "longitude": 15.9837108, "altitude": 120.7}},
        {"name": "Trg Eugena Kvaternika", "position": {"latitude": 45.8144669, "longitude": 15.9965289, "altitude": 115.1}}
    ],
    "route": "Kvatrić"
}
```

Example of command payload when there is no mission defined for the car:
```json
{
    "action": "NO_ACTION",
    "stations": [],
    "route": ""
}
```


# Device documentation

## Autonomy Device

