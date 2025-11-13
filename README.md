---
layout: default
title: "HardFOC NTC Thermistor Driver"
description: "Hardware-agnostic NTC thermistor driver with lookup table and mathematical conversion methods"
nav_order: 1
permalink: /
---

# HF-NTC-Thermistor Driver
**Hardware-agnostic NTC thermistor driver for the HardFOC system**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## 📚 Table of Contents
1. [Overview](#-overview)
2. [Features](#-features)
3. [Quick Start](#-quick-start)
4. [Installation](#-installation)
5. [API Reference](#-api-reference)
6. [Examples](#-examples)
7. [Documentation](#-documentation)
8. [Contributing](#-contributing)
9. [License](#-license)

## 📦 Overview

> **📖 [📚🌐 Live Complete Documentation](https://n3b3x.github.io/hf-ntc-thermistor-driver/)** - 
> Interactive guides, examples, and step-by-step tutorials

This driver provides a comprehensive solution for temperature measurement using NTC thermistors. It supports various NTC types including the NTCG163JFT103FT1S and provides both lookup table and mathematical conversion methods for accurate temperature readings.

## ✨ Features

- **Hardware Agnostic**: Works with any ADC interface that implements NtcAdcInterface
- **Multiple NTC Types**: Support for various NTC thermistor specifications
- **Dual Conversion Methods**: Lookup table and mathematical (Steinhart-Hart) conversion
- **High Accuracy**: Optimized for precision temperature measurement
- **Calibration Support**: Built-in calibration and offset compensation
- **Temperature Units**: Support for Celsius, Fahrenheit, and Kelvin
- **Error Handling**: Comprehensive error reporting and validation
- **Thread Safe**: Designed for multi-threaded applications

## Supported NTC Types

| NTC Type | Resistance at 25°C | Beta Value | Temperature Range | Accuracy |
|----------|-------------------|------------|-------------------|----------|
| NTCG163JFT103FT1S | 10kΩ | 3435K | -40°C to +125°C | ±1% |
| NTCG164JF103FT1S | 10kΩ | 3435K | -40°C to +125°C | ±1% |
| NTCG163JF103FT1S | 10kΩ | 3435K | -40°C to +125°C | ±1% |
| Custom | Configurable | Configurable | Configurable | Configurable |

## 🚀 Quick Start

### Basic Example

```cpp
#include "NtcThermistor.h"

// 1. Define your ADC implementation (must inherit from NTC::NtcAdcInterface)
class MyAdc : public NTC::NtcAdcInterface<MyAdc> {
public:
    bool IsInitialized() const { return initialized_; }
    bool EnsureInitialized() { /* initialize ADC */ return true; }
    bool IsChannelAvailable(uint8_t channel) const { return channel < 8; }
    AdcError ReadChannelCount(uint8_t channel, uint32_t* count) { /* ... */ }
    AdcError ReadChannelV(uint8_t channel, float* voltage_v) { /* ... */ }
    float GetReferenceVoltage() const { return 3.3f; }
    uint8_t GetResolutionBits() const { return 12; }
private:
    bool initialized_ = false;
};

// 2. Create ADC and NTC instances
MyAdc adc;
adc.Initialize();

NtcThermistor<MyAdc> thermistor(NTC_TYPE_NTCG163JFT103FT1S, &adc);

// 3. Initialize and read temperature
if (thermistor.Initialize()) {
    float temperature;
    if (thermistor.ReadTemperatureCelsius(&temperature) == NTC_SUCCESS) {
        printf("Temperature: %.2f°C\n", temperature);
    }
}
```

### Custom Configuration Example

```cpp
// Create custom configuration
ntc_config_t config = NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S();
config.adc_channel = 1;                              // Use ADC channel 1
config.series_resistance = 10000.0f;                 // 10kΩ series resistor
config.conversion_method = NTC_CONVERSION_MATHEMATICAL; // Use math conversion
config.enable_filtering = true;                      // Enable filtering
config.filter_alpha = 0.1f;                          // Filter coefficient
config.sample_count = 5;                             // Average 5 samples
config.sample_delay_ms = 10;                         // 10ms between samples

NtcThermistor<MyAdc> thermistor(config, &adc);
thermistor.Initialize();

// Read complete temperature information
ntc_reading_t reading;
if (thermistor.ReadTemperature(&reading) == NTC_SUCCESS && reading.is_valid) {
    printf("Temperature: %.2f°C (%.2f°F, %.2fK)\n",
           reading.temperature_celsius,
           reading.temperature_fahrenheit,
           reading.temperature_kelvin);
    printf("Resistance: %.2fΩ, Voltage: %.3fV\n",
           reading.resistance_ohms,
           reading.voltage_volts);
}
```

## 📖 API Reference

### Core Functions

#### Initialization
- `Initialize()` - Initialize the thermistor driver (must be called before use)
- `Deinitialize()` - Clean up resources
- `IsInitialized()` - Check initialization status

#### Temperature Reading
- `ReadTemperatureCelsius(float* temp)` - Read temperature in Celsius
- `ReadTemperatureFahrenheit(float* temp)` - Read temperature in Fahrenheit
- `ReadTemperatureKelvin(float* temp)` - Read temperature in Kelvin
- `ReadTemperature(ntc_reading_t* reading)` - Read complete temperature information

#### Resistance and Voltage
- `GetResistance(float* resistance)` - Get thermistor resistance (ohms)
- `GetVoltage(float* voltage)` - Get voltage across thermistor (V)
- `GetRawAdcValue(uint32_t* value)` - Get raw ADC reading

#### Calibration
- `Calibrate(float reference_temp)` - Calibrate using known reference temperature
- `SetCalibrationOffset(float offset)` - Set calibration offset (°C)
- `GetCalibrationOffset(float* offset)` - Get current calibration offset
- `ResetCalibration()` - Reset calibration to zero

### Configuration Methods

- `SetConfiguration(const ntc_config_t& config)` - Set complete configuration
- `GetConfiguration(ntc_config_t* config)` - Get current configuration
- `SetConversionMethod(ntc_conversion_method_t method)` - Choose conversion method
- `SetVoltageDivider(float series_resistance)` - Configure voltage divider
- `SetReferenceVoltage(float voltage)` - Set ADC reference voltage
- `SetBetaValue(float beta)` - Set thermistor beta value
- `SetAdcChannel(uint8_t channel)` - Set ADC channel
- `SetSamplingParameters(uint32_t count, uint32_t delay_ms)` - Configure sampling
- `SetFiltering(bool enable, float alpha)` - Enable/disable filtering

### Utility Functions

- `CelsiusToFahrenheit(float celsius)` - Static conversion function
- `FahrenheitToCelsius(float fahrenheit)` - Static conversion function
- `CelsiusToKelvin(float celsius)` - Static conversion function
- `KelvinToCelsius(float kelvin)` - Static conversion function
- `GetErrorString(ntc_err_t error)` - Get human-readable error message
- `GetTypeString(ntc_type_t type)` - Get NTC type name string

## 🔧 Installation

1. Add to your project's `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/utils-and-drivers/hf-core-drivers/external/hf-ntc-thermistor-driver)
```

2. Include in your component's `CMakeLists.txt`:
```cmake
idf_component_register(
    REQUIRES hf-ntc-thermistor-driver
)
```

## 📊 Examples

### Architecture Overview

The NTC thermistor driver uses a **CRTP (Curiously Recurring Template Pattern)** design for hardware abstraction:

```
┌─────────────────────────────────────┐
│   NtcThermistor<AdcType>            │
│   (Main Driver Class)               │
└──────────────┬──────────────────────┘
               │ uses
               ▼
┌─────────────────────────────────────┐
│   NTC::NtcAdcInterface<AdcType>     │
│   (CRTP Base Interface)             │
└──────────────┬──────────────────────┘
               │ implements
               ▼
┌─────────────────────────────────────┐
│ YourAdc : NtcAdcInterface<YourAdc>  │
│ (Platform-Specific Implementation)  │
└─────────────────────────────────────┘
```

### Key Design Features

1. **Hardware Agnostic**: Works with any ADC by implementing the interface
2. **Zero Virtual Overhead**: CRTP provides compile-time polymorphism
3. **Type Safe**: Template-based design catches errors at compile time
4. **Modular**: Separate modules for conversion, lookup tables, and types

### Conversion Methods

The driver supports two conversion methods:

1. **Lookup Table** (`NTC_CONVERSION_LOOKUP_TABLE`)
   - Fast, pre-calculated values
   - Lower memory usage
   - Slightly less accurate
   - Best for real-time applications

2. **Mathematical** (`NTC_CONVERSION_MATHEMATICAL`)
   - Uses Steinhart-Hart or Beta equations
   - Higher accuracy
   - More computational overhead
   - Best for precision applications

3. **Auto** (`NTC_CONVERSION_AUTO`)
   - Automatically selects best method
   - Balances speed and accuracy

### Best Practices

1. **Always check return values**: Functions return `ntc_err_t` error codes
2. **Initialize before use**: Call `Initialize()` before reading temperatures
3. **Validate readings**: Check `is_valid` flag in `ntc_reading_t` structures
4. **Configure appropriately**: Set ADC channel, series resistance, and reference voltage correctly
5. **Use filtering for noisy signals**: Enable filtering with appropriate alpha value
6. **Calibrate when needed**: Use `Calibrate()` for improved accuracy

### ESP32 Examples

For complete ESP32 examples, see the [examples/esp32](examples/esp32/) directory.

## 📚 Documentation

For detailed documentation, see the [docs](docs/) directory.

## 🤝 Contributing

Pull requests and suggestions are welcome! Please see the main HardFOC coding standards for contribution guidelines.

## 📄 License

This project is licensed under the **GNU General Public License v3.0**.  
See the [LICENSE](LICENSE) file for details. 