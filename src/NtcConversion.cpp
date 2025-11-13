/**
 * @file NtcConversion.cpp
 * @brief Mathematical conversion functions for NTC thermistor temperature calculation.
 *
 * This file contains mathematical conversion methods for calculating temperature
 * from NTC thermistor resistance values using the Steinhart-Hart equation and
 * beta parameter approximation.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#include "NtcConversion.h"
#include <algorithm>
#include <cmath>

using NTC::Constants::kEpsilonDouble;
using NTC::Constants::kEpsilonFloat;
using NTC::Constants::kKelvinOffset;
using NTC::Constants::kMaxResistanceOhms;
using NTC::Constants::kMaxTemperatureCelsius;
using NTC::Constants::kMinResistanceOhms;
using NTC::Constants::kMinTemperatureCelsius;
using NTC::Constants::kOneFloat;
using NTC::Constants::kZeroFloat;

//--------------------------------------
//  Conversion Methods
//--------------------------------------

namespace NTC {

bool ConvertResistanceToTemperatureBeta(float resistance_ohms, float resistance_at_25c,
                                        float beta_value, float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  if (!ValidateBetaValue(beta_value)) {
    return false;
  }

  if (resistance_at_25c <= kZeroFloat) {
    return false;
  }

  // Beta equation: 1/T = 1/T0 + (1/β) * ln(R/R0)
  // Where: T = temperature in Kelvin, T0 = reference temperature (298.15K = 25°C)
  // R = resistance, R0 = resistance at reference temperature, β = beta value

  constexpr float kReferenceTemperatureKelvin =
      NTC::Constants::kReferenceTemperatureC + kKelvinOffset; // 298.15K
  const float temp_reference_kelvin = kReferenceTemperatureKelvin;
  const float ln_ratio = std::log(resistance_ohms / resistance_at_25c);
  const float inv_temperature = (kOneFloat / temp_reference_kelvin) + (ln_ratio / beta_value);

  if (inv_temperature <= kZeroFloat) {
    return false; // Invalid result
  }

  const float temp_kelvin = kOneFloat / inv_temperature;
  *temperature_celsius = temp_kelvin - kKelvinOffset;

  return true;
}

bool ConvertTemperatureToResistanceBeta(float temperature_celsius, float resistance_at_25c,
                                        float beta_value, float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temperature_celsius, kMinTemperatureCelsius, kMaxTemperatureCelsius)) {
    return false;
  }

  if (!ValidateBetaValue(beta_value)) {
    return false;
  }

  if (resistance_at_25c <= kZeroFloat) {
    return false;
  }

  // Beta equation: R = R0 * exp(β * (1/T - 1/T0))
  // Where: T = temperature in Kelvin, T0 = reference temperature (298.15K = 25°C)
  // R = resistance, R0 = resistance at reference temperature, β = beta value

  constexpr float kReferenceTemperatureKelvin =
      NTC::Constants::kReferenceTemperatureC + kKelvinOffset; // 298.15K
  const float temp_kelvin = temperature_celsius + kKelvinOffset;
  const float temp_reference_kelvin = kReferenceTemperatureKelvin;

  if (temp_kelvin <= kZeroFloat) {
    return false; // Invalid temperature
  }

  const float inv_diff = (kOneFloat / temp_kelvin) - (kOneFloat / temp_reference_kelvin);
  const float exp_term = std::exp(beta_value * inv_diff);
  *resistance_ohms = resistance_at_25c * exp_term;

  return true;
}

bool ConvertResistanceToTemperatureSteinhartHart(float resistance_ohms, float coeff_a,
                                                 float coeff_b, float coeff_c,
                                                 float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  if (!ValidateSteinhartHartCoefficients(coeff_a, coeff_b, coeff_c)) {
    return false;
  }

  // Steinhart-Hart equation: 1/T = A + B*ln(R) + C*ln(R)^3
  // Where: T = temperature in Kelvin, R = resistance

  const float ln_R = std::log(resistance_ohms);
  const float ln_R_cubed = ln_R * ln_R * ln_R;
  const float inv_temperature = coeff_a + (coeff_b * ln_R) + (coeff_c * ln_R_cubed);

  if (inv_temperature <= kZeroFloat) {
    return false; // Invalid result
  }

  const float temp_kelvin = kOneFloat / inv_temperature;
  *temperature_celsius = temp_kelvin - kKelvinOffset;

  return true;
}

bool ConvertTemperatureToResistanceSteinhartHart(float temperature_celsius, float coeff_a,
                                                 float coeff_b, float coeff_c,
                                                 float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temperature_celsius, kMinTemperatureCelsius, kMaxTemperatureCelsius)) {
    return false;
  }

  if (!ValidateSteinhartHartCoefficients(coeff_a, coeff_b, coeff_c)) {
    return false;
  }

  // Steinhart-Hart equation: ln(R) = (1/T - A - C*ln(R)^3) / B
  // This requires iterative solution, so we use a simplified approach

  const float temp_kelvin = temperature_celsius + kKelvinOffset;
  if (temp_kelvin <= kZeroFloat) {
    return false; // Invalid temperature
  }

  const float inv_temperature = kOneFloat / temp_kelvin;

  // For most NTC thermistors, the C term is small, so we can approximate
  // ln(R) ≈ (1/T - A) / B
  const float ln_R_approx = (inv_temperature - coeff_a) / coeff_b;

  constexpr float kMaxLnResistanceApprox = 20.0F;
  if (ln_R_approx <= -kMaxLnResistanceApprox || ln_R_approx >= kMaxLnResistanceApprox) {
    return false; // Result out of reasonable range
  }

  *resistance_ohms = std::exp(ln_R_approx);

  return true;
}

bool ConvertResistanceToTemperatureDefault(float resistance_ohms,
                                           float* temperature_celsius) noexcept {
  return ConvertResistanceToTemperatureSteinhartHart(resistance_ohms, NTC_STEINHART_HART_A,
                                                     NTC_STEINHART_HART_B, NTC_STEINHART_HART_C,
                                                     temperature_celsius);
}

bool ConvertTemperatureToResistanceDefault(float temperature_celsius,
                                           float* resistance_ohms) noexcept {
  return ConvertTemperatureToResistanceSteinhartHart(temperature_celsius, NTC_STEINHART_HART_A,
                                                     NTC_STEINHART_HART_B, NTC_STEINHART_HART_C,
                                                     resistance_ohms);
}

//--------------------------------------
//  Voltage Divider Calculations
//--------------------------------------

bool CalculateThermistorResistance(float voltage_thermistor, float reference_voltage,
                                   float series_resistance, float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateVoltage(voltage_thermistor, kZeroFloat, reference_voltage)) {
    return false;
  }

  if (reference_voltage <= kZeroFloat || series_resistance <= kZeroFloat) {
    return false;
  }

  // Voltage divider equation: V_thermistor = V_ref * (R_thermistor / (R_series + R_thermistor))
  // Solving for R_thermistor: R_thermistor = R_series * (V_thermistor / (V_ref - V_thermistor))

  const float voltage_diff = reference_voltage - voltage_thermistor;
  if (std::abs(voltage_diff) < kEpsilonFloat) {
    return false; // Voltage too close to reference voltage
  }

  const float ratio = voltage_thermistor / voltage_diff;
  *resistance_ohms = series_resistance * ratio;

  return true;
}

bool CalculateThermistorVoltage(float resistance_ohms, float reference_voltage,
                                float series_resistance, float* voltage_thermistor) noexcept {
  if (voltage_thermistor == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  if (reference_voltage <= kZeroFloat || series_resistance <= kZeroFloat) {
    return false;
  }

  // Voltage divider equation: V_thermistor = V_ref * (R_thermistor / (R_series + R_thermistor))

  const float total_resistance = series_resistance + resistance_ohms;
  if (total_resistance <= kZeroFloat) {
    return false;
  }

  *voltage_thermistor = reference_voltage * (resistance_ohms / total_resistance);

  return true;
}

bool CalculateVoltageDividerRatio(float resistance_ohms, float series_resistance,
                                  float* ratio) noexcept {
  if (ratio == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  if (series_resistance <= kZeroFloat) {
    return false;
  }

  const float total_resistance = series_resistance + resistance_ohms;
  if (total_resistance <= kZeroFloat) {
    return false;
  }

  *ratio = resistance_ohms / total_resistance;

  return true;
}

//--------------------------------------
//  Validation Functions
//--------------------------------------

bool ValidateResistance(float resistance_ohms, float min_resistance,
                        float max_resistance) noexcept {
  return (resistance_ohms >= min_resistance && resistance_ohms <= max_resistance);
}

bool ValidateTemperature(float temperature_celsius, float min_temperature,
                         float max_temperature) noexcept {
  return (temperature_celsius >= min_temperature && temperature_celsius <= max_temperature);
}

bool ValidateVoltage(float voltage_volts, float min_voltage, float max_voltage) noexcept {
  return (voltage_volts >= min_voltage && voltage_volts <= max_voltage);
}

bool ValidateBetaValue(float beta_value) noexcept {
  // Beta values for NTC thermistors typically range from 2000K to 5000K
  return (beta_value >= NTC::Constants::kMinBetaValue &&
          beta_value <= NTC::Constants::kMaxBetaValue);
}

bool ValidateSteinhartHartCoefficients(float coeff_a, float coeff_b, float coeff_c) noexcept {
  // Validate reasonable ranges for Steinhart-Hart coefficients
  // These are typical ranges, but may vary for specific thermistors
  constexpr float kMinCoeffA = -1e-2F;
  constexpr float kMaxCoeffA = 1e-2F;
  constexpr float kMinCoeffB = 1e-4F;
  constexpr float kMaxCoeffB = 1e-3F;
  constexpr float kMinCoeffC = -1e-7F;
  constexpr float kMaxCoeffC = 1e-7F;
  return (coeff_a >= kMinCoeffA && coeff_a <= kMaxCoeffA && coeff_b >= kMinCoeffB &&
          coeff_b <= kMaxCoeffB && coeff_c >= kMinCoeffC && coeff_c <= kMaxCoeffC);
}

//--------------------------------------
//  Utility Functions
//--------------------------------------

bool CalculateBetaValue(float t1, float r1, float t2, float r2, float* beta_value) noexcept {
  if (beta_value == nullptr) {
    return false;
  }

  if (!ValidateTemperature(t1, kMinTemperatureCelsius, kMaxTemperatureCelsius) ||
      !ValidateTemperature(t2, kMinTemperatureCelsius, kMaxTemperatureCelsius)) {
    return false;
  }

  if (!ValidateResistance(r1, kMinResistanceOhms, kMaxResistanceOhms) ||
      !ValidateResistance(r2, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  if (std::abs(t1 - t2) < kEpsilonFloat) {
    return false; // Temperatures too close
  }

  if (r1 <= kZeroFloat || r2 <= kZeroFloat) {
    return false;
  }

  // Beta equation: β = ln(R1/R2) / (1/T1 - 1/T2)
  // Where: T1, T2 = temperatures in Kelvin, R1, R2 = resistances

  const float T1 = t1 + kKelvinOffset;
  const float T2 = t2 + kKelvinOffset;

  const float ln_ratio = std::log(r1 / r2);
  const float inv_diff = (kOneFloat / T1) - (kOneFloat / T2);

  if (std::abs(inv_diff) < kEpsilonFloat) {
    return false; // Invalid temperature difference
  }

  *beta_value = ln_ratio / inv_diff;

  return ValidateBetaValue(*beta_value);
}

bool CalculateSteinhartHartCoefficients(float t1, float r1, float t2, float r2, float t3, float r3,
                                        float* coeff_a, float* coeff_b, float* coeff_c) noexcept {
  if (coeff_a == nullptr || coeff_b == nullptr || coeff_c == nullptr) {
    return false;
  }

  if (!ValidateTemperature(t1, kMinTemperatureCelsius, kMaxTemperatureCelsius) ||
      !ValidateTemperature(t2, kMinTemperatureCelsius, kMaxTemperatureCelsius) ||
      !ValidateTemperature(t3, kMinTemperatureCelsius, kMaxTemperatureCelsius)) {
    return false;
  }

  if (!ValidateResistance(r1, kMinResistanceOhms, kMaxResistanceOhms) ||
      !ValidateResistance(r2, kMinResistanceOhms, kMaxResistanceOhms) ||
      !ValidateResistance(r3, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  // Check that temperatures are different
  if (std::abs(t1 - t2) < kEpsilonFloat || std::abs(t2 - t3) < kEpsilonFloat ||
      std::abs(t1 - t3) < kEpsilonFloat) {
    return false;
  }

  // Convert temperatures to Kelvin
  const float T1 = t1 + kKelvinOffset;
  const float T2 = t2 + kKelvinOffset;
  const float T3 = t3 + kKelvinOffset;

  // Calculate natural logarithms of resistances
  const float ln_R1 = std::log(r1);
  const float ln_R2 = std::log(r2);
  const float ln_R3 = std::log(r3);

  // Calculate inverse temperatures
  const float inv_T1 = kOneFloat / T1;
  const float inv_T2 = kOneFloat / T2;
  const float inv_T3 = kOneFloat / T3;

  // Solve the system of equations using Cramer's rule
  // Matrix: [1 ln_R1 ln_R1^3] [A]   [1/T1]
  //         [1 ln_R2 ln_R2^3] [B] = [1/T2]
  //         [1 ln_R3 ln_R3^3] [C]   [1/T3]

  const float det = kOneFloat * (ln_R2 * ln_R3 * ln_R3 * ln_R3 - ln_R3 * ln_R2 * ln_R2 * ln_R2) +
                    ln_R1 * (ln_R3 * ln_R2 * ln_R2 * ln_R2 - ln_R2 * ln_R3 * ln_R3 * ln_R3) +
                    ln_R1 * ln_R1 * ln_R1 * (ln_R2 - ln_R3);

  if (std::abs(det) < kEpsilonDouble) {
    return false; // Singular matrix
  }

  const float det_a = inv_T1 * (ln_R2 * ln_R3 * ln_R3 * ln_R3 - ln_R3 * ln_R2 * ln_R2 * ln_R2) +
                      inv_T2 * (ln_R3 * ln_R1 * ln_R1 * ln_R1 - ln_R1 * ln_R3 * ln_R3 * ln_R3) +
                      inv_T3 * (ln_R1 * ln_R2 * ln_R2 * ln_R2 - ln_R2 * ln_R1 * ln_R1 * ln_R1);

  const float det_b =
      kOneFloat * (inv_T2 * ln_R3 * ln_R3 * ln_R3 - inv_T3 * ln_R2 * ln_R2 * ln_R2) +
      ln_R1 * (inv_T3 * ln_R1 * ln_R1 * ln_R1 - inv_T1 * ln_R3 * ln_R3 * ln_R3) +
      ln_R1 * ln_R1 * ln_R1 * (inv_T1 - inv_T2);

  const float det_c = kOneFloat * (ln_R2 * inv_T3 - ln_R3 * inv_T2) +
                      ln_R1 * (ln_R3 * inv_T1 - ln_R1 * inv_T3) +
                      ln_R1 * ln_R1 * ln_R1 * (inv_T2 - inv_T1);

  *coeff_a = det_a / det;
  *coeff_b = det_b / det;
  *coeff_c = det_c / det;

  return ValidateSteinhartHartCoefficients(*coeff_a, *coeff_b, *coeff_c);
}

bool CalculateTemperatureAccuracy(float resistance_ohms, float resistance_tolerance,
                                  float beta_tolerance, float* accuracy_celsius) noexcept {
  if (accuracy_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, kMinResistanceOhms, kMaxResistanceOhms)) {
    return false;
  }

  constexpr float kMinTolerance = 0.0f;
  constexpr float kMaxTolerance = 1.0f;
  if (resistance_tolerance < kMinTolerance || resistance_tolerance > kMaxTolerance) {
    return false;
  }

  if (beta_tolerance < kMinTolerance || beta_tolerance > kMaxTolerance) {
    return false;
  }

  // Estimate temperature accuracy based on resistance and beta tolerances
  // This is a simplified calculation - actual accuracy depends on many factors
  constexpr float kTypicalBetaValue = 3435.0f;      // Typical beta value for NTC thermistors
  constexpr float kResistanceErrorDivisor = 100.0f; // Rough approximation divisor
  constexpr float kBetaErrorDivisor = 1000.0f;      // Rough approximation divisor

  const float resistance_error = resistance_ohms * resistance_tolerance;
  const float beta_error = kTypicalBetaValue * beta_tolerance;

  // Convert resistance error to temperature error (approximate)
  const float temp_error_resistance = resistance_error / kResistanceErrorDivisor;

  // Convert beta error to temperature error (approximate)
  const float temp_error_beta = beta_error / kBetaErrorDivisor;

  *accuracy_celsius =
      std::sqrt(temp_error_resistance * temp_error_resistance + temp_error_beta * temp_error_beta);

  return true;
}

bool CalculateOptimalSeriesResistance(float thermistor_resistance_at_25c, float min_temperature,
                                      float max_temperature, float* optimal_resistance) noexcept {
  if (optimal_resistance == nullptr) {
    return false;
  }

  constexpr float kMinThermistorResistance = 100.0f;
  if (!ValidateResistance(thermistor_resistance_at_25c, kMinThermistorResistance,
                          kMaxResistanceOhms)) {
    return false;
  }

  if (!ValidateTemperature(min_temperature, kMinTemperatureCelsius, kMaxTemperatureCelsius) ||
      !ValidateTemperature(max_temperature, kMinTemperatureCelsius, kMaxTemperatureCelsius)) {
    return false;
  }

  if (min_temperature >= max_temperature) {
    return false;
  }

  // Calculate resistances at min and max temperatures
  float r_min = 0.0F;
  float r_max = 0.0F;
  if (!ConvertTemperatureToResistanceBeta(min_temperature, thermistor_resistance_at_25c, 3435.0F,
                                          &r_min) ||
      !ConvertTemperatureToResistanceBeta(max_temperature, thermistor_resistance_at_25c, 3435.0F,
                                          &r_max)) {
    return false;
  }

  // Optimal series resistance is the geometric mean of min and max resistances
  // This provides the most linear voltage response over the temperature range
  *optimal_resistance = std::sqrt(r_min * r_max);

  return true;
}

} // namespace NTC