/**
 * @file NtcThermistor.cpp
 * @brief Hardware-agnostic NTC thermistor driver implementation.
 *
 * This file contains the complete implementation of the NtcThermistor class
 * that provides temperature measurement using NTC thermistors with support
 * for various NTC types and conversion methods.
 *
 * @author HardFOC Development Team
 * @date 2025
 * @copyright HardFOC
 */

#include "NtcThermistor.h"
#include "NtcConversion.h"
#include "NtcLookupTable.h"

#include <algorithm>
#include <cmath>
#include <cstring>

//--------------------------------------
//  CONSTRUCTORS AND DESTRUCTOR
//--------------------------------------

NtcThermistor::NtcThermistor(ntc_type_t ntc_type, BaseAdc* adc_interface) noexcept
    : config_(), adc_interface_(adc_interface), initialized_(false), filtered_temperature_(0.0f),
      filter_initialized_(false) {

  // Initialize configuration for NTC type
  InitializeConfigForType(ntc_type, &config_);
}

NtcThermistor::NtcThermistor(const ntc_config_t& config, BaseAdc* adc_interface) noexcept
    : config_(config), adc_interface_(adc_interface), initialized_(false),
      filtered_temperature_(0.0f), filter_initialized_(false) {}

//--------------------------------------
//  INITIALIZATION AND CONFIGURATION
//--------------------------------------

bool NtcThermistor::Initialize() noexcept {
  if (initialized_) {
    return true;
  }

  // Validate configuration
  ntc_err_t validation_error = ValidateConfiguration(config_);
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
  filtered_temperature_ = 0.0f;

  initialized_ = true;
  return true;
}

bool NtcThermistor::Deinitialize() noexcept {
  if (!initialized_) {
    return true;
  }

  initialized_ = false;
  filter_initialized_ = false;
  filtered_temperature_ = 0.0f;

  return true;
}

bool NtcThermistor::IsInitialized() const noexcept {
  return initialized_;
}

