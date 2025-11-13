/**
 * @file NtcThermistor.cpp
 * @brief Hardware-agnostic NTC thermistor driver implementation.
 *
 * This file contains the complete implementation of the NtcThermistor class
 * that provides temperature measurement using NTC thermistors with support
 * for various NTC types and conversion methods.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 *
 * @note This file is included by NtcThermistor.h for template instantiation.
 *       It should not be compiled separately when included.
 */

#ifndef NTC_THERMISTOR_IMPL
#define NTC_THERMISTOR_IMPL

// When included from header, use relative path; when compiled directly, use standard include
#ifdef NTC_THERMISTOR_HEADER_INCLUDED
#include "../inc/NtcThermistor.h"
#include "../inc/NtcConversion.h"
#include "../inc/NtcLookupTable.h"
#else
#include "NtcConversion.h"
#include "NtcLookupTable.h"
#include "NtcThermistor.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstring>

using NTC::Constants::kCelsiusToFahrenheitMultiplier;
using NTC::Constants::kFahrenheitOffset;
using NTC::Constants::kFahrenheitToCelsiusMultiplier;
using NTC::Constants::kKelvinOffset;
using NTC::Constants::kMillisecondsPerSecond;
using NTC::Constants::kOneFloat;
using NTC::Constants::kZeroFloat;

//--------------------------------------
//  CONSTRUCTORS AND DESTRUCTOR
//--------------------------------------

template <typename AdcType>
NtcThermistor<AdcType>::NtcThermistor(ntc_type_t ntc_type, AdcType* adc_interface) noexcept
    : config_(), adc_interface_(adc_interface), initialized_(false),
      filtered_temperature_(kZeroFloat), filter_initialized_(false) {

  // Initialize configuration for NTC type
  initializeConfigForType(ntc_type, &config_);
}

template <typename AdcType>
NtcThermistor<AdcType>::NtcThermistor(const ntc_config_t& config, AdcType* adc_interface) noexcept
    : config_(config), adc_interface_(adc_interface), initialized_(false),
      filtered_temperature_(kZeroFloat), filter_initialized_(false) {}

//--------------------------------------
//  INITIALIZATION AND CONFIGURATION
//--------------------------------------

template <typename AdcType>
bool NtcThermistor<AdcType>::Initialize() noexcept {
  if (initialized_) {
    return true;
  }

  // Validate configuration
  ntc_err_t validation_error = validateConfiguration(config_);
  if (validation_error != NTC_SUCCESS) {
    return false;
  }

  // Validate ADC interface
  if (adc_interface_ == nullptr) {
    return false;
  }

  // Initialize ADC interface if needed
  if (!adc_interface_->IsInitialized()) {
    if (!adc_interface_->EnsureInitialized()) {
      return false;
    }
  }

  // Validate ADC channel
  if (!adc_interface_->IsChannelAvailable(config_.adc_channel)) {
    return false;
  }

  // Reset filter
  filter_initialized_ = false;
  filtered_temperature_ = kZeroFloat;

  initialized_ = true;
  return true;
}

template <typename AdcType>
bool NtcThermistor<AdcType>::Deinitialize() noexcept {
  if (!initialized_) {
    return true;
  }

  initialized_ = false;
  filter_initialized_ = false;
  filtered_temperature_ = kZeroFloat;

  return true;
}

