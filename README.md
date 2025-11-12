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

- **Hardware Agnostic**: Works with any ADC interface that implements BaseAdc
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

```cpp
#include "NtcThermistor.h"

// Create NTC thermistor instance
NtcThermistor thermistor(NTC_TYPE_NTCG163JFT103FT1S, adc_interface);

// Initialize
if (thermistor.Initialize()) {
    // Read temperature
    float temperature;
    if (thermistor.ReadTemperatureCelsius(&temperature) == NTC_SUCCESS) {
        printf("Temperature: %.2f°C\n", temperature);
    }
}
```

## 📖 API Reference

### Core Functions

- `Initialize()` - Initialize the thermistor driver
- `ReadTemperatureCelsius()` - Read temperature in Celsius
- `ReadTemperatureFahrenheit()` - Read temperature in Fahrenheit
- `ReadTemperatureKelvin()` - Read temperature in Kelvin
- `GetResistance()` - Get thermistor resistance
- `Calibrate()` - Calibrate the thermistor
- `SetCalibrationOffset()` - Set calibration offset

### Configuration

- `SetConversionMethod()` - Choose between lookup table and mathematical conversion
- `SetVoltageDivider()` - Configure voltage divider parameters
- `SetReferenceVoltage()` - Set ADC reference voltage
- `SetBetaValue()` - Set thermistor beta value

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

For ESP32 examples, see the [examples/esp32](examples/esp32/) directory.

## 📚 Documentation

For detailed documentation, see the [docs](docs/) directory.

## 🤝 Contributing

Pull requests and suggestions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

This project is licensed under the **GNU General Public License v3.0**.  
See the [LICENSE](LICENSE) file for details. 