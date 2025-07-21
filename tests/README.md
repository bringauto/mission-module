# Mission Module Tests

So far only manual tests are implemented.

## Manual tests

### Setup

Set up an [etna](https://github.com/bringauto/etna) repository, ideally with the latest master branch.

Steps to use local mission module changes in etna:
- Set up a [Module Gateway](https://github.com/bringauto/module-gateway) repository, ideally with the latest master branch.
- Push the local mission module changes to a new branch
- Change the mission module vertion in the module gateway Dockerfile to the new branch name
- Run the create_docker_compose_for_testing.py python script in etna to use a local build of module gateway

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
