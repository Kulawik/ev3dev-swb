1. Install latest docker for your OS (see [install_docker.sh](scripts/install_docker.sh) script for Ubuntu 14.04 or refer to the [official docker guide](https://docs.docker.com/engine/installation/)). Make sure that you properly add yourself to the docker group.
2. Download docker image with compiler environment and tag it:

    ```sh
docker pull fuine/ev3cc:v2
docker tag fuine/ev3cc:v2 ev3cc
```
3. Use [docker_build.sh](scripts/docker_build.sh) to build your project. Refer to the [pidtest example](pidtest/) to see the example CMakeLists.txt and help information from the script.