ntc_err_t NtcThermistor::GetConfiguration(ntc_config_t* config) const noexcept {
  if (config == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  *config = config_;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetConfiguration(const ntc_config_t& config) noexcept {
  // Validate new configuration
  ntc_err_t validation_error = ValidateConfiguration(config);
  if (validation_error != NTC_SUCCESS) {
    return validation_error;
  }

  config_ = config;

  // Reset filter when configuration changes
  filter_initialized_ = false;
  filtered_temperature_ = 0.0f;

  return NTC_SUCCESS;
}

//--------------------------------------
//  TEMPERATURE READING
//--------------------------------------

ntc_err_t NtcThermistor::ReadTemperatureCelsius(float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  // Read voltage from ADC
  float voltage_volts;
  ntc_err_t voltage_error = ReadAdcVoltage(&voltage_volts);
  if (voltage_error != NTC_SUCCESS) {
    return voltage_error;
  }

  // Calculate resistance
  float resistance_ohms;
  ntc_err_t resistance_error = CalculateResistance(voltage_volts, &resistance_ohms);
  if (resistance_error != NTC_SUCCESS) {
    return resistance_error;
  }

  // Convert resistance to temperature
  float raw_temperature;
  ntc_err_t conversion_error = ConvertResistanceToTemperature(resistance_ohms, &raw_temperature);
  if (conversion_error != NTC_SUCCESS) {
    return conversion_error;
  }

  // Apply calibration offset
  *temperature_celsius = raw_temperature + config_.calibration_offset;

  // Apply filtering if enabled
  if (config_.enable_filtering) {
    *temperature_celsius = ApplyFiltering(*temperature_celsius);
  }

  // Validate temperature range
  if (!ValidateTemperature(*temperature_celsius, config_.min_temperature,
                           config_.max_temperature)) {
    return NTC_ERR_TEMPERATURE_OUT_OF_RANGE;
  }

  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::ReadTemperatureFahrenheit(float* temperature_fahrenheit) noexcept {
  if (temperature_fahrenheit == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  float celsius;
  ntc_err_t error = ReadTemperatureCelsius(&celsius);
  if (error == NTC_SUCCESS) {
    *temperature_fahrenheit = CelsiusToFahrenheit(celsius);
  }
  return error;
}

ntc_err_t NtcThermistor::ReadTemperatureKelvin(float* temperature_kelvin) noexcept {
  if (temperature_kelvin == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  float celsius;
  ntc_err_t error = ReadTemperatureCelsius(&celsius);
  if (error == NTC_SUCCESS) {
    *temperature_kelvin = CelsiusToKelvin(celsius);
  }
  return error;
}

ntc_err_t NtcThermistor::ReadTemperature(ntc_reading_t* reading) noexcept {
  if (reading == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  // Initialize reading structure
  reading->error = NTC_SUCCESS;
  reading->is_valid = false;
  reading->timestamp_us = 0;        // Implementer should provide timestamp
  reading->accuracy_celsius = 0.5f; // Estimate based on typical NTC accuracy

  // Read temperature
  float temperature_celsius;
  ntc_err_t error = ReadTemperatureCelsius(&temperature_celsius);

  if (error == NTC_SUCCESS) {
    // Get additional data
    float resistance_ohms;
    GetResistance(&resistance_ohms);

    float voltage_volts;
    GetVoltage(&voltage_volts);

    uint32_t adc_raw;
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

ntc_err_t NtcThermistor::GetResistance(float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  float voltage_volts;
  ntc_err_t voltage_error = ReadAdcVoltage(&voltage_volts);
  if (voltage_error != NTC_SUCCESS) {
    return voltage_error;
  }

  return CalculateResistance(voltage_volts, resistance_ohms);
}

ntc_err_t NtcThermistor::GetVoltage(float* voltage_volts) noexcept {
  if (voltage_volts == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  return ReadAdcVoltage(voltage_volts);
}

ntc_err_t NtcThermistor::GetRawAdcValue(uint32_t* adc_value) noexcept {
  if (adc_value == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  if (adc_interface_ == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  return static_cast<ntc_err_t>(adc_interface_->ReadChannelCount(
      config_.adc_channel, *adc_value, config_.sample_count, config_.sample_delay_ms));
}

//--------------------------------------
//  CALIBRATION
//--------------------------------------

ntc_err_t NtcThermistor::Calibrate(float reference_temperature_celsius) noexcept {
  if (!initialized_) {
    return NTC_ERR_NOT_INITIALIZED;
  }

  // Read current temperature
  float current_temperature;
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

ntc_err_t NtcThermistor::SetCalibrationOffset(float offset_celsius) noexcept {
  config_.calibration_offset = offset_celsius;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::GetCalibrationOffset(float* offset_celsius) const noexcept {
  if (offset_celsius == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  *offset_celsius = config_.calibration_offset;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::ResetCalibration() noexcept {
  return SetCalibrationOffset(0.0f);
}

//--------------------------------------
//  CONFIGURATION METHODS
//--------------------------------------

ntc_err_t NtcThermistor::SetConversionMethod(ntc_conversion_method_t method) noexcept {
  config_.conversion_method = method;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetVoltageDivider(float series_resistance) noexcept {
  if (series_resistance <= 0.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.series_resistance = series_resistance;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetReferenceVoltage(float reference_voltage) noexcept {
  if (reference_voltage <= 0.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.reference_voltage = reference_voltage;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetBetaValue(float beta_value) noexcept {
  if (!ValidateBetaValue(beta_value)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.beta_value = beta_value;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetAdcChannel(uint8_t adc_channel) noexcept {
  if (adc_interface_ != nullptr && !adc_interface_->IsChannelAvailable(adc_channel)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.adc_channel = adc_channel;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetSamplingParameters(uint32_t sample_count,
                                               uint32_t sample_delay_ms) noexcept {
  if (sample_count == 0) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.sample_count = sample_count;
  config_.sample_delay_ms = sample_delay_ms;
  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::SetFiltering(bool enable, float alpha) noexcept {
  if (alpha < 0.0f || alpha > 1.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  config_.enable_filtering = enable;
  config_.filter_alpha = alpha;

  // Reset filter when changing settings
  filter_initialized_ = false;
  filtered_temperature_ = 0.0f;

  return NTC_SUCCESS;
}

//--------------------------------------
//  UTILITY FUNCTIONS
//--------------------------------------

float NtcThermistor::CelsiusToFahrenheit(float celsius) noexcept {
  return celsius * 9.0f / 5.0f + 32.0f;
}

float NtcThermistor::FahrenheitToCelsius(float fahrenheit) noexcept {
  return (fahrenheit - 32.0f) * 5.0f / 9.0f;
}

float NtcThermistor::CelsiusToKelvin(float celsius) noexcept {
  return celsius + 273.15f;
}

float NtcThermistor::KelvinToCelsius(float kelvin) noexcept {
  return kelvin - 273.15f;
}

const char* NtcThermistor::GetErrorString(ntc_err_t error) noexcept {
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

const char* NtcThermistor::GetTypeString(ntc_type_t type) noexcept {
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

ntc_err_t NtcThermistor::ValidateConfiguration(const ntc_config_t& config) const noexcept {
  if (config.resistance_at_25c <= 0.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (!ValidateBetaValue(config.beta_value)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.reference_voltage <= 0.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.series_resistance <= 0.0f) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.sample_count == 0) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.min_temperature >= config.max_temperature) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  if (config.enable_filtering && (config.filter_alpha < 0.0f || config.filter_alpha > 1.0f)) {
    return NTC_ERR_INVALID_PARAMETER;
  }

  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::ReadAdcVoltage(float* voltage_volts) noexcept {
  if (voltage_volts == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (adc_interface_ == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  return static_cast<ntc_err_t>(adc_interface_->ReadChannelV(
      config_.adc_channel, *voltage_volts, config_.sample_count, config_.sample_delay_ms));
}

ntc_err_t NtcThermistor::CalculateResistance(float voltage_volts, float* resistance_ohms) noexcept {
  if (resistance_ohms == nullptr) {
    return NTC_ERR_NULL_POINTER;
  }

  if (!CalculateThermistorResistance(voltage_volts, config_.reference_voltage,
                                     config_.series_resistance, resistance_ohms)) {
    return NTC_ERR_CONVERSION_FAILED;
  }

  return NTC_SUCCESS;
}

ntc_err_t NtcThermistor::ConvertResistanceToTemperature(float resistance_ohms,
                                                        float* temperature_celsius) noexcept {
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
  if (!ConvertResistanceToTemperatureBeta(resistance_ohms, config_.resistance_at_25c,
                                          config_.beta_value, temperature_celsius)) {
    return NTC_ERR_CONVERSION_FAILED;
  }

  return NTC_SUCCESS;
}

float NtcThermistor::ApplyFiltering(float new_temperature) noexcept {
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
                          (1.0f - config_.filter_alpha) * filtered_temperature_;

  return filtered_temperature_;
}

void NtcThermistor::InitializeConfigForType(ntc_type_t ntc_type, ntc_config_t* config) noexcept {
  if (config == nullptr) {
    return;
  }

  // Set default values
  *config = NTC_CONFIG_DEFAULT();
  config->type = ntc_type;

  // Set type-specific values
  switch (ntc_type) {
  case NTC_TYPE_NTCG163JFT103FT1S:
    config->resistance_at_25c = 10000.0f;
    config->beta_value = 3435.0f;
    break;

  case NTC_TYPE_NTCG164JF103FT1S:
    config->resistance_at_25c = 10000.0f;
    config->beta_value = 3435.0f;
    break;

  case NTC_TYPE_NTCG163JF103FT1S:
    config->resistance_at_25c = 10000.0f;
    config->beta_value = 3435.0f;
    break;

  case NTC_TYPE_CUSTOM:
  case NTC_TYPE_UNKNOWN:
  default:
    // Use default values from NTC_CONFIG_DEFAULT
    break;
  }
}