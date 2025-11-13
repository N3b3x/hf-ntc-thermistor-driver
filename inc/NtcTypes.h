/**
 * @file NtcTypes.h
 * @brief Type definitions for NTC thermistor driver.
 *
 * This header contains all type definitions, enumerations, and structures
 * used by the NTC thermistor driver. This file is included by other modules
 * that need type definitions without requiring the full NtcThermistor class.
 *
 * @note This header is designed to minimize dependencies and can be included
 *       independently of the main NtcThermistor class for type-only usage.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cstdint>

//--------------------------------------
//  NTC Error Codes
//--------------------------------------

/**
 * @brief NTC thermistor error codes
 *
 * Error codes returned by NTC thermistor driver functions. All functions
 * that can fail return an ntc_err_t value indicating success or the specific
 * error condition.
 *
 * @note Always check return values for error conditions before using results.
 *
 * @see NtcThermistor::GetErrorString() for human-readable error messages
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
 *
 * Supported NTC thermistor types. Each type has predefined characteristics
 * (resistance at 25°C, beta value) that are automatically configured
 * when using the type-specific constructor.
 *
 * @note Use NTC_TYPE_CUSTOM for thermistors not in this list, and configure
 *       resistance_at_25c and beta_value manually.
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
 *
 * Methods for converting resistance to temperature. The driver supports
 * two conversion approaches with different trade-offs:
 *
 * - **Lookup Table**: Fast, pre-calculated values, slightly less accurate
 * - **Mathematical**: Slower, uses Steinhart-Hart or Beta equations, more accurate
 * - **Auto**: Automatically selects the best method based on configuration
 *
 * @note For real-time applications requiring high update rates, use
 *       NTC_CONVERSION_LOOKUP_TABLE. For maximum accuracy, use
 *       NTC_CONVERSION_MATHEMATICAL.
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
 *
 * Complete configuration for an NTC thermistor instance. All parameters
 * must be set appropriately for accurate temperature readings.
 *
 * @note Use NTC_CONFIG_DEFAULT() or NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()
 *       macros to initialize with sensible defaults, then modify as needed.
 *
 * @warning Invalid configuration values (e.g., negative resistance) will
 *          cause initialization or reading functions to fail.
 *
 * @see NtcThermistor::SetConfiguration()
 * @see NtcThermistor::GetConfiguration()
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
 *
 * Complete temperature reading information including all temperature units,
 * raw ADC values, resistance, voltage, and error status.
 *
 * @note Always check the is_valid and error fields before using temperature
 *       values. Invalid readings may contain NaN or invalid data.
 *
 * @see NtcThermistor::ReadTemperature()
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
 * @brief Default NTC configuration constants
 */
namespace NTC::DefaultConfig {
constexpr float kDefaultResistanceAt25C = 10000.0f;  ///< Default resistance at 25°C (10kΩ)
constexpr float kDefaultBetaValue = 3435.0f;         ///< Default beta value (3435K)
constexpr float kDefaultReferenceVoltage = 3.3f;     ///< Default reference voltage (3.3V)
constexpr float kDefaultSeriesResistance = 10000.0f; ///< Default series resistance (10kΩ)
constexpr float kDefaultCalibrationOffset = 0.0f;    ///< Default calibration offset
constexpr uint8_t kDefaultAdcChannel = 0;            ///< Default ADC channel
constexpr uint32_t kDefaultAdcResolutionBits = 12U;  ///< Default ADC resolution (12-bit)
constexpr uint32_t kDefaultSampleCount = 1U;         ///< Default sample count
constexpr uint32_t kDefaultSampleDelayMs = 0U;       ///< Default sample delay (ms)
constexpr float kDefaultMinTemperature = -40.0f;     ///< Default minimum temperature (°C)
constexpr float kDefaultMaxTemperature = 125.0f;     ///< Default maximum temperature (°C)
constexpr bool kDefaultEnableFiltering = false;      ///< Default filtering enabled
constexpr float kDefaultFilterAlpha = 0.1f;          ///< Default filter alpha value
} // namespace NTC::DefaultConfig

/**
 * @brief Default NTC configuration for NTCG163JFT103FT1S
 *
 * Initializes a configuration structure with default values for the
 * NTCG163JFT103FT1S thermistor (10kΩ @ 25°C, β=3435K).
 *
 * @note This macro uses constexpr constants from NTC::DefaultConfig namespace
 *
 * @example
 * @code
 * ntc_config_t config = NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S();
 * config.adc_channel = 1;  // Override default channel
 * NtcThermistor<MyAdc> thermistor(config, &my_adc);
 * @endcode
 */
#define NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()                                                     \
  {.type = NTC_TYPE_NTCG163JFT103FT1S,                                                             \
   .resistance_at_25c = NTC::DefaultConfig::kDefaultResistanceAt25C,                               \
   .beta_value = NTC::DefaultConfig::kDefaultBetaValue,                                            \
   .reference_voltage = NTC::DefaultConfig::kDefaultReferenceVoltage,                              \
   .series_resistance = NTC::DefaultConfig::kDefaultSeriesResistance,                              \
   .calibration_offset = NTC::DefaultConfig::kDefaultCalibrationOffset,                            \
   .conversion_method = NTC_CONVERSION_AUTO,                                                       \
   .adc_channel = NTC::DefaultConfig::kDefaultAdcChannel,                                          \
   .adc_resolution_bits = NTC::DefaultConfig::kDefaultAdcResolutionBits,                           \
   .sample_count = NTC::DefaultConfig::kDefaultSampleCount,                                        \
   .sample_delay_ms = NTC::DefaultConfig::kDefaultSampleDelayMs,                                   \
   .min_temperature = NTC::DefaultConfig::kDefaultMinTemperature,                                  \
   .max_temperature = NTC::DefaultConfig::kDefaultMaxTemperature,                                  \
   .enable_filtering = NTC::DefaultConfig::kDefaultEnableFiltering,                                \
   .filter_alpha = NTC::DefaultConfig::kDefaultFilterAlpha}

/**
 * @brief Default NTC configuration
 *
 * Convenience macro that defaults to NTCG163JFT103FT1S configuration.
 * Equivalent to NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S().
 *
 * @see NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()
 */
#define NTC_CONFIG_DEFAULT() NTC_CONFIG_DEFAULT_NTCG163JFT103FT1S()
