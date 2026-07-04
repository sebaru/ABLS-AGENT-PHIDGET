# abls-agent-phidget

Containerized Phidget runtime for Abls-Habitat.

## Current implementation status

- Runtime skeleton based on ABLS-THREAD-LIBS
- Facility fixed to `phidget`
- Prefix initialized from `thread_tech_id`
- Config bootstrap via Json_read_config using precedence:
  1. Environment variables (ABLS_*)
  2. /etc/abls-habitat-agent.conf
  3. Defaults in code

## Build

```sh
./install_deps.sh
./build.sh
```

## Container build

```sh
podman build -t abls-agent-phidget:dev \
  --build-arg ABLS_LIBS_DEVEL_RPM_URL=<url> \
  --build-arg ABLS_THREAD_LIBS_DEVEL_RPM_URL=<url> \
  --build-arg ABLS_LIBS_RPM_URL=<url> \
  --build-arg ABLS_THREAD_LIBS_RPM_URL=<url> \
  -f Containerfile .
```
