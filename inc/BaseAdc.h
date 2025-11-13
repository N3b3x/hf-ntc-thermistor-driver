/**
 * @file BaseAdc.h
 * @brief Abstract base class for ADC interface used by NTC thermistor driver.
 *
 * This header defines the abstract interface that any ADC implementation must provide
 * to work with the NTC thermistor driver. Implementations for specific platforms
 * (e.g., ESP32-C6) should inherit from this class.
 *
 * @author HardFOC Development Team
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include <cstdint>

namespace NTC {

/**
 * @brief ADC error codes
 */
enum class AdcError : uint8_t {
  SUCCESS = 0,           ///< Operation successful
  NOT_INITIALIZED,       ///< ADC not initialized
  INVALID_CHANNEL,       ///< Invalid ADC channel
  READ_FAILED,           ///< ADC read operation failed
  TIMEOUT,               ///< Operation timeout
  HARDWARE_ERROR         ///< Hardware-level error
};

/**
 * @brief Abstract base class for ADC interface
 *
 * This class provides a hardware-agnostic interface for ADC operations.
 * Platform-specific implementations should inherit from this class and
 * implement all pure virtual methods.
 *
 * Example usage:
 * @code
 * class Esp32C6Adc : public NTC::BaseAdc {
 * public:
 *   bool IsInitialized() const override { ... }
 *   bool EnsureInitialized() override { ... }
 *   // ... implement other methods
 * };
 * @endcode
 */
class BaseAdc {
public:
  /**
   * @brief Virtual destructor
   */
  virtual ~BaseAdc() = default;

  /**
   * @brief Check if ADC is initialized
   * @return true if ADC is initialized, false otherwise
   */
  virtual bool IsInitialized() const = 0;

  /**
   * @brief Ensure ADC is initialized, initializing if necessary
   * @return true if ADC is initialized (or was successfully initialized), false otherwise
   */
  virtual bool EnsureInitialized() = 0;

  /**
   * @brief Check if a specific ADC channel is available
   * @param channel ADC channel number to check
   * @return true if channel is available, false otherwise
   */
  virtual bool IsChannelAvailable(uint8_t channel) const = 0;

  /**
   * @brief Read raw ADC count value from specified channel
   * @param channel ADC channel to read from
   * @param count Pointer to store the raw count value
   * @return AdcError::SUCCESS on success, error code otherwise
   */
  virtual AdcError ReadChannelCount(uint8_t channel, uint32_t* count) = 0;

  /**
   * @brief Read voltage value from specified channel
   * @param channel ADC channel to read from
   * @param voltage_v Pointer to store the voltage value in volts
   * @return AdcError::SUCCESS on success, error code otherwise
   */
  virtual AdcError ReadChannelV(uint8_t channel, float* voltage_v) = 0;

  /**
   * @brief Get the reference voltage of the ADC
   * @return Reference voltage in volts
   */
  virtual float GetReferenceVoltage() const = 0;

  /**
   * @brief Get the ADC resolution in bits
   * @return ADC resolution (e.g., 12 for 12-bit ADC)
   */
  virtual uint8_t GetResolutionBits() const = 0;

protected:
  /**
   * @brief Protected constructor to prevent direct instantiation
   */
  BaseAdc() = default;

  // Prevent copying
  BaseAdc(const BaseAdc&) = delete;
  BaseAdc& operator=(const BaseAdc&) = delete;

  // Allow moving
  BaseAdc(BaseAdc&&) = default;
  BaseAdc& operator=(BaseAdc&&) = default;
};

}  // namespace NTC
