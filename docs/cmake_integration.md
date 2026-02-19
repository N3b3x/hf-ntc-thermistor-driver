# CMake Integration — NTC Thermistor Driver

> **Canonical reference**: [`hf-hal-vortex-v1/docs/development/CMAKE_BUILD_CONTRACT.md`](../../../../../docs/development/CMAKE_BUILD_CONTRACT.md)

## Quick start (ESP-IDF)

The ESP-IDF component wrapper lives at:

```
examples/esp32/components/hf_ntc_thermistor/CMakeLists.txt
```

It imports `cmake/hf_ntc_thermistor_build_settings.cmake`, which is the single
source of truth for sources, includes, and version information.

## Layer overview

| Layer | File | Role |
|-------|------|------|
| 1 — Settings | `cmake/hf_ntc_thermistor_build_settings.cmake` | Sources, includes, version |
| 2 — Root | `CMakeLists.txt` | Desktop / CI static library |
| 3 — ESP-IDF | `examples/esp32/components/…/CMakeLists.txt` | IDF wrapper |

## Version

The version is defined once in `hf_ntc_thermistor_build_settings.cmake` and
stamped into `inc/ntc_thermistor_version.h.in` → `ntc_thermistor_version.h` via
`configure_file()`.

Access at runtime:

```cpp
NtcThermistor<MyAdc>::GetDriverVersion();      // "1.0.0"
NtcThermistor<MyAdc>::GetDriverVersionMajor(); // 1
GetNtcThermistorDriverVersion();               // "1.0.0" (free function)
```
