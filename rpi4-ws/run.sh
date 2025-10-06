#!/bin/bash

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
ROOT_DIR="$(realpath "$SCRIPT_DIR/..")"

docker run -it --privileged \
  -u "$(id -u)":"$(id -g)" \
  --rm \
  -v "$ROOT_DIR":/work/crosscon \
  -v /dev:/dev \
  crosscon_hv \
  "$@"
