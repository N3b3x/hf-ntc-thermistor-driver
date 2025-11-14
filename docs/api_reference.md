---
layout: default
title: "📖 API Reference"
description: "Complete API documentation for the NTC Thermistor driver"
nav_order: 6
parent: "📚 Documentation"
permalink: /docs/api_reference/
---

# API Reference

Complete reference documentation for all public methods and types in the NTC Thermistor driver.

## Source Code

- **Main Header**: [`inc/ntc_thermistor.hpp`](../inc/ntc_thermistor.hpp)
- **Implementation**: [`src/ntc_thermistor.cpp`](../src/ntc_thermistor.cpp)
- **ADC Interface**: [`inc/ntc_adc_interface.hpp`](../inc/ntc_adc_interface.hpp)
- **Types**: [`inc/ntc_types.hpp`](../inc/ntc_types.hpp)

## Core Classes

### `NtcThermistor<AdcType>`

Main driver class for temperature measurement using NTC thermistors.

**Template Parameter**: `AdcType` - Your ADC interface implementation (must inherit from `ntc::AdcInterface<AdcType>`)

**Location**: [`inc/ntc_thermistor.hpp#L84`](../inc/ntc_thermistor.hpp#L84)

**Constructors:**
```cpp
NtcThermistor(NtcType ntc_type, AdcType* adc_interface);
NtcThermistor(const ntc_config_t& config, AdcType* adc_interface);
```

**Location**: [`inc/ntc_thermistor.hpp#L106`](../inc/ntc_thermistor.hpp#L106), [`inc/ntc_thermistor.hpp#L114`](../inc/ntc_thermistor.hpp#L114)

## Methods

### Initialization

#### `Initialize()`

Initialize the driver and prepare it for use.

**Signature:**
```cpp
bool Initialize() noexcept;
```

**Location**: [`src/ntc_thermistor.cpp#L68`](../src/ntc_thermistor.cpp#L68)

**Returns:**
- `true` if initialization succeeded
- `false` if initialization failed

**Example:**
```cpp
if (!thermistor.Initialize()) {
    // Handle initialization error
}
```

---

#### `Deinitialize()`

Clean up resources and deinitialize the driver.

**Signature:**
```cpp
bool Deinitialize() noexcept;
```

**Location**: [`src/ntc_thermistor.cpp#L105`](../src/ntc_thermistor.cpp#L105)

---

#### `IsInitialized()`

Check if the driver is initialized.

**Signature:**
```cpp
bool IsInitialized() const noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L161`](../inc/ntc_thermistor.hpp#L161)

---

### Temperature Reading

#### `ReadTemperatureCelsius(float*)`

Read temperature in Celsius.

**Signature:**
```cpp
NtcError ReadTemperatureCelsius(float* temperature_celsius) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L197`](../inc/ntc_thermistor.hpp#L197)

**Parameters:**
- `temperature_celsius`: Pointer to store temperature (°C)

**Returns:**
- `NtcError::Success` on success
- Error code on failure

**Example:**
```cpp
float temp_c;
if (thermistor.ReadTemperatureCelsius(&temp_c) == NtcError::Success) {
    printf("Temperature: %.2f°C\n", temp_c);
}
```

---

#### `ReadTemperatureFahrenheit(float*)`

Read temperature in Fahrenheit.

**Signature:**
```cpp
NtcError ReadTemperatureFahrenheit(float* temperature_fahrenheit) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L204`](../inc/ntc_thermistor.hpp#L204)

---

#### `ReadTemperatureKelvin(float*)`

Read temperature in Kelvin.

**Signature:**
```cpp
NtcError ReadTemperatureKelvin(float* temperature_kelvin) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L211`](../inc/ntc_thermistor.hpp#L211)

---

#### `ReadTemperature(ntc_reading_t*)`

Read complete temperature information including all units, resistance, and voltage.

**Signature:**
```cpp
NtcError ReadTemperature(ntc_reading_t* reading) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L218`](../inc/ntc_thermistor.hpp#L218)

**Parameters:**
- `reading`: Pointer to `ntc_reading_t` structure to store complete reading

**Returns:**
- `NtcError::Success` on success
- Error code on failure

---

### Configuration

#### `GetConfiguration(ntc_config_t*)`

Get current configuration.

**Signature:**
```cpp
NtcError GetConfiguration(ntc_config_t* config) const noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L168`](../inc/ntc_thermistor.hpp#L168)

---

#### `SetConfiguration(const ntc_config_t&)`

Set complete configuration.

**Signature:**
```cpp
NtcError SetConfiguration(const ntc_config_t& config) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L175`](../inc/ntc_thermistor.hpp#L175)

---

### Calibration

#### `Calibrate(float)`

Calibrate using a known reference temperature.

**Signature:**
```cpp
NtcError Calibrate(float reference_temperature_celsius) noexcept;
```

**Location**: [`inc/ntc_thermistor.hpp#L254`](../inc/ntc_thermistor.hpp#L254)

**Parameters:**
- `reference_temperature_celsius`: Known reference temperature (°C)

**Example:**
```cpp
// Calibrate at 25°C room temperature
thermistor.Calibrate(25.0f);
```

---

## Error Handling

The driver uses the `NtcError` enum for error reporting.

**Location**: [`inc/ntc_types.hpp#L37`](../inc/ntc_types.hpp#L37)

| Code | Name | Description |
|------|------|-------------|
| `Success` | Success | Operation succeeded |
| `NotInitialized` | Not initialized | Driver not initialized |
| `InvalidParameter` | Invalid parameter | Invalid method parameter |
| `AdcReadFailed` | ADC read failed | ADC read operation failed |
| `TemperatureOutOfRange` | Temperature out of range | Temperature outside valid range |
| `ConversionFailed` | Conversion failed | Temperature conversion failed |

## Type Definitions

### `NtcError` Enum

Error code enumeration.

**Location**: [`inc/ntc_types.hpp#L37`](../inc/ntc_types.hpp#L37)

### `NtcType` Enum

NTC thermistor type enumeration.

**Location**: [`inc/ntc_types.hpp#L71`](../inc/ntc_types.hpp#L71)

### `ntc_config_t` Structure

Configuration structure.

**Location**: [`inc/ntc_types.hpp#L120`](../inc/ntc_types.hpp#L120)

### `ntc_reading_t` Structure

Temperature reading structure.

**Location**: [`inc/ntc_types.hpp#L149`](../inc/ntc_types.hpp#L149)

## Thread Safety

The driver is **not thread-safe**. If used in a multi-threaded environment:
- Each `NtcThermistor` instance should be used by a single thread
- Use external synchronization (mutex, etc.) for shared access
- ADC interface access must be thread-safe in your ADC implementation

---

**Navigation**
⬅️ [Configuration](configuration.md) | [Next: Examples ➡️](examples.md) | [Back to Index](index.md)

