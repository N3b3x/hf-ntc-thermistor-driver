/**
 * @file NtcThermistor.h
 * @brief Hardware-agnostic NTC thermistor driver for temperature measurement.
 *
 * This header provides a comprehensive interface for temperature measurement using
 * NTC (Negative Temperature Coefficient) thermistors. It supports various NTC types
 * and provides both lookup table and mathematical conversion methods.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cstdint>
#include <memory>

#include "NtcAdcInterface.h"
#include "NtcTypes.h"

//--------------------------------------
//  NtcThermistor Class
//--------------------------------------

/**
 * @class NtcThermistor
 * @brief Hardware-agnostic NTC thermistor driver
 *
 * This class provides a comprehensive interface for temperature measurement
 * using NTC thermistors. It supports various NTC types and provides both
 * lookup table and mathematical conversion methods.
 *
 * Key features:
 * - Hardware-agnostic design using CRTP-based ADC interface
 * - Support for multiple NTC types
 * - Dual conversion methods (lookup table and mathematical)
 * - Built-in calibration and filtering
 * - Comprehensive error handling
 *
 * @tparam AdcType The ADC implementation type that inherits from NTC::NtcAdcInterface<AdcType>
 *
 * @note The driver requires an ADC type that implements NTC::NtcAdcInterface
 *
 * @example Basic Usage
 * @code
 * // Define your ADC implementation
 * class MyAdc : public NTC::NtcAdcInterface<MyAdc> {
 * public:
 *   bool IsInitialized() const { return initialized_; }
 *   bool EnsureInitialized() { return true; }
 * // ... implement other required methods
 * };
 *
 * // Create ADC instance
 * MyAdc my_adc;
 * my_adc.Initialize();
 * // Create NTC thermistor with default configuration
 * NtcThermistor<MyAdc> thermistor(NTC_TYPE_NTCG163JFT103FT1S, &my_adc);
 * // Initialize and read temperature
 *if (thermistor.Initialize()) {
 * float temp_c;
 * if (thermistor.ReadTemperatureCelsius(&temp_c) == NTC_SUCCESS) {
 * printf("Temperature: %.2f°C\n", temp_c);
 * }
 * }
 * @endcode
 * @example Custom Configuration
 * @code
 * // Create custom configuration
 * ntc_config_t config = NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S();
 * config.adc_channel = 1;
 * config.series_resistance = 10000.0f;
 * config.conversion_method = NTC_CONVERSION_MATHEMATICAL;
 * config.enable_filtering = true;
 * config.filter_alpha = 0.1f;
 *
 * NtcThermistor<MyAdc> thermistor(config, &my_adc);
 * thermistor.Initialize();
 * @endcode
 */
template <typename AdcType>
class NtcThermistor {
public:
  //==============================================================//
  // CONSTRUCTORS AND DESTRUCTOR
  //==============================================================//

  /**
   * @brief Constructor with NTC type and ADC interface
   *
   * Creates an NTC thermistor instance with default configuration for the
   * specified NTC type. The configuration is automatically initialized with
   * appropriate values for the thermistor type.
   *
   * @param ntc_type NTC thermistor type (e.g., NTC_TYPE_NTCG163JFT103FT1S)
   * @param adc_interface Pointer to ADC interface (must inherit from NTC::NtcAdcInterface<AdcType>)
   *
   * @note The ADC interface must remain valid for the lifetime of the
   *       NtcThermistor instance. The driver does not take ownership.
   *
   * @warning The ADC interface pointer must not be nullptr.
   */
  NtcThermistor(ntc_type_t ntc_type, AdcType* adc_interface) noexcept;

  /**
   * @brief Constructor with custom configuration
   * @param config NTC configuration structure
   * @param adc_interface Pointer to ADC interface (must inherit from NTC::NtcAdcInterface<AdcType>)
   */
  NtcThermistor(const ntc_config_t& config, AdcType* adc_interface) noexcept;

  /**
   * @brief Copy constructor is deleted
   */
  NtcThermistor(const NtcThermistor&) = delete;

  /**
   * @brief Assignment operator is deleted
   */
  NtcThermistor& operator=(const NtcThermistor&) = delete;

  /**
   * @brief Move constructor
   */
  NtcThermistor(NtcThermistor&&) noexcept = default;

  /**
   * @brief Move assignment operator
   */
  NtcThermistor& operator=(NtcThermistor&&) noexcept = default;

