# Introduction to the Mission Module details

Here, additional info on the Mission Module is provided. For the purpose and usage see the [README](../README.md).

# Behavior of supported devices

## Autonomy Device

After connection, the autonomy device is in a state `Idle`. The internal state of Autonomy is driven by

- the received command (e.g., the command stops names and the action value),
- the sensors for car orientation (e.g., some obstacle prevents the car from driving).

![autonomy state diagram](images/image.png "Car state diagram")

The Autonomy keeps in memory the NAME of the next stop (the `nextStop` in the activity diagram below), it should drive to (it can be empty). This stop is used when calling for a change in the internal components of the Autonomy device driving the car. The actions taken by the autonomy after receiving the command are shown below. Note the `route` in the command is informative and does not affect the actions.

![activity diagram of autonomy receiving command](uml/exported_diagrams/command_activity_diagram.png "Activity diagram of the autonomy receiving command")

The Autonomy device sends the status of the car to the Mission Module. The status contains a field `State` with the value corresponding to the state of the device (`DRIVE`, `IN_STOP`, `IDLE`, `OBSTACLE`, `ERROR`).

# Messages

## Structure

Below, the data passed in the messages used in the External Protocol are described. The data structure can be also found in the [.proto](../lib/protobuf-mission-module/) file.

## Status payload data

Always contains

- state (State enum value: `IDLE`, `DRIVE`, `IN_STOP`, `OBSTACLE`, `ERROR`)

If the state is `DRIVE` or `IN_STOP`, it must contain

- next stop (`Station`)

Optionally, the status contains

- telemetry (`Telemetry`)

Example:

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
  },
  "nextStop": {
    "name": "Train Station",
    "position": {
      "latitude": 45.815011,
      "longitude": 15.981919,
      "altitude": 125.3
    }
  }
}
```

## Status Error payload data

Always contains

- finished stops (list of Station)

Example:

```json
{
  "finishedStops": [
    {
      "name": "Train Station",
      "position": {
        "latitude": 45.8144669,
        "longitude": 15.9965289,
        "altitude": 115.1
      }
    },
    {
      "name": "Bus Station",
      "position": {
        "latitude": 45.8139306,
        "longitude": 15.9913147,
        "altitude": 118.4
      }
    }
  ]
}
```

## Command payload data

Always contains

- action (Action enum value)
- stop (Station)
- stations (list of Station)
- route (string)

Example of command payload when the car received the mission:

```json
{
  "action": "START",
  "stations": [
    {
      "name": "Fish Market",
      "position": {
        "latitude": 45.815011,
        "longitude": 15.981919,
        "altitude": 125.3
      }
    },
    {
      "name": "Bus Station",
      "position": {
        "latitude": 45.8120758,
        "longitude": 15.9837108,
        "altitude": 120.7
      }
    },
    {
      "name": "Train Station",
      "position": {
        "latitude": 45.8144669,
        "longitude": 15.9965289,
        "altitude": 115.1
      }
    }
  ],
  "route": "FishMarketRoundTrip"
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

## Validation

The validation of each of the messages is done against the message structure described above (and in the [.proto](../lib/protobuf-mission-module/) file). The functions for validation:

- `parseAutonomyStatus` for status,
- `parseAutonomyError` for status error,
- `parseAutonomyCommand` for command.
