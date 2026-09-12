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

## Packaging RPM

```sh
./build_rpm.sh
```

Produces runtime RPM package in `build/`.

The runtime package also installs a templated systemd unit:

- `abls-agent-phidget@.service`

Start one instance per agent tech id:

```sh
sudo systemctl enable --now abls-agent-phidget@<agent_tech_id>.service
```

## Packaging DEB

```sh
./build_apt.sh --dist bookworm
./build_apt.sh --dist trixie
```

Default target suite is detected from host OS codename (`/etc/os-release`), with `bookworm` fallback.

Useful options:

- `--version-suffix <s>`: override Debian version suffix (example `~trixie`)
- `--no-dist-suffix`: disable automatic `~<suite>` suffix

Produces runtime DEB package and copies normalized artifacts to:

- `build/deb/<suite>/<arch>/`

`build_apt.sh` builds only the native host architecture.

Package signatures are centralized in ABLS-PKGS (both DEB repository metadata and RPM package/repository signatures).

The DEB package installs the same templated systemd unit:

```sh
sudo systemctl enable --now abls-agent-phidget@<agent_tech_id>.service
```

## Release bump + publication

```sh
./bump.sh 1.2.3
```

The release flow:

- tags `v1.2.3` from `trunk`
- merges `trunk` into `main`
- builds RPM + DEB packages
- copies RPM to `../ABLS-PKGS/public/rpms/<arch>/`
- copies DEB to `../ABLS-PKGS/deb-packages/<suite>/<arch>/`

## Container build

```sh
podman build -t abls-agent-phidget:dev \
  --build-arg ABLS_LIBS_DEVEL_RPM_URL=<url> \
  --build-arg ABLS_AGENT_LIBS_DEVEL_RPM_URL=<url> \
  --build-arg ABLS_LIBS_RPM_URL=<url> \
  --build-arg ABLS_AGENT_LIBS_RPM_URL=<url> \
  -f Containerfile .
```
