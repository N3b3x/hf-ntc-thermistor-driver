/**
 * @file NtcThermistor.h
 * @brief Hardware-agnostic NTC thermistor driver for temperature measurement.
 *
 * This header provides a comprehensive interface for temperature measurement using
 * NTC (Negative Temperature Coefficient) thermistors. It supports various NTC types
 * and provides both lookup table and mathematical conversion methods.
 *
 * @author HardFOC Development Team
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cstdint>
#include <memory>

#include "BaseAdc.h"

//--------------------------------------
//  NTC Error Codes
//--------------------------------------

/**
 * @brief NTC thermistor error codes
 */
typedef enum {
  NTC_SUCCESS = 0,                      ///< Operation successful
  NTC_ERR_FAILURE = 1,                  ///< General failure
  NTC_ERR_NOT_INITIALIZED = 2,          ///< Not initialized
  NTC_ERR_ALREADY_INITIALIZED = 3,      ///< Already initialized
  NTC_ERR_INVALID_PARAMETER = 4,        ///< Invalid parameter
  NTC_ERR_NULL_POINTER = 5,             ///< Null pointer
  NTC_ERR_OUT_OF_MEMORY = 6,            ///< Out of memory
  NTC_ERR_ADC_READ_FAILED = 7,          ///< ADC read failed
  NTC_ERR_INVALID_RESISTANCE = 8,       ///< Invalid resistance value
  NTC_ERR_TEMPERATURE_OUT_OF_RANGE = 9, ///< Temperature out of range
  NTC_ERR_LOOKUP_TABLE_ERROR = 10,      ///< Lookup table error
  NTC_ERR_CONVERSION_FAILED = 11,       ///< Temperature conversion failed
  NTC_ERR_CALIBRATION_FAILED = 12,      ///< Calibration failed
  NTC_ERR_UNSUPPORTED_OPERATION = 13,   ///< Operation not supported
  NTC_ERR_TIMEOUT = 14,                 ///< Operation timeout
  NTC_ERR_HARDWARE_FAULT = 15,          ///< Hardware fault
  NTC_ERR_MAX = 16                      ///< Maximum error code
} ntc_err_t;

//--------------------------------------
//  NTC Types
//--------------------------------------

/**
 * @brief NTC thermistor types
 */
typedef enum {
  NTC_TYPE_UNKNOWN = 0,           ///< Unknown type
  NTC_TYPE_NTCG163JFT103FT1S = 1, ///< NTCG163JFT103FT1S (10kΩ @ 25°C, β=3435K)
  NTC_TYPE_NTCG164JF103FT1S = 2,  ///< NTCG164JF103FT1S (10kΩ @ 25°C, β=3435K)
  NTC_TYPE_NTCG163JF103FT1S = 3,  ///< NTCG163JF103FT1S (10kΩ @ 25°C, β=3435K)
  NTC_TYPE_CUSTOM = 4,            ///< Custom NTC type
  NTC_TYPE_MAX = 5                ///< Maximum type
} ntc_type_t;

/**
 * @brief Temperature conversion methods
 */
typedef enum {
  NTC_CONVERSION_LOOKUP_TABLE = 0, ///< Use lookup table (faster, less accurate)
  NTC_CONVERSION_MATHEMATICAL = 1, ///< Use mathematical conversion (slower, more accurate)
  NTC_CONVERSION_AUTO = 2          ///< Auto-select based on accuracy requirements
} ntc_conversion_method_t;

//--------------------------------------
//  NTC Configuration
//--------------------------------------

/**
 * @brief NTC thermistor configuration structure
 */
typedef struct {
  ntc_type_t type;                           ///< NTC type
  float resistance_at_25c;                   ///< Resistance at 25°C (ohms)
  float beta_value;                          ///< Beta value (K)
  float reference_voltage;                   ///< Reference voltage (V)
  float series_resistance;                   ///< Series resistance in voltage divider (ohms)
  float calibration_offset;                  ///< Calibration offset (°C)
  ntc_conversion_method_t conversion_method; ///< Conversion method
  uint8_t adc_channel;                       ///< ADC channel number
  uint32_t adc_resolution_bits;              ///< ADC resolution in bits
  uint32_t sample_count;                     ///< Number of samples to average
  uint32_t sample_delay_ms;                  ///< Delay between samples (ms)
  float min_temperature;                     ///< Minimum temperature (°C)
  float max_temperature;                     ///< Maximum temperature (°C)
  bool enable_filtering;                     ///< Enable temperature filtering
  float filter_alpha;                        ///< Filter alpha value (0.0-1.0)
} ntc_config_t;