  /**
   * @brief Virtual destructor
   */
  virtual ~NtcThermistor() noexcept = default;

  //==============================================================//
  // INITIALIZATION AND CONFIGURATION
  //==============================================================//

  /**
   * @brief Initialize the NTC thermistor driver
   * @return true if successful, false otherwise
   */
  bool Initialize() noexcept;

  /**
   * @brief Deinitialize the NTC thermistor driver
   * @return true if successful, false otherwise
   */
  bool Deinitialize() noexcept;

  /**
   * @brief Check if initialized
   * @return true if initialized, false otherwise
   */
  [[nodiscard]] bool IsInitialized() const noexcept;

  /**
   * @brief Get current configuration
   * @param config Pointer to store configuration
   * @return Error code
   */
  ntc_err_t GetConfiguration(ntc_config_t* config) const noexcept;

  /**
   * @brief Set configuration
   * @param config New configuration
   * @return Error code
   */
  ntc_err_t SetConfiguration(const ntc_config_t& config) noexcept;

  //==============================================================//
  // TEMPERATURE READING
  //==============================================================//

  /**
   * @brief Read temperature in Celsius
   *
   * Performs a complete temperature reading cycle: reads ADC, calculates
   * resistance, converts to temperature, applies filtering (if enabled),
   * and validates the result.
   *
   * @param temperature_celsius Pointer to store temperature (°C)
   * @return Error code (NTC_SUCCESS on success)
   *
   * @note This function may take several milliseconds depending on sample_count
   *       and sample_delay_ms configuration.
   *
   * @warning The driver must be initialized before calling this function.
   *          Check return value before using the temperature value.
   */
  ntc_err_t ReadTemperatureCelsius(float* temperature_celsius) noexcept;

  /**
   * @brief Read temperature in Fahrenheit
   * @param temperature_fahrenheit Pointer to store temperature
   * @return Error code
   */
  ntc_err_t ReadTemperatureFahrenheit(float* temperature_fahrenheit) noexcept;

  /**
   * @brief Read temperature in Kelvin
   * @param temperature_kelvin Pointer to store temperature
   * @return Error code
   */
  ntc_err_t ReadTemperatureKelvin(float* temperature_kelvin) noexcept;

  /**
   * @brief Read complete temperature information
   * @param reading Pointer to store reading information
   * @return Error code
   */
  ntc_err_t ReadTemperature(ntc_reading_t* reading) noexcept;

  //==============================================================//
  // RESISTANCE AND VOLTAGE
  //==============================================================//

  /**
   * @brief Get thermistor resistance
   * @param resistance_ohms Pointer to store resistance
   * @return Error code
   */
  ntc_err_t GetResistance(float* resistance_ohms) noexcept;

  /**
   * @brief Get voltage across thermistor
   * @param voltage_volts Pointer to store voltage
   * @return Error code
   */
  ntc_err_t GetVoltage(float* voltage_volts) noexcept;

  /**
   * @brief Get raw ADC value
   * @param adc_value Pointer to store ADC value
   * @return Error code
   */
  ntc_err_t GetRawAdcValue(uint32_t* adc_value) noexcept;

  //==============================================================//
  // CALIBRATION
  //==============================================================//

  /**
   * @brief Calibrate the thermistor
   * @param reference_temperature_celsius Known reference temperature
   * @return Error code
   */
  ntc_err_t Calibrate(float reference_temperature_celsius) noexcept;

  /**
   * @brief Set calibration offset
   * @param offset_celsius Calibration offset in Celsius
   * @return Error code
   */
  ntc_err_t SetCalibrationOffset(float offset_celsius) noexcept;

  /**
   * @brief Get calibration offset
   * @param offset_celsius Pointer to store offset
   * @return Error code
   */
  ntc_err_t GetCalibrationOffset(float* offset_celsius) const noexcept;

  /**
   * @brief Reset calibration
   * @return Error code
   */
  ntc_err_t ResetCalibration() noexcept;

  //==============================================================//
  // CONFIGURATION METHODS
  //==============================================================//

  /**
   * @brief Set conversion method
   * @param method Conversion method
   * @return Error code
   */
  ntc_err_t SetConversionMethod(ntc_conversion_method_t method) noexcept;

  /**
   * @brief Set voltage divider parameters
   * @param series_resistance Series resistance (ohms)
   * @return Error code
   */
  ntc_err_t SetVoltageDivider(float series_resistance) noexcept;

