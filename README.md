# HF-NTC-Thermistor-Driver

Hardware-agnostic NTC (Negative Temperature Coefficient) thermistor driver for the HardFOC system.

## Overview

This driver provides a comprehensive solution for temperature measurement using NTC thermistors. It supports various NTC types including the NTCG163JFT103FT1S and provides both lookup table and mathematical conversion methods for accurate temperature readings.

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

## API Reference

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

## Installation

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

## License

MIT License - see LICENSE file for details. 