/**
 * @brief NTC thermistor reading structure
 */
typedef struct {
  float temperature_celsius;    ///< Temperature in Celsius
  float temperature_fahrenheit; ///< Temperature in Fahrenheit
  float temperature_kelvin;     ///< Temperature in Kelvin
  float resistance_ohms;        ///< Thermistor resistance (ohms)
  float voltage_volts;          ///< Voltage across thermistor (V)
  uint32_t adc_raw_value;       ///< Raw ADC value
  uint64_t timestamp_us;        ///< Timestamp (microseconds)
  ntc_err_t error;              ///< Error code
  bool is_valid;                ///< Whether reading is valid
  float accuracy_celsius;       ///< Estimated accuracy (°C)
} ntc_reading_t;

//--------------------------------------
//  Default Configurations
//--------------------------------------

/**
 * @brief Default NTC configuration for NTCG163JFT103FT1S
 */
#define NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()                                                     \
  {.type = NTC_TYPE_NTCG163JFT103FT1S,                                                             \
   .resistance_at_25c = 10000.0f,                                                                  \
   .beta_value = 3435.0f,                                                                          \
   .reference_voltage = 3.3f,                                                                      \
   .series_resistance = 10000.0f,                                                                  \
   .calibration_offset = 0.0f,                                                                     \
   .conversion_method = NTC_CONVERSION_AUTO,                                                       \
   .adc_channel = 0,                                                                               \
   .adc_resolution_bits = 12,                                                                      \
   .sample_count = 1,                                                                              \
   .sample_delay_ms = 0,                                                                           \
   .min_temperature = -40.0f,                                                                      \
   .max_temperature = 125.0f,                                                                      \
   .enable_filtering = false,                                                                      \
   .filter_alpha = 0.1f}

/**
 * @brief Default NTC configuration
 */
#define NTC_CONFIG_DEFAULT() NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()

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
 * - Hardware-agnostic design using BaseAdc interface
 * - Support for multiple NTC types
 * - Dual conversion methods (lookup table and mathematical)
 * - Built-in calibration and filtering
 * - Comprehensive error handling
 *
 * @note The driver requires a BaseAdc interface for voltage measurements
 */
class NtcThermistor {
public:
  //==============================================================//
  // CONSTRUCTORS AND DESTRUCTOR
  //==============================================================//

  /**
   * @brief Constructor with NTC type and ADC interface
   * @param ntc_type NTC thermistor type
   * @param adc_interface Pointer to BaseAdc interface
   */
  NtcThermistor(ntc_type_t ntc_type, BaseAdc* adc_interface) noexcept;

  /**
   * @brief Constructor with custom configuration
   * @param config NTC configuration structure
   * @param adc_interface Pointer to BaseAdc interface
   */
  NtcThermistor(const ntc_config_t& config, BaseAdc* adc_interface) noexcept;

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
   * @param temperature_celsius Pointer to store temperature
   * @return Error code
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
  BaseAdc* adc_interface_; ///< ADC interface pointer
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
  ntc_err_t ValidateConfiguration(const ntc_config_t& config) const noexcept;

  /**
   * @brief Read ADC voltage
   * @param voltage_volts Pointer to store voltage
   * @return Error code
   */
  ntc_err_t ReadAdcVoltage(float* voltage_volts) noexcept;

  /**
   * @brief Calculate resistance from voltage
   * @param voltage_volts Voltage across thermistor
   * @param resistance_ohms Pointer to store resistance
   * @return Error code
   */
  ntc_err_t CalculateResistance(float voltage_volts, float* resistance_ohms) noexcept;

  /**
   * @brief Convert resistance to temperature
   * @param resistance_ohms Thermistor resistance
   * @param temperature_celsius Pointer to store temperature
   * @return Error code
   */
  ntc_err_t ConvertResistanceToTemperature(float resistance_ohms,
                                           float* temperature_celsius) noexcept;

  /**
   * @brief Apply filtering
   * @param new_temperature New temperature reading
   * @return Filtered temperature
   */
  float ApplyFiltering(float new_temperature) noexcept;

  /**
   * @brief Initialize configuration for NTC type
   * @param ntc_type NTC type
   * @param config Pointer to configuration to initialize
   */
  static void InitializeConfigForType(ntc_type_t ntc_type, ntc_config_t* config) noexcept;
};