  /**
   * @brief Set reference voltage
   * @param reference_voltage Reference voltage (V)
   * @return Error code
   */
  ntc_err_t SetReferenceVoltage(float reference_voltage) noexcept;

  /**
   * @brief Set beta value
   * @param beta_value Beta value (K)
   * @return Error code
   */
  ntc_err_t SetBetaValue(float beta_value) noexcept;

  /**
   * @brief Set ADC channel
   * @param adc_channel ADC channel number
   * @return Error code
   */
  ntc_err_t SetAdcChannel(uint8_t adc_channel) noexcept;

  /**
   * @brief Set sampling parameters
   * @param sample_count Number of samples
   * @param sample_delay_ms Delay between samples (ms)
   * @return Error code
   */
  ntc_err_t SetSamplingParameters(uint32_t sample_count, uint32_t sample_delay_ms) noexcept;

  /**
   * @brief Enable/disable filtering
   * @param enable Enable filtering
   * @param alpha Filter alpha value (0.0-1.0)
   * @return Error code
   */
  ntc_err_t SetFiltering(bool enable, float alpha = 0.1f) noexcept;

  //==============================================================//
  // UTILITY FUNCTIONS
  //==============================================================//

  /**
   * @brief Convert Celsius to Fahrenheit
   * @param celsius Temperature in Celsius
   * @return Temperature in Fahrenheit
   */
  static float CelsiusToFahrenheit(float celsius) noexcept;

  /**
   * @brief Convert Fahrenheit to Celsius
   * @param fahrenheit Temperature in Fahrenheit
   * @return Temperature in Celsius
   */
  static float FahrenheitToCelsius(float fahrenheit) noexcept;

  /**
   * @brief Convert Celsius to Kelvin
   * @param celsius Temperature in Celsius
   * @return Temperature in Kelvin
   */
  static float CelsiusToKelvin(float celsius) noexcept;

  /**
   * @brief Convert Kelvin to Celsius
   * @param kelvin Temperature in Kelvin
   * @return Temperature in Celsius
   */
  static float KelvinToCelsius(float kelvin) noexcept;

  /**
   * @brief Get error string
   * @param error Error code
   * @return Error string
   */
  static const char* GetErrorString(ntc_err_t error) noexcept;

  /**
   * @brief Get NTC type string
   * @param type NTC type
   * @return Type string
   */
  static const char* GetTypeString(ntc_type_t type) noexcept;

private:
  //==============================================================//
  // PRIVATE MEMBER VARIABLES
  //==============================================================//

  ntc_config_t config_;    ///< NTC configuration
  AdcType* adc_interface_; ///< ADC interface pointer
  bool initialized_;       ///< Initialization status

  // Filtering
  float filtered_temperature_; ///< Filtered temperature
  bool filter_initialized_;    ///< Filter initialization status

  //==============================================================//
  // PRIVATE HELPER METHODS
  //==============================================================//

  /**
   * @brief Validate configuration
   * @param config Configuration to validate
   * @return Error code
   */
  ntc_err_t validateConfiguration(const ntc_config_t& config) const noexcept;

  /**
   * @brief Read ADC voltage
   * @param voltage_volts Pointer to store voltage
   * @return Error code
   */
  ntc_err_t readAdcVoltage(float* voltage_volts) noexcept;

  /**
   * @brief Calculate resistance from voltage
   * @param voltage_volts Voltage across thermistor
   * @param resistance_ohms Pointer to store resistance
   * @return Error code
   */
  ntc_err_t calculateResistance(float voltage_volts, float* resistance_ohms) noexcept;

  /**
   * @brief Convert resistance to temperature
   * @param resistance_ohms Thermistor resistance
   * @param temperature_celsius Pointer to store temperature
   * @return Error code
   */
  ntc_err_t convertResistanceToTemperature(float resistance_ohms,
                                           float* temperature_celsius) noexcept;

  /**
   * @brief Apply filtering
   * @param new_temperature New temperature reading
   * @return Filtered temperature
   */
  float applyFiltering(float new_temperature) noexcept;

  /**
   * @brief Initialize configuration for NTC type
   * @param ntc_type NTC type
   * @param config Pointer to configuration to initialize
   */
  static void initializeConfigForType(ntc_type_t ntc_type, ntc_config_t* config) noexcept;
};

// Include template implementation
#define NTC_THERMISTOR_HEADER_INCLUDED
#include "../src/NtcThermistor.cpp"
#undef NTC_THERMISTOR_HEADER_INCLUDED