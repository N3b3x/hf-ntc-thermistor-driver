/**
 * @file NtcConversion.cpp
 * @brief Mathematical conversion functions for NTC thermistor temperature
 * calculation.
 *
 * This file contains mathematical conversion methods for calculating
 * temperature from NTC thermistor resistance values using the Steinhart-Hart
 * equation and beta parameter approximation.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#include "NtcConversion.h"
#include <algorithm>
#include <cmath>

using NTC::Constants::EPSILON_DOUBLE_;
using NTC::Constants::EPSILON_FLOAT_;
using NTC::Constants::KELVIN_OFFSET_;
using NTC::Constants::MAX_RESISTANCE_OHMS_;
using NTC::Constants::MAX_TEMPERATURE_CELSIUS_;
using NTC::Constants::MIN_RESISTANCE_OHMS_;
using NTC::Constants::MIN_TEMPERATURE_CELSIUS_;
using NTC::Constants::ONE_FLOAT_;
using NTC::Constants::ZERO_FLOAT_;

//--------------------------------------
//  Conversion Methods
//--------------------------------------

namespace NTC {

bool ConvertResistanceToTemperatureBeta(float resistance_ohms,
                                        float resistance_at_25c,
                                        float beta_value,
                                        float *temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (!ValidateBetaValue(beta_value)) {
    return false;
  }

  if (resistance_at_25c <= ZERO_FLOAT_) {
    return false;
  }

  // Beta equation: 1/T = 1/T0 + (1/β) * ln(R/R0)
  // Where: T = temperature in Kelvin, T0 = reference temperature (298.15K =
  // 25°C) R = resistance, R0 = resistance at reference temperature, β = beta
  // value

  constexpr float kReferenceTemperatureKelvin =
      NTC::Constants::REFERENCE_TEMPERATURE_C_ + KELVIN_OFFSET_; // 298.15K
  const float temp_reference_kelvin = kReferenceTemperatureKelvin;
  const float ln_ratio = std::log(resistance_ohms / resistance_at_25c);
  const float inv_temperature =
      (ONE_FLOAT_ / temp_reference_kelvin) + (ln_ratio / beta_value);

  if (inv_temperature <= ZERO_FLOAT_) {
    return false; // Invalid result
  }

  const float temp_kelvin = ONE_FLOAT_ / inv_temperature;
  *temperature_celsius = temp_kelvin - KELVIN_OFFSET_;

  return true;
}

bool ConvertTemperatureToResistanceBeta(float temperature_celsius,
                                        float resistance_at_25c,
                                        float beta_value,
                                        float *resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temperature_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_)) {
    return false;
  }

  if (!ValidateBetaValue(beta_value)) {
    return false;
  }

  if (resistance_at_25c <= ZERO_FLOAT_) {
    return false;
  }

  // Beta equation: R = R0 * exp(β * (1/T - 1/T0))
  // Where: T = temperature in Kelvin, T0 = reference temperature (298.15K =
  // 25°C) R = resistance, R0 = resistance at reference temperature, β = beta
  // value

  constexpr float kReferenceTemperatureKelvin =
      NTC::Constants::REFERENCE_TEMPERATURE_C_ + KELVIN_OFFSET_; // 298.15K
  const float temp_kelvin = temperature_celsius + KELVIN_OFFSET_;
  const float temp_reference_kelvin = kReferenceTemperatureKelvin;

  if (temp_kelvin <= ZERO_FLOAT_) {
    return false; // Invalid temperature
  }

  const float inv_diff =
      (ONE_FLOAT_ / temp_kelvin) - (ONE_FLOAT_ / temp_reference_kelvin);
  const float exp_term = std::exp(beta_value * inv_diff);
  *resistance_ohms = resistance_at_25c * exp_term;

  return true;
}

bool ConvertResistanceToTemperatureSteinhartHart(
    float resistance_ohms, float coeff_a, float coeff_b, float coeff_c,
    float *temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (!ValidateSteinhartHartCoefficients(coeff_a, coeff_b, coeff_c)) {
    return false;
  }

  // Steinhart-Hart equation: 1/T = A + B*ln(R) + C*ln(R)^3
  // Where: T = temperature in Kelvin, R = resistance

  const float ln_R = std::log(resistance_ohms);
  const float ln_R_cubed = ln_R * ln_R * ln_R;
  const float inv_temperature =
      coeff_a + (coeff_b * ln_R) + (coeff_c * ln_R_cubed);

  if (inv_temperature <= ZERO_FLOAT_) {
    return false; // Invalid result
  }

  const float temp_kelvin = ONE_FLOAT_ / inv_temperature;
  *temperature_celsius = temp_kelvin - KELVIN_OFFSET_;

  return true;
}

bool ConvertTemperatureToResistanceSteinhartHart(
    float temperature_celsius, float coeff_a, float coeff_b, float coeff_c,
    float *resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temperature_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_)) {
    return false;
  }

  if (!ValidateSteinhartHartCoefficients(coeff_a, coeff_b, coeff_c)) {
    return false;
  }

  // Steinhart-Hart equation: ln(R) = (1/T - A - C*ln(R)^3) / B
  // This requires iterative solution, so we use a simplified approach

  const float temp_kelvin = temperature_celsius + KELVIN_OFFSET_;
  if (temp_kelvin <= ZERO_FLOAT_) {
    return false; // Invalid temperature
  }

  const float inv_temperature = ONE_FLOAT_ / temp_kelvin;

  // For most NTC thermistors, the C term is small, so we can approximate
  // ln(R) ≈ (1/T - A) / B
  const float ln_R_approx = (inv_temperature - coeff_a) / coeff_b;

  constexpr float kMaxLnResistanceApprox = 20.0F;
  if (ln_R_approx <= -kMaxLnResistanceApprox ||
      ln_R_approx >= kMaxLnResistanceApprox) {
    return false; // Result out of reasonable range
  }

  *resistance_ohms = std::exp(ln_R_approx);

  return true;
}

bool ConvertResistanceToTemperatureDefault(
    float resistance_ohms, float *temperature_celsius) noexcept {
  return ConvertResistanceToTemperatureSteinhartHart(
      resistance_ohms, NTC::Constants::STEINHART_HART_A_,
      NTC::Constants::STEINHART_HART_B_, NTC::Constants::STEINHART_HART_C_,
      temperature_celsius);
}

bool ConvertTemperatureToResistanceDefault(float temperature_celsius,
                                           float *resistance_ohms) noexcept {
  return ConvertTemperatureToResistanceSteinhartHart(
      temperature_celsius, NTC::Constants::STEINHART_HART_A_,
      NTC::Constants::STEINHART_HART_B_, NTC::Constants::STEINHART_HART_C_,
      resistance_ohms);
}

//--------------------------------------
//  Voltage Divider Calculations
//--------------------------------------

bool CalculateThermistorResistance(float voltage_thermistor,
                                   float reference_voltage,
                                   float series_resistance,
                                   float *resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateVoltage(voltage_thermistor, ZERO_FLOAT_, reference_voltage)) {
    return false;
  }

  if (reference_voltage <= ZERO_FLOAT_ || series_resistance <= ZERO_FLOAT_) {
    return false;
  }

  // Voltage divider equation: V_thermistor = V_ref * (R_thermistor / (R_series
  // + R_thermistor)) Solving for R_thermistor: R_thermistor = R_series *
  // (V_thermistor / (V_ref - V_thermistor))

  const float voltage_diff = reference_voltage - voltage_thermistor;
  if (std::abs(voltage_diff) < EPSILON_FLOAT_) {
    return false; // Voltage too close to reference voltage
  }

  const float ratio = voltage_thermistor / voltage_diff;
  *resistance_ohms = series_resistance * ratio;

  return true;
}

bool CalculateThermistorVoltage(float resistance_ohms, float reference_voltage,
                                float series_resistance,
                                float *voltage_thermistor) noexcept {
  if (voltage_thermistor == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (reference_voltage <= ZERO_FLOAT_ || series_resistance <= ZERO_FLOAT_) {
    return false;
  }

  // Voltage divider equation: V_thermistor = V_ref * (R_thermistor / (R_series
  // + R_thermistor))

  const float total_resistance = series_resistance + resistance_ohms;
  if (total_resistance <= ZERO_FLOAT_) {
    return false;
  }

  *voltage_thermistor =
      reference_voltage * (resistance_ohms / total_resistance);

  return true;
}

bool CalculateVoltageDividerRatio(float resistance_ohms,
                                  float series_resistance,
                                  float *ratio) noexcept {
  if (ratio == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (series_resistance <= ZERO_FLOAT_) {
    return false;
  }

  const float total_resistance = series_resistance + resistance_ohms;
  if (total_resistance <= ZERO_FLOAT_) {
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
  return (resistance_ohms >= min_resistance &&
          resistance_ohms <= max_resistance);
}

bool ValidateTemperature(float temperature_celsius, float min_temperature,
                         float max_temperature) noexcept {
  return (temperature_celsius >= min_temperature &&
          temperature_celsius <= max_temperature);
}

bool ValidateVoltage(float voltage_volts, float min_voltage,
                     float max_voltage) noexcept {
  return (voltage_volts >= min_voltage && voltage_volts <= max_voltage);
}

bool ValidateBetaValue(float beta_value) noexcept {
  // Beta values for NTC thermistors typically range from 2000K to 5000K
  return (beta_value >= NTC::Constants::MIN_BETA_VALUE_ &&
          beta_value <= NTC::Constants::MAX_BETA_VALUE_);
}

bool ValidateSteinhartHartCoefficients(float coeff_a, float coeff_b,
                                       float coeff_c) noexcept {
  // Validate reasonable ranges for Steinhart-Hart coefficients
  // These are typical ranges, but may vary for specific thermistors
  constexpr float MIN_COEFF_A_ = -1e-2F;
  constexpr float MAX_COEFF_A_ = 1e-2F;
  constexpr float MIN_COEFF_B_ = 1e-4F;
  constexpr float MAX_COEFF_B_ = 1e-3F;
  constexpr float MIN_COEFF_C_ = -1e-7F;
  constexpr float MAX_COEFF_C_ = 1e-7F;
  return (coeff_a >= MIN_COEFF_A_ && coeff_a <= MAX_COEFF_A_ &&
          coeff_b >= MIN_COEFF_B_ && coeff_b <= MAX_COEFF_B_ &&
          coeff_c >= MIN_COEFF_C_ && coeff_c <= MAX_COEFF_C_);
}

//--------------------------------------
//  Utility Functions
//--------------------------------------

bool CalculateBetaValue(float temp1_celsius, float resistance1_ohms,
                        float temp2_celsius, float resistance2_ohms,
                        float *beta_value) noexcept {
  if (beta_value == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temp1_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_) ||
      !ValidateTemperature(temp2_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_)) {
    return false;
  }

  if (!ValidateResistance(resistance1_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_) ||
      !ValidateResistance(resistance2_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (std::abs(temp1_celsius - temp2_celsius) < EPSILON_FLOAT_) {
    return false; // Temperatures too close
  }

  if (resistance1_ohms <= ZERO_FLOAT_ || resistance2_ohms <= ZERO_FLOAT_) {
    return false;
  }

  // Beta equation: β = ln(R1/R2) / (1/T1 - 1/T2)
  // Where: T1, T2 = temperatures in Kelvin, R1, R2 = resistances

  const float temp1_kelvin = temp1_celsius + KELVIN_OFFSET_;
  const float temp2_kelvin = temp2_celsius + KELVIN_OFFSET_;

  const float ln_ratio = std::log(resistance1_ohms / resistance2_ohms);
  const float inv_diff =
      (ONE_FLOAT_ / temp1_kelvin) - (ONE_FLOAT_ / temp2_kelvin);

  if (std::abs(inv_diff) < EPSILON_FLOAT_) {
    return false; // Invalid temperature difference
  }

  *beta_value = ln_ratio / inv_diff;

  return ValidateBetaValue(*beta_value);
}

bool CalculateSteinhartHartCoefficients(
    float temp1_celsius, float resistance1_ohms, float temp2_celsius,
    float resistance2_ohms, float temp3_celsius, float resistance3_ohms,
    float *coeff_a, float *coeff_b, float *coeff_c) noexcept {
  if (coeff_a == nullptr || coeff_b == nullptr || coeff_c == nullptr) {
    return false;
  }

  if (!ValidateTemperature(temp1_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_) ||
      !ValidateTemperature(temp2_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_) ||
      !ValidateTemperature(temp3_celsius, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_)) {
    return false;
  }

  if (!ValidateResistance(resistance1_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_) ||
      !ValidateResistance(resistance2_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_) ||
      !ValidateResistance(resistance3_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  // Check that temperatures are different
  if (std::abs(temp1_celsius - temp2_celsius) < EPSILON_FLOAT_ ||
      std::abs(temp2_celsius - temp3_celsius) < EPSILON_FLOAT_ ||
      std::abs(temp1_celsius - temp3_celsius) < EPSILON_FLOAT_) {
    return false;
  }

  // Convert temperatures to Kelvin
  const float temp1_kelvin = temp1_celsius + KELVIN_OFFSET_;
  const float temp2_kelvin = temp2_celsius + KELVIN_OFFSET_;
  const float temp3_kelvin = temp3_celsius + KELVIN_OFFSET_;

  // Calculate natural logarithms of resistances
  const float ln_R1 = std::log(resistance1_ohms);
  const float ln_R2 = std::log(resistance2_ohms);
  const float ln_R3 = std::log(resistance3_ohms);

  // Calculate inverse temperatures
  const float inv_T1 = ONE_FLOAT_ / temp1_kelvin;
  const float inv_T2 = ONE_FLOAT_ / temp2_kelvin;
  const float inv_T3 = ONE_FLOAT_ / temp3_kelvin;

  // Solve the system of equations using Cramer's rule
  // Matrix: [1 ln_R1 ln_R1^3] [A]   [1/T1]
  //         [1 ln_R2 ln_R2^3] [B] = [1/T2]
  //         [1 ln_R3 ln_R3^3] [C]   [1/T3]

  const float det =
      (ONE_FLOAT_ *
       (ln_R2 * ln_R3 * ln_R3 * ln_R3 - ln_R3 * ln_R2 * ln_R2 * ln_R2)) +
      (ln_R1 *
       (ln_R3 * ln_R2 * ln_R2 * ln_R2 - ln_R2 * ln_R3 * ln_R3 * ln_R3)) +
      (ln_R1 * ln_R1 * ln_R1 * (ln_R2 - ln_R3));

  if (std::abs(det) < EPSILON_DOUBLE_) {
    return false; // Singular matrix
  }

  const float det_a =
      (inv_T1 *
       (ln_R2 * ln_R3 * ln_R3 * ln_R3 - ln_R3 * ln_R2 * ln_R2 * ln_R2)) +
      (inv_T2 *
       (ln_R3 * ln_R1 * ln_R1 * ln_R1 - ln_R1 * ln_R3 * ln_R3 * ln_R3)) +
      (inv_T3 *
       (ln_R1 * ln_R2 * ln_R2 * ln_R2 - ln_R2 * ln_R1 * ln_R1 * ln_R1));

  const float det_b =
      (ONE_FLOAT_ *
       (inv_T2 * ln_R3 * ln_R3 * ln_R3 - inv_T3 * ln_R2 * ln_R2 * ln_R2)) +
      (ln_R1 *
       (inv_T3 * ln_R1 * ln_R1 * ln_R1 - inv_T1 * ln_R3 * ln_R3 * ln_R3)) +
      (ln_R1 * ln_R1 * ln_R1 * (inv_T1 - inv_T2));

  const float det_c = (ONE_FLOAT_ * (ln_R2 * inv_T3 - ln_R3 * inv_T2)) +
                      (ln_R1 * (ln_R3 * inv_T1 - ln_R1 * inv_T3)) +
                      (ln_R1 * ln_R1 * ln_R1 * (inv_T2 - inv_T1));

  *coeff_a = det_a / det;
  *coeff_b = det_b / det;
  *coeff_c = det_c / det;

  return ValidateSteinhartHartCoefficients(*coeff_a, *coeff_b, *coeff_c);
}

bool CalculateTemperatureAccuracy(float resistance_ohms,
                                  float resistance_tolerance,
                                  float beta_tolerance,
                                  float *accuracy_celsius) noexcept {
  if (accuracy_celsius == nullptr) {
    return false;
  }

  if (!ValidateResistance(resistance_ohms, MIN_RESISTANCE_OHMS_,
                          MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  constexpr float kMinTolerance = 0.0F;
  constexpr float kMaxTolerance = 1.0F;
  if (resistance_tolerance < kMinTolerance ||
      resistance_tolerance > kMaxTolerance) {
    return false;
  }

  if (beta_tolerance < kMinTolerance || beta_tolerance > kMaxTolerance) {
    return false;
  }

  // Estimate temperature accuracy based on resistance and beta tolerances
  // This is a simplified calculation - actual accuracy depends on many factors
  constexpr float kTypicalBetaValue =
      3435.0F; // Typical beta value for NTC thermistors
  constexpr float kResistanceErrorDivisor =
      100.0F;                                  // Rough approximation divisor
  constexpr float kBetaErrorDivisor = 1000.0F; // Rough approximation divisor

  const float resistance_error = resistance_ohms * resistance_tolerance;
  const float beta_error = kTypicalBetaValue * beta_tolerance;

  // Convert resistance error to temperature error (approximate)
  const float temp_error_resistance =
      resistance_error / kResistanceErrorDivisor;

  // Convert beta error to temperature error (approximate)
  const float temp_error_beta = beta_error / kBetaErrorDivisor;

  *accuracy_celsius =
      std::sqrt((temp_error_resistance * temp_error_resistance) +
                (temp_error_beta * temp_error_beta));

  return true;
}

bool CalculateOptimalSeriesResistance(float thermistor_resistance_at_25c,
                                      float min_temperature,
                                      float max_temperature,
                                      float *optimal_resistance) noexcept {
  if (optimal_resistance == nullptr) {
    return false;
  }

  constexpr float kMinThermistorResistance = 100.0F;
  if (!ValidateResistance(thermistor_resistance_at_25c,
                          kMinThermistorResistance, MAX_RESISTANCE_OHMS_)) {
    return false;
  }

  if (!ValidateTemperature(min_temperature, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_) ||
      !ValidateTemperature(max_temperature, MIN_TEMPERATURE_CELSIUS_,
                           MAX_TEMPERATURE_CELSIUS_)) {
    return false;
  }

  if (min_temperature >= max_temperature) {
    return false;
  }

  // Calculate resistances at min and max temperatures
  float r_min = 0.0F;
  float r_max = 0.0F;
  if (!ConvertTemperatureToResistanceBeta(
          min_temperature, thermistor_resistance_at_25c, 3435.0F, &r_min) ||
      !ConvertTemperatureToResistanceBeta(
          max_temperature, thermistor_resistance_at_25c, 3435.0F, &r_max)) {
    return false;
  }

  // Optimal series resistance is the geometric mean of min and max resistances
  // This provides the most linear voltage response over the temperature range
  *optimal_resistance = std::sqrt(r_min * r_max);

  return true;
}

} // namespace NTC