template <typename AdcType>
bool NtcThermistor<AdcType>::IsInitialized() const noexcept {
  return initialized_;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::GetConfiguration(ntc_config_t* config) const noexcept {
  if (config == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  *config = config_;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetConfiguration(const ntc_config_t& config) noexcept {
  // Validate new configuration
  ntc_err_t validation_error = validateConfiguration(config);
  if (validation_error != NTC_SUCCESS) {
    return validation_error;
  }

  config_ = config;

  // Reset filter when configuration changes
  filter_initialized_ = false;
  filtered_temperature_ = kZeroFloat;

  return NTC_SUCCESS;
}

//--------------------------------------
//  TEMPERATURE READING
//--------------------------------------

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::ReadTemperatureCelsius(float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  // Read voltage from ADC
  float voltage_volts = 0.0f;
  ntc_err_t voltage_error = readAdcVoltage(&voltage_volts);
  if (voltage_error != NTC_SUCCESS) {
    return voltage_error;
  }

  // Calculate resistance
  float resistance_ohms = 0.0f;
  ntc_err_t resistance_error = calculateResistance(voltage_volts, &resistance_ohms);
  if (resistance_error != NTC_SUCCESS) {
    return resistance_error;
  }

  // Convert resistance to temperature
  float raw_temperature = 0.0f;
  ntc_err_t conversion_error = convertResistanceToTemperature(resistance_ohms, &raw_temperature);
  if (conversion_error != NTC_SUCCESS) {
    return conversion_error;
  }

  // Apply calibration offset
  *temperature_celsius = raw_temperature + config_.calibration_offset;

  // Apply filtering if enabled
  if (config_.enable_filtering) {
    *temperature_celsius = applyFiltering(*temperature_celsius);
  }

  // Validate temperature range
  if (!NTC::ValidateTemperature(*temperature_celsius, config_.min_temperature,
                                config_.max_temperature)) {
    return NTC_ERR_TEMPERATURE_OUT_OF_RANGE;
  }

  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::ReadTemperatureFahrenheit(
    float* temperature_fahrenheit) noexcept {
  if (temperature_fahrenheit == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  float celsius = 0.0f;
  ntc_err_t error = ReadTemperatureCelsius(&celsius);
  if (error == NTC_SUCCESS) {
    *temperature_fahrenheit = CelsiusToFahrenheit(celsius);
  }
  return error;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::ReadTemperatureKelvin(float* temperature_kelvin) noexcept {
  if (temperature_kelvin == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  float celsius = 0.0f;
  ntc_err_t error = ReadTemperatureCelsius(&celsius);
  if (error == NTC_SUCCESS) {
    *temperature_kelvin = CelsiusToKelvin(celsius);
  }
  return error;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::ReadTemperature(ntc_reading_t* reading) noexcept {
  if (reading == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  // Initialize reading structure
  reading->error = NTC_SUCCESS;
  reading->is_valid = false;
  reading->timestamp_us = 0;        // Implementer should provide timestamp
  reading->accuracy_celsius = 0.5F; // Estimate based on typical NTC accuracy

  // Read temperature
  float temperature_celsius = 0.0f;
  ntc_err_t error = ReadTemperatureCelsius(&temperature_celsius);

  if (error == NTC_SUCCESS) {
    // Get additional data
    float resistance_ohms = 0.0f;
    GetResistance(&resistance_ohms);

    float voltage_volts = 0.0f;
    GetVoltage(&voltage_volts);

    uint32_t adc_raw = 0U;
    GetRawAdcValue(&adc_raw);

    // Fill reading structure
    reading->temperature_celsius = temperature_celsius;
    reading->temperature_fahrenheit = CelsiusToFahrenheit(temperature_celsius);
    reading->temperature_kelvin = CelsiusToKelvin(temperature_celsius);
    reading->resistance_ohms = resistance_ohms;
    reading->voltage_volts = voltage_volts;
    reading->adc_raw_value = adc_raw;
    reading->is_valid = true;
  }

  reading->error = error;
  return error;
}

//--------------------------------------
//  RESISTANCE AND VOLTAGE
//--------------------------------------

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::GetResistance(float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  float voltage_volts = 0.0f;
  ntc_err_t voltage_error = readAdcVoltage(&voltage_volts);
  if (voltage_error != NTC_SUCCESS) {
    return voltage_error;
  }

  return calculateResistance(voltage_volts, resistance_ohms);
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::GetVoltage(float* voltage_volts) noexcept {
  if (voltage_volts == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  return readAdcVoltage(voltage_volts);
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::GetRawAdcValue(uint32_t* adc_value) noexcept {
  if (adc_value == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  if (adc_interface_ == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  // Convert AdcError to ntc_err_t helper
  auto convert_error = [](NTC::AdcError err) -> ntc_err_t {
    switch (err) {
    case NTC::AdcError::SUCCESS:
      return NTC_SUCCESS;
    case NTC::AdcError::NOT_INITIALIZED:
      return NTC_ERR_NOT_INITIALIZED;
    case NTC::AdcError::INVALID_CHANNEL:
      return NTC_ERR_INVALID_PARAMETER;
    case NTC::AdcError::READ_FAILED:
      return NTC_ERR_ADC_READ_FAILED;
    case NTC::AdcError::TIMEOUT:
      return NTC_ERR_TIMEOUT;
    case NTC::AdcError::HARDWARE_ERROR:
      return NTC_ERR_HARDWARE_FAULT;
    default:
      return NTC_ERR_ADC_READ_FAILED;
    }
  };

  // Perform sampling if configured
  if (config_.sample_count > 1) {
    uint64_t sum = 0;
    uint32_t successful_samples = 0;

    for (uint32_t i = 0; i < config_.sample_count; ++i) {
      uint32_t sample_value = 0;
      NTC::AdcError err = adc_interface_->ReadChannelCount(config_.adc_channel, &sample_value);
      if (err == NTC::AdcError::SUCCESS) {
        sum += sample_value;
        successful_samples++;
      }

      // Simple delay loop if delay is configured (not accurate but hardware-agnostic)
      if (config_.sample_delay_ms > 0 && i < config_.sample_count - 1) {
        // Simple busy-wait delay (approximate)
        volatile uint32_t delay_count =
            config_.sample_delay_ms * static_cast<uint32_t>(kMillisecondsPerSecond);
        while (delay_count-- > 0) {
          // Busy wait
        }
      }
    }

    if (successful_samples == 0) {
      return NTC_ERR_ADC_READ_FAILED;
    }

    *adc_value = static_cast<uint32_t>(sum / successful_samples);
    return NTC_SUCCESS;
  }
  // Single sample
  {
    uint32_t sample_value = 0;
    NTC::AdcError err = adc_interface_->ReadChannelCount(config_.adc_channel, &sample_value);
    if (err != NTC::AdcError::SUCCESS) {
      return convert_error(err);
    }
    *adc_value = sample_value;
    return NTC_SUCCESS;
  }
}

//--------------------------------------
//  CALIBRATION
//--------------------------------------

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::Calibrate(float reference_temperature_celsius) noexcept {
  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  // Read current temperature
  float current_temperature = 0.0f;
  ntc_err_t read_error = ReadTemperatureCelsius(&current_temperature);
  if (read_error != NTC_SUCCESS) {
    return read_error;
  }

  // Calculate calibration offset
  float new_offset = reference_temperature_celsius - current_temperature;

  // Apply offset
  config_.calibration_offset = new_offset;

  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetCalibrationOffset(float offset_celsius) noexcept {
  config_.calibration_offset = offset_celsius;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::GetCalibrationOffset(float* offset_celsius) const noexcept {
  if (offset_celsius == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  *offset_celsius = config_.calibration_offset;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::ResetCalibration() noexcept {
  return SetCalibrationOffset(kZeroFloat);
}

//--------------------------------------
//  CONFIGURATION METHODS
//--------------------------------------

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetConversionMethod(ntc_conversion_method_t method) noexcept {
  config_.conversion_method = method;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetVoltageDivider(float series_resistance) noexcept {
  if (series_resistance <= kZeroFloat) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.series_resistance = series_resistance;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetReferenceVoltage(float reference_voltage) noexcept {
  if (reference_voltage <= kZeroFloat) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.reference_voltage = reference_voltage;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetBetaValue(float beta_value) noexcept {
  if (!NTC::ValidateBetaValue(beta_value)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.beta_value = beta_value;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetAdcChannel(uint8_t adc_channel) noexcept {
  if (adc_interface_ != nullptr && !adc_interface_->IsChannelAvailable(adc_channel)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.adc_channel = adc_channel;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetSamplingParameters(uint32_t sample_count,
                                                        uint32_t sample_delay_ms) noexcept {
  if (sample_count == 0) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.sample_count = sample_count;
  config_.sample_delay_ms = sample_delay_ms;
  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::SetFiltering(bool enable, float alpha) noexcept {
  constexpr float kMinFilterAlpha = 0.0F;
  constexpr float kMaxFilterAlpha = 1.0F;
  if (alpha < kMinFilterAlpha || alpha > kMaxFilterAlpha) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.enable_filtering = enable;
  config_.filter_alpha = alpha;

  // Reset filter when changing settings
  filter_initialized_ = false;
  filtered_temperature_ = kZeroFloat;

  return NTC_SUCCESS;
}

//--------------------------------------
//  UTILITY FUNCTIONS
//--------------------------------------

template <typename AdcType>
float NtcThermistor<AdcType>::CelsiusToFahrenheit(float celsius) noexcept {
  return celsius * kCelsiusToFahrenheitMultiplier + kFahrenheitOffset;
}

template <typename AdcType>
float NtcThermistor<AdcType>::FahrenheitToCelsius(float fahrenheit) noexcept {
  return (fahrenheit - kFahrenheitOffset) * kFahrenheitToCelsiusMultiplier;
}

template <typename AdcType>
float NtcThermistor<AdcType>::CelsiusToKelvin(float celsius) noexcept {
  return celsius + kKelvinOffset;
}

template <typename AdcType>
float NtcThermistor<AdcType>::KelvinToCelsius(float kelvin) noexcept {
  return kelvin - kKelvinOffset;
}

template <typename AdcType>
const char* NtcThermistor<AdcType>::GetErrorString(ntc_err_t error) noexcept {
  switch (error) {
  case NTC_SUCCESS:
    return "Success";
  case NTC_ERR_FAILURE:
    return "General failure";
  case NTC_ERR_NOT_INITIALIZED:
    return "Not initialized";
  case NTC_ERR_ALREADY_INITIALIZED:
    return "Already initialized";
  case NTC_ERR_INVALID_PARAMETER:
    return "Invalid parameter";
  case NTC_ERR_NULL_POINTER:
    return "Null pointer";
  case NTC_ERR_OUT_OF_MEMORY:
    return "Out of memory";
  case NTC_ERR_ADC_READ_FAILED:
    return "ADC read failed";
  case NTC_ERR_INVALID_RESISTANCE:
    return "Invalid resistance value";
  case NTC_ERR_TEMPERATURE_OUT_OF_RANGE:
    return "Temperature out of range";
  case NTC_ERR_LOOKUP_TABLE_ERROR:
    return "Lookup table error";
  case NTC_ERR_CONVERSION_FAILED:
    return "Temperature conversion failed";
  case NTC_ERR_CALIBRATION_FAILED:
    return "Calibration failed";
  case NTC_ERR_UNSUPPORTED_OPERATION:
    return "Operation not supported";
  case NTC_ERR_TIMEOUT:
    return "Operation timeout";
  case NTC_ERR_HARDWARE_FAULT:
    return "Hardware fault";
  default:
    return "Unknown error";
  }
}

template <typename AdcType>
const char* NtcThermistor<AdcType>::GetTypeString(ntc_type_t type) noexcept {
  switch (type) {
  case NTC_TYPE_UNKNOWN:
    return "Unknown";
  case NTC_TYPE_NTCG163JFT103FT1S:
    return "NTCG163JFT103FT1S";
  case NTC_TYPE_NTCG164JF103FT1S:
    return "NTCG164JF103FT1S";
  case NTC_TYPE_NTCG163JF103FT1S:
    return "NTCG163JF103FT1S";
  case NTC_TYPE_CUSTOM:
    return "Custom";
  default:
    return "Invalid";
  }
}

//--------------------------------------
//  PRIVATE HELPER METHODS
//--------------------------------------

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::validateConfiguration(const ntc_config_t& config) const noexcept {
  if (config.resistance_at_25c <= kZeroFloat) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (!NTC::ValidateBetaValue(config.beta_value)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.reference_voltage <= kZeroFloat) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.series_resistance <= kZeroFloat) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.sample_count == 0) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.min_temperature >= config.max_temperature) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  constexpr float kMinFilterAlpha = 0.0f;
  constexpr float kMaxFilterAlpha = 1.0f;
  if (config.enable_filtering &&
      (config.filter_alpha < kMinFilterAlpha || config.filter_alpha > kMaxFilterAlpha)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::readAdcVoltage(float* voltage_volts) noexcept {
  if (voltage_volts == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (adc_interface_ == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  // Convert AdcError to ntc_err_t helper
  auto convert_error = [](NTC::AdcError err) -> ntc_err_t {
    switch (err) {
    case NTC::AdcError::SUCCESS:
      return NTC_SUCCESS;
    case NTC::AdcError::NOT_INITIALIZED:
      return NTC_ERR_NOT_INITIALIZED;
    case NTC::AdcError::INVALID_CHANNEL:
      return NTC_ERR_INVALID_PARAMETER;
    case NTC::AdcError::READ_FAILED:
      return NTC_ERR_ADC_READ_FAILED;
    case NTC::AdcError::TIMEOUT:
      return NTC_ERR_TIMEOUT;
    case NTC::AdcError::HARDWARE_ERROR:
      return NTC_ERR_HARDWARE_FAULT;
    default:
      return NTC_ERR_ADC_READ_FAILED;
    }
  };

  // Perform sampling if configured
  if (config_.sample_count > 1) {
    float sum = kZeroFloat;
    uint32_t successful_samples = 0;

    for (uint32_t i = 0; i < config_.sample_count; ++i) {
      float sample_voltage = kZeroFloat;
      NTC::AdcError err = adc_interface_->ReadChannelV(config_.adc_channel, &sample_voltage);
      if (err == NTC::AdcError::SUCCESS) {
        sum += sample_voltage;
        successful_samples++;
      }

      // Simple delay loop if delay is configured (not accurate but hardware-agnostic)
      if (config_.sample_delay_ms > 0 && i < config_.sample_count - 1) {
        // Simple busy-wait delay (approximate)
        volatile uint32_t delay_count =
            config_.sample_delay_ms * static_cast<uint32_t>(kMillisecondsPerSecond);
        while (delay_count-- > 0) {
          // Busy wait
        }
      }
    }

    if (successful_samples == 0) {
      return NTC_ERR_ADC_READ_FAILED;
    }

    *voltage_volts = sum / static_cast<float>(successful_samples);
    return NTC_SUCCESS;
  } else {
    // Single sample
    float sample_voltage = 0.0f;
    NTC::AdcError err = adc_interface_->ReadChannelV(config_.adc_channel, &sample_voltage);
    if (err != NTC::AdcError::SUCCESS) {
      return convert_error(err);
    }
    *voltage_volts = sample_voltage;
    return NTC_SUCCESS;
  }
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::calculateResistance(float voltage_volts,
                                                      float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!NTC::CalculateThermistorResistance(voltage_volts, config_.reference_voltage,
                                          config_.series_resistance, resistance_ohms)) {
    return NTC_ERR_CONVERSION_FAILED;
  }

  return NTC_SUCCESS;
}

template <typename AdcType>
ntc_err_t NtcThermistor<AdcType>::convertResistanceToTemperature(
    float resistance_ohms, float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  // Choose conversion method
  switch (config_.conversion_method) {
  case NTC_CONVERSION_LOOKUP_TABLE: {
    const ntc_lookup_table_t* table = GetNtcLookupTable(static_cast<int>(config_.type));
    if (table != nullptr &&
        FindTemperatureFromLookupTable(table, resistance_ohms, temperature_celsius)) {
      return NTC_SUCCESS;
    }
    // Fall back to mathematical conversion if lookup fails
    break;
  }

  case NTC_CONVERSION_MATHEMATICAL:
  case NTC_CONVERSION_AUTO:
  default:
    break;
  }

  // Use mathematical conversion (beta parameter)
  if (!NTC::ConvertResistanceToTemperatureBeta(resistance_ohms, config_.resistance_at_25c,
                                               config_.beta_value, temperature_celsius)) {
    return NTC_ERR_CONVERSION_FAILED;
  }

  return NTC_SUCCESS;
}

template <typename AdcType>
float NtcThermistor<AdcType>::applyFiltering(float new_temperature) noexcept {
  if (!config_.enable_filtering) {
    return new_temperature;
  }

  if (!filter_initialized_) {
    filtered_temperature_ = new_temperature;
    filter_initialized_ = true;
    return new_temperature;
  }

  // Apply exponential moving average
  filtered_temperature_ = config_.filter_alpha * new_temperature +
                          (kOneFloat - config_.filter_alpha) * filtered_temperature_;

  return filtered_temperature_;
}

template <typename AdcType>
void NtcThermistor<AdcType>::initializeConfigForType(ntc_type_t ntc_type,
                                                     ntc_config_t* config) noexcept {
  if (config == nullptr) {
    return;
  }

  // Set default values
  *config = NTC_CONFIG_DEFAULT();
  config->type = ntc_type;

  // Set type-specific values
  switch (ntc_type) {
  case NTC_TYPE_NTCG163JFT103FT1S:
  case NTC_TYPE_NTCG164JF103FT1S:
  case NTC_TYPE_NTCG163JF103FT1S: {
    config->resistance_at_25c = NTC::DefaultConfig::kDefaultResistanceAt25C;
    config->beta_value = NTC::DefaultConfig::kDefaultBetaValue;
    break;
  }

  case NTC_TYPE_CUSTOM:
  case NTC_TYPE_UNKNOWN:
  default:
    // Use default values from NTC_CONFIG_DEFAULT
    break;
  }
}

#endif // NTC_THERMISTOR_IMPL