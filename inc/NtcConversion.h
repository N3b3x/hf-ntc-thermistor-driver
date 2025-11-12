/**
 * @file NtcConversion.h
 * @brief Mathematical conversion functions for NTC thermistor temperature calculation.
 *
 * This header provides mathematical conversion methods for calculating temperature
 * from NTC thermistor resistance values using the Steinhart-Hart equation and
 * beta parameter approximation.
 *
 * @author HardFOC Development Team
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cmath>
#include <cstdint>

//--------------------------------------
//  Conversion Constants
//--------------------------------------

/**
 * @brief Physical constants for NTC calculations
 */
#define NTC_ABSOLUTE_ZERO_CELSIUS -273.15f ///< Absolute zero in Celsius
#define NTC_REFERENCE_TEMPERATURE_C 25.0f  ///< Reference temperature (25°C)
#define NTC_STEINHART_HART_A 1.129241e-3f  ///< Steinhart-Hart coefficient A
#define NTC_STEINHART_HART_B 2.341077e-4f  ///< Steinhart-Hart coefficient B
#define NTC_STEINHART_HART_C 8.775468e-8f  ///< Steinhart-Hart coefficient C

//--------------------------------------
//  Conversion Methods
//--------------------------------------

/**
 * @brief Convert resistance to temperature using beta parameter
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param resistance_at_25c Resistance at 25°C (ohms)
 * @param beta_value Beta value (K)
 * @param temperature_celsius Pointer to store temperature (°C)
 * @return true if successful, false otherwise
 */
bool ConvertResistanceToTemperatureBeta(float resistance_ohms, float resistance_at_25c,
                                        float beta_value, float* temperature_celsius) noexcept;

/**
 * @brief Convert temperature to resistance using beta parameter
 * @param temperature_celsius Temperature (°C)
 * @param resistance_at_25c Resistance at 25°C (ohms)
 * @param beta_value Beta value (K)
 * @param resistance_ohms Pointer to store resistance (ohms)
 * @return true if successful, false otherwise
 */
bool ConvertTemperatureToResistanceBeta(float temperature_celsius, float resistance_at_25c,
                                        float beta_value, float* resistance_ohms) noexcept;

/**
 * @brief Convert resistance to temperature using Steinhart-Hart equation
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param a Steinhart-Hart coefficient A
 * @param b Steinhart-Hart coefficient B
 * @param c Steinhart-Hart coefficient C
 * @param temperature_celsius Pointer to store temperature (°C)
 * @return true if successful, false otherwise
 */
bool ConvertResistanceToTemperatureSteinhartHart(float resistance_ohms, float a, float b, float c,
                                                 float* temperature_celsius) noexcept;

/**
 * @brief Convert temperature to resistance using Steinhart-Hart equation
 * @param temperature_celsius Temperature (°C)
 * @param a Steinhart-Hart coefficient A
 * @param b Steinhart-Hart coefficient B
 * @param c Steinhart-Hart coefficient C
 * @param resistance_ohms Pointer to store resistance (ohms)
 * @return true if successful, false otherwise
 */
bool ConvertTemperatureToResistanceSteinhartHart(float temperature_celsius, float a, float b,
                                                 float c, float* resistance_ohms) noexcept;

/**
 * @brief Convert resistance to temperature using default Steinhart-Hart coefficients
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param temperature_celsius Pointer to store temperature (°C)
 * @return true if successful, false otherwise
 */
bool ConvertResistanceToTemperatureDefault(float resistance_ohms,
                                           float* temperature_celsius) noexcept;

/**
 * @brief Convert temperature to resistance using default Steinhart-Hart coefficients
 * @param temperature_celsius Temperature (°C)
 * @param resistance_ohms Pointer to store resistance (ohms)
 * @return true if successful, false otherwise
 */
bool ConvertTemperatureToResistanceDefault(float temperature_celsius,
                                           float* resistance_ohms) noexcept;

//--------------------------------------
//  Voltage Divider Calculations
//--------------------------------------

/**
 * @brief Calculate thermistor resistance from voltage divider
 * @param voltage_thermistor Voltage across thermistor (V)
 * @param reference_voltage Reference voltage (V)
 * @param series_resistance Series resistance (ohms)
 * @param resistance_ohms Pointer to store thermistor resistance (ohms)
 * @return true if successful, false otherwise
 */
bool CalculateThermistorResistance(float voltage_thermistor, float reference_voltage,
                                   float series_resistance, float* resistance_ohms) noexcept;

