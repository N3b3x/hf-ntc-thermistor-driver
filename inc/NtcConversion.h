/**
 * @file NtcConversion.h
 * @brief Mathematical conversion functions for NTC thermistor temperature calculation.
 *
 * This header provides mathematical conversion methods for calculating temperature
 * from NTC thermistor resistance values using the Steinhart-Hart equation and
 * beta parameter approximation.
 *
 * All conversion functions are in the @ref NTC namespace and can be used
 * independently of the NtcThermistor class for standalone calculations.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cmath>
#include <cstdint>

//--------------------------------------
//  Conversion Constants
//--------------------------------------

namespace NTC {
/**
 * @brief Physical constants for NTC calculations
 */
namespace Constants {
constexpr float kAbsoluteZeroCelsius = -273.15f; ///< Absolute zero in Celsius
constexpr float kKelvinOffset = 273.15f;         ///< Celsius to Kelvin conversion offset
constexpr float kReferenceTemperatureC = 25.0f;  ///< Reference temperature (25°C)
constexpr float kSteinhartHartA = 1.129241e-3f;  ///< Steinhart-Hart coefficient A
constexpr float kSteinhartHartB = 2.341077e-4f;  ///< Steinhart-Hart coefficient B
constexpr float kSteinhartHartC = 8.775468e-8f;  ///< Steinhart-Hart coefficient C

// Validation limits
constexpr float kMinResistanceOhms = 0.1f;         ///< Minimum valid resistance (ohms)
constexpr float kMaxResistanceOhms = 1000000.0f;   ///< Maximum valid resistance (ohms)
constexpr float kMinTemperatureCelsius = -273.15f; ///< Minimum valid temperature (°C)
constexpr float kMaxTemperatureCelsius = 1000.0f;  ///< Maximum valid temperature (°C)
constexpr float kMinBetaValue = 1000.0f;           ///< Minimum valid beta value (K)
constexpr float kMaxBetaValue = 5000.0f;           ///< Maximum valid beta value (K)

// Numerical precision constants
constexpr float kEpsilonFloat = 1e-6f;   ///< Float comparison epsilon
constexpr float kEpsilonDouble = 1e-12f; ///< Double precision epsilon
constexpr float kOneFloat = 1.0f;        ///< One as float
constexpr float kZeroFloat = 0.0f;       ///< Zero as float

// Time conversion constants
constexpr uint32_t kMillisecondsPerSecond = 1000U; ///< Milliseconds per second

// Temperature conversion constants
constexpr float kCelsiusToFahrenheitMultiplier = 9.0f / 5.0f; ///< Celsius to Fahrenheit multiplier
constexpr float kFahrenheitOffset = 32.0f;                    ///< Fahrenheit offset
constexpr float kFahrenheitToCelsiusMultiplier = 5.0f / 9.0f; ///< Fahrenheit to Celsius multiplier
} // namespace Constants
} // namespace NTC

// Legacy compatibility macros (deprecated, use NTC::Constants::*)
#define NTC_ABSOLUTE_ZERO_CELSIUS NTC::Constants::kAbsoluteZeroCelsius
#define NTC_REFERENCE_TEMPERATURE_C NTC::Constants::kReferenceTemperatureC
#define NTC_STEINHART_HART_A NTC::Constants::kSteinhartHartA
#define NTC_STEINHART_HART_B NTC::Constants::kSteinhartHartB
#define NTC_STEINHART_HART_C NTC::Constants::kSteinhartHartC

//--------------------------------------
//  Conversion Methods
//--------------------------------------

namespace NTC {

/**
 * @brief Convert resistance to temperature using beta parameter
 *
 * Uses the beta parameter equation to calculate temperature from resistance.
 * This is a simplified model that assumes a linear relationship in the
 * temperature-resistance curve around the reference temperature.
 *
 * @note The beta parameter method is faster but less accurate than
 *       Steinhart-Hart, especially at temperature extremes.
 *
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param resistance_at_25c Resistance at 25°C (ohms)
 * @param beta_value Beta value (K), typically 3000-4000K for NTC thermistors
 * @param temperature_celsius Pointer to store temperature (°C)
 * @return true if successful, false otherwise
 *
 * @warning Input resistance must be positive and non-zero. Beta value should
 *          be within valid range (1000-5000K).
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
 *
 * Uses the Steinhart-Hart equation for high-accuracy temperature calculation.
 * This method provides better accuracy across a wider temperature range
 * compared to the beta parameter method.
 *
 * The Steinhart-Hart equation: 1/T = A + B*ln(R) + C*ln(R)³
 *
 * @note This method is more computationally expensive but provides superior
 *       accuracy, especially at temperature extremes.
 *
 * @param resistance_ohms Thermistor resistance (ohms)
 * @param a Steinhart-Hart coefficient A (typically ~1.129e-3)
 * @param b Steinhart-Hart coefficient B (typically ~2.341e-4)
 * @param c Steinhart-Hart coefficient C (typically ~8.775e-8)
 * @param temperature_celsius Pointer to store temperature (°C)
 * @return true if successful, false otherwise
 *
 * @see ValidateSteinhartHartCoefficients() to validate coefficients
 */
bool ConvertResistanceToTemperatureSteinhartHart(float resistance_ohms, float coeff_a,
                                                 float coeff_b, float coeff_c,
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
bool ConvertTemperatureToResistanceSteinhartHart(float temperature_celsius, float coeff_a,
                                                 float coeff_b, float coeff_c,
                                                 float* resistance_ohms) noexcept;

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
bool ValidateSteinhartHartCoefficients(float coeff_a, float coeff_b, float coeff_c) noexcept;

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
                                        float* coeff_a, float* coeff_b, float* coeff_c) noexcept;

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

} // namespace NTC