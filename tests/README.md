# Mission Module Tests

So far only manual tests are implemented.

## Manual tests

### Setup

Set up an [etna](https://github.com/bringauto/etna) repository anywhere with the latest master branch.

```bash
cd tests
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt
```

### MQTT disconnect on stop arrival

```bash
python3 ./test_disconnect_on_stop_arrival.py --yaml <path-to-etna-docker-compose-file>
```

This test case runs the default etna scenario and disconnects the mqtt broker as soon as the car reaches the first stop. After roughly 2m 30s the car should reach the next stop automatically without mqtt connection. The test then reconnects the broker and checks for a status error message, which should contain the next stop in its finished stops list. If it doesn't exist, the test fails.

Path to the etna docker-compose file needs to be provided as an argument.

Command line arguments:
- `--yaml <path-to-etna-docker-compose-file>` : path to etna docker-compose file.
- `--logging` : enables logging of communication of mission module
- `--verbose` : enables logging of all mqtt communication
