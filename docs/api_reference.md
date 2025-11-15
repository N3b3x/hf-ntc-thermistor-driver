---
layout: default
title: "📖 API Reference"
description: "Complete API reference for the NTC Thermistor driver"
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

## Core Class

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

| Method | Signature | Location |
|--------|-----------|----------|
| `Initialize()` | `bool Initialize() noexcept` | [`src/ntc_thermistor.cpp#L68`](../src/ntc_thermistor.cpp#L68) |
| `Deinitialize()` | `bool Deinitialize() noexcept` | [`src/ntc_thermistor.cpp#L105`](../src/ntc_thermistor.cpp#L105) |
| `IsInitialized()` | `bool IsInitialized() const noexcept` | [`inc/ntc_thermistor.hpp#L161`](../inc/ntc_thermistor.hpp#L161) |
| `GetConfiguration()` | `NtcError GetConfiguration(ntc_config_t *config) const noexcept` | [`inc/ntc_thermistor.hpp#L168`](../inc/ntc_thermistor.hpp#L168) |
| `SetConfiguration()` | `NtcError SetConfiguration(const ntc_config_t &config) noexcept` | [`inc/ntc_thermistor.hpp#L175`](../inc/ntc_thermistor.hpp#L175) |

### Temperature Reading

| Method | Signature | Location |
|--------|-----------|----------|
| `ReadTemperatureCelsius()` | `NtcError ReadTemperatureCelsius(float *temperature_celsius) noexcept` | [`src/ntc_thermistor.cpp#L197`](../src/ntc_thermistor.cpp#L197) |
| `ReadTemperatureFahrenheit()` | `NtcError ReadTemperatureFahrenheit(float *temperature_fahrenheit) noexcept` | [`src/ntc_thermistor.cpp#L204`](../src/ntc_thermistor.cpp#L204) |
| `ReadTemperatureKelvin()` | `NtcError ReadTemperatureKelvin(float *temperature_kelvin) noexcept` | [`src/ntc_thermistor.cpp#L211`](../src/ntc_thermistor.cpp#L211) |
| `ReadTemperature()` | `NtcError ReadTemperature(ntc_reading_t *reading) noexcept` | [`src/ntc_thermistor.cpp#L218`](../src/ntc_thermistor.cpp#L218) |

### Resistance and Voltage

| Method | Signature | Location |
|--------|-----------|----------|
| `GetResistance()` | `NtcError GetResistance(float *resistance_ohms) noexcept` | [`src/ntc_thermistor.cpp#L229`](../src/ntc_thermistor.cpp#L229) |
| `GetVoltage()` | `NtcError GetVoltage(float *voltage_volts) noexcept` | [`src/ntc_thermistor.cpp#L236`](../src/ntc_thermistor.cpp#L236) |
| `GetRawAdcValue()` | `NtcError GetRawAdcValue(uint32_t *adc_value) noexcept` | [`src/ntc_thermistor.cpp#L243`](../src/ntc_thermistor.cpp#L243) |

### Calibration

| Method | Signature | Location |
|--------|-----------|----------|
| `Calibrate()` | `NtcError Calibrate(float reference_temperature_celsius) noexcept` | [`src/ntc_thermistor.cpp#L254`](../src/ntc_thermistor.cpp#L254) |
| `SetCalibrationOffset()` | `NtcError SetCalibrationOffset(float offset_celsius) noexcept` | [`src/ntc_thermistor.cpp#L261`](../src/ntc_thermistor.cpp#L261) |
| `GetCalibrationOffset()` | `NtcError GetCalibrationOffset(float *offset_celsius) const noexcept` | [`src/ntc_thermistor.cpp#L268`](../src/ntc_thermistor.cpp#L268) |
| `ResetCalibration()` | `NtcError ResetCalibration() noexcept` | [`src/ntc_thermistor.cpp#L274`](../src/ntc_thermistor.cpp#L274) |

### Configuration

| Method | Signature | Location |
|--------|-----------|----------|
| `SetConversionMethod()` | `NtcError SetConversionMethod(NtcConversionMethod method) noexcept` | [`src/ntc_thermistor.cpp#L285`](../src/ntc_thermistor.cpp#L285) |
| `SetVoltageDivider()` | `NtcError SetVoltageDivider(float series_resistance) noexcept` | [`src/ntc_thermistor.cpp#L292`](../src/ntc_thermistor.cpp#L292) |
| `SetReferenceVoltage()` | `NtcError SetReferenceVoltage(float reference_voltage) noexcept` | [`src/ntc_thermistor.cpp#L299`](../src/ntc_thermistor.cpp#L299) |
| `SetBetaValue()` | `NtcError SetBetaValue(float beta_value) noexcept` | [`src/ntc_thermistor.cpp#L306`](../src/ntc_thermistor.cpp#L306) |
| `SetAdcChannel()` | `NtcError SetAdcChannel(uint8_t adc_channel) noexcept` | [`src/ntc_thermistor.cpp#L313`](../src/ntc_thermistor.cpp#L313) |
| `SetSamplingParameters()` | `NtcError SetSamplingParameters(uint32_t sample_count, uint32_t sample_delay_ms) noexcept` | [`src/ntc_thermistor.cpp#L321`](../src/ntc_thermistor.cpp#L321) |
| `SetFiltering()` | `NtcError SetFiltering(bool enable, float alpha = 0.1F) noexcept` | [`src/ntc_thermistor.cpp#L330`](../src/ntc_thermistor.cpp#L330) |

### Utility Functions

| Method | Signature | Location |
|--------|-----------|----------|
| `CelsiusToFahrenheit()` | `static float CelsiusToFahrenheit(float celsius) noexcept` | [`inc/ntc_thermistor.hpp#L341`](../inc/ntc_thermistor.hpp#L341) |
| `FahrenheitToCelsius()` | `static float FahrenheitToCelsius(float fahrenheit) noexcept` | [`inc/ntc_thermistor.hpp#L348`](../inc/ntc_thermistor.hpp#L348) |
| `CelsiusToKelvin()` | `static float CelsiusToKelvin(float celsius) noexcept` | [`inc/ntc_thermistor.hpp#L355`](../inc/ntc_thermistor.hpp#L355) |
| `KelvinToCelsius()` | `static float KelvinToCelsius(float kelvin) noexcept` | [`inc/ntc_thermistor.hpp#L362`](../inc/ntc_thermistor.hpp#L362) |
| `GetErrorString()` | `static const char *GetErrorString(NtcError error) noexcept` | [`inc/ntc_thermistor.hpp#L369`](../inc/ntc_thermistor.hpp#L369) |
| `GetTypeString()` | `static const char *GetTypeString(NtcType type) noexcept` | [`inc/ntc_thermistor.hpp#L376`](../inc/ntc_thermistor.hpp#L376) |

## Types

### Enumerations

| Type | Values | Location |
|------|--------|----------|
| `NtcError` | `NTC_SUCCESS`, `NTC_ERROR_INIT`, `NTC_ERROR_ADC`, etc. | [`inc/ntc_types.hpp`](../inc/ntc_types.hpp) |
| `NtcType` | `NTC_TYPE_NTCG163JFT103FT1S`, etc. | [`inc/ntc_types.hpp`](../inc/ntc_types.hpp) |
| `NtcConversionMethod` | `NTC_METHOD_STEINHART_HART`, `NTC_METHOD_BETA`, `NTC_METHOD_LOOKUP_TABLE` | [`inc/ntc_types.hpp`](../inc/ntc_types.hpp) |

### Structures

| Type | Description | Location |
|------|-------------|----------|
| `ntc_config_t` | NTC configuration structure | [`inc/ntc_types.hpp`](../inc/ntc_types.hpp) |
| `ntc_reading_t` | Temperature reading structure | [`inc/ntc_types.hpp`](../inc/ntc_types.hpp) |

---

**Navigation**
⬅️ [Configuration](configuration.md) | [Next: Examples ➡️](examples.md) | [Back to Index](index.md)
