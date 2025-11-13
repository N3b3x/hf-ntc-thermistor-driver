# HF-NTC-Thermistor Documentation

Welcome! This directory contains documentation for the **HF-NTC-Thermistor** driver library.

## Overview

The HF-NTC-Thermistor driver provides a comprehensive solution for temperature measurement using NTC (Negative Temperature Coefficient) thermistors. It supports various NTC types and provides both lookup table and mathematical conversion methods for accurate temperature readings.

## Features

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

## Quick Start

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

## Documentation Structure

- **API Reference**: See the main README.md for API documentation
- **Examples**: See `examples/esp32/` for ESP32 usage examples
- **Doxygen**: Full API documentation available via Doxygen

## Additional Resources

- [Main README](../README.md) - Complete driver documentation
- [Examples](../examples/) - Usage examples
- [Contributing](../CONTRIBUTING.md) - Contribution guidelines

