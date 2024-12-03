# Docker for CROSSCON Hypervisor

## Overview

The purpose of this repo is to provide the environment required for
compilation of the CROSSCON hypervisor.

## How to build the container

### Build the docker image

Run the `./build.sh` to build the image. The resulting image will have
the tag: `crosscon_hv`.

This will also produce the `build.log` log file, which is useful for
debugging.

### Create the container from the image

`docker create --name crosscon_hv_container crosscon_hv`

### Start the container

`docker run -d --name crosscon_hv_container crosscon_hv tail -f /dev/null`

`docker exec -it crosscon_hv_container /bin/bash`

### Remove the container

If the container already exists, and you need to create it from newer
image, use this command:

`docker stop crosscon_hv_container`

`docker remove crosscon_hv_container`

After this you should be able to create a new container.

## After you attach

1. Run `source env.sh` command. It will enable some of the tools that
   were installed earlier to be found globally. It will also let you use
   `gcc-9` as `gcc`.

2. CROSSCON Hypervisor repository is already installed and it's
   submodules were updated. It is available under the `crosscon`
   directory.