/**
 * @brief Calculate voltage across thermistor from resistance
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param reference_voltage Reference voltage (V)
 * @param series_resistance Series resistance (ohms)
 * @param voltage_thermistor Pointer to store voltage across thermistor (V)
 * @return true if successful, false otherwise
 */
bool CalculateThermistorVoltage(float resistance_ohms, float reference_voltage,
                                float series_resistance, float* voltage_thermistor) noexcept;

/**
 * @brief Calculate voltage divider ratio
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param series_resistance Series resistance (ohms)
 * @param ratio Pointer to store voltage ratio
 * @return true if successful, false otherwise
 */
bool CalculateVoltageDividerRatio(float resistance_ohms, float series_resistance,
                                  float* ratio) noexcept;

//--------------------------------------
//  Validation Functions
//--------------------------------------

/**
 * @brief Validate resistance value
 * @param resistance_ohms Resistance value to validate
 * @param min_resistance Minimum valid resistance
 * @param max_resistance Maximum valid resistance
 * @return true if valid, false otherwise
 */
bool ValidateResistance(float resistance_ohms, float min_resistance, float max_resistance) noexcept;

/**
 * @brief Validate temperature value
 * @param temperature_celsius Temperature value to validate
 * @param min_temperature Minimum valid temperature
 * @param max_temperature Maximum valid temperature
 * @return true if valid, false otherwise
 */
bool ValidateTemperature(float temperature_celsius, float min_temperature,
                         float max_temperature) noexcept;

/**
 * @brief Validate voltage value
 * @param voltage_volts Voltage value to validate
 * @param min_voltage Minimum valid voltage
 * @param max_voltage Maximum valid voltage
 * @return true if valid, false otherwise
 */
bool ValidateVoltage(float voltage_volts, float min_voltage, float max_voltage) noexcept;

/**
 * @brief Validate beta value
 * @param beta_value Beta value to validate
 * @return true if valid, false otherwise
 */
bool ValidateBetaValue(float beta_value) noexcept;

/**
 * @brief Validate Steinhart-Hart coefficients
 * @param a Coefficient A
 * @param b Coefficient B
 * @param c Coefficient C
 * @return true if valid, false otherwise
 */
bool ValidateSteinhartHartCoefficients(float a, float b, float c) noexcept;

//--------------------------------------
//  Utility Functions
//--------------------------------------

/**
 * @brief Calculate beta value from two temperature-resistance pairs
 * @param t1 First temperature (°C)
 * @param r1 First resistance (ohms)
 * @param t2 Second temperature (°C)
 * @param r2 Second resistance (ohms)
 * @param beta_value Pointer to store calculated beta value
 * @return true if successful, false otherwise
 */
bool CalculateBetaValue(float t1, float r1, float t2, float r2, float* beta_value) noexcept;

/**
 * @brief Calculate Steinhart-Hart coefficients from three temperature-resistance pairs
 * @param t1 First temperature (°C)
 * @param r1 First resistance (ohms)
 * @param t2 Second temperature (°C)
 * @param r2 Second resistance (ohms)
 * @param t3 Third temperature (°C)
 * @param r3 Third resistance (ohms)
 * @param a Pointer to store coefficient A
 * @param b Pointer to store coefficient B
 * @param c Pointer to store coefficient C
 * @return true if successful, false otherwise
 */
bool CalculateSteinhartHartCoefficients(float t1, float r1, float t2, float r2, float t3, float r3,
                                        float* a, float* b, float* c) noexcept;

/**
 * @brief Calculate temperature accuracy estimate
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param resistance_tolerance Resistance tolerance (fraction)
 * @param beta_tolerance Beta tolerance (fraction)
 * @param accuracy_celsius Pointer to store accuracy estimate (°C)
 * @return true if successful, false otherwise
 */
bool CalculateTemperatureAccuracy(float resistance_ohms, float resistance_tolerance,
                                  float beta_tolerance, float* accuracy_celsius) noexcept;

/**
 * @brief Calculate optimal series resistance for voltage divider
 * @param thermistor_resistance_at_25c Thermistor resistance at 25°C (ohms)
 * @param min_temperature Minimum temperature (°C)
 * @param max_temperature Maximum temperature (°C)
 * @param optimal_resistance Pointer to store optimal series resistance (ohms)
 * @return true if successful, false otherwise
 */
bool CalculateOptimalSeriesResistance(float thermistor_resistance_at_25c, float min_temperature,
                                      float max_temperature, float* optimal_resistance) noexcept;