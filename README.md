# ButtonComboModule

[![CI-Release](https://github.com/wiiu-env/ButtonComboModule/actions/workflows/ci.yml/badge.svg)](https://github.com/wiiu-env/ButtonComboModule/actions/workflows/ci.yml)

**ButtonComboModule** is a Wii U Module System (WUMS) module that provides system-wide button combination detection. It
allows other homebrew applications and modules to easily register callbacks for specific button presses or holds across
various controllers (GamePad, Pro Controller, Wii Remote, etc.).

## Usage

(`[ENVIRONMENT]` is a placeholder for the actual environment name, e.g., `tiramisu` or `aroma`.)

1. Copy the file `ButtonComboModule.wms` into `sd:/wiiu/environments/[ENVIRONMENT]/modules`.
2. Requires the [WUMSLoader](https://github.com/wiiu-env/WUMSLoader) in
   `sd:/wiiu/environments/[ENVIRONMENT]/modules/setup`.
3. Requires the [FunctionPatcherModule](https://github.com/wiiu-env/FunctionPatcherModule) in
   `sd:/wiiu/environments/[ENVIRONMENT]/modules`.

## Development

### Homebrew Applications (.rpx / .wuhb)

If you are developing a standard homebrew application and want to use system-wide button combos, you should use the
**libbuttoncombo** client library.

* **Repository**: [wiiu-env/libbuttoncombo](https://github.com/wiiu-env/libbuttoncombo)

### WUPS Plugins (.wps)

If you are developing a plugin for the Wii U Plugin System (WUPS), you **should not** use `libbuttoncombo`.

The [WiiUPluginSystem](https://github.com/wiiu-env/WiiUPluginSystem) (WUPS) library already provides built-in wrappers
for the ButtonComboModule. You can use the WUPS API directly to register combos without linking an external library.

## Building

To build this module, you need **devkitPro** installed with `wut` and `wums`. You also need the `libfunctionpatcher`
libraries installed.

### Build Flags (Logging)

Building via `make` only logs critical errors via OSReport by default. To enable verbose logging via
the [LoggingModule](https://github.com/wiiu-env/LoggingModule), set `DEBUG` to `1` or `VERBOSE`.

* `make`: Logs errors only (via OSReport).
* `make DEBUG=1`: Enables information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).
* `make DEBUG=VERBOSE`: Enables verbose information and error logging
  via [LoggingModule](https://github.com/wiiu-env/LoggingModule).

If the [LoggingModule](https://github.com/wiiu-env/LoggingModule) is not present, it will fallback to UDP (Port 4405)
and [USBSerialLoggingModule](https://github.com/wiiu-env/USBSerialLoggingModule) logging.

### Building using Docker

It is possible to use a Docker image for building. This way, you don't need anything installed on your host system.

```
# Build Docker image (only needed once)
docker build . -t buttoncombomodule-builder

# make 
docker run -it --rm -v ${PWD}:/project buttoncombomodule-builder make

# make clean
docker run -it --rm -v ${PWD}:/project buttoncombomodule-builder make clean
```

## Formatting

You can format the code via Docker:

```
docker run --rm -v ${PWD}:/src ghcr.io/wiiu-env/clang-format:13.0.0-2 -r ./source -i
```

## License

This module is licensed under the **GPL-3.0**.