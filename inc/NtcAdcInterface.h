/**
 * @file NtcAdcInterface.h
 * @brief CRTP-based template interface for ADC used by NTC thermistor driver.
 *
 * This header defines a CRTP (Curiously Recurring Template Pattern) interface
 * that any ADC implementation must provide to work with the NTC thermistor driver.
 * This approach provides compile-time polymorphism without virtual function overhead.
 *
 * @author Nebiyu Tadesse
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
  SUCCESS = 0,     ///< Operation successful
  NOT_INITIALIZED, ///< ADC not initialized
  INVALID_CHANNEL, ///< Invalid ADC channel
  READ_FAILED,     ///< ADC read operation failed
  TIMEOUT,         ///< Operation timeout
  HARDWARE_ERROR   ///< Hardware-level error
};

/**
 * @brief CRTP-based template interface for ADC operations
 *
 * This template class provides a hardware-agnostic interface for ADC operations
 * using the CRTP pattern. Platform-specific implementations should inherit from
 * this template with themselves as the template parameter.
 *
 * Benefits of CRTP:
 * - Compile-time polymorphism (no virtual function overhead)
 * - Static dispatch instead of dynamic dispatch
 * - Better optimization opportunities for the compiler
 *
 * Example usage:
 * @code
 * class Esp32C6Adc : public NTC::NtcAdcInterface<Esp32C6Adc> {
 * public:
 *   bool IsInitialized() const { ... }
 *   bool EnsureInitialized() { ... }
 *   // ... implement other methods
 * };
 * @endcode
 *
 * @tparam Derived The derived class type (CRTP pattern)
 */
template <typename Derived>
class NtcAdcInterface {
public:
  /**
   * @brief Check if ADC is initialized
   * @return true if ADC is initialized, false otherwise
   */
  bool IsInitialized() const {
    return static_cast<const Derived*>(this)->IsInitialized();
  }

  /**
   * @brief Ensure ADC is initialized, initializing if necessary
   * @return true if ADC is initialized (or was successfully initialized), false otherwise
   */
  bool EnsureInitialized() {
    return static_cast<Derived*>(this)->EnsureInitialized();
  }

  /**
   * @brief Check if a specific ADC channel is available
   * @param channel ADC channel number to check
   * @return true if channel is available, false otherwise
   */
  bool IsChannelAvailable(uint8_t channel) const {
    return static_cast<const Derived*>(this)->IsChannelAvailable(channel);
  }

  /**
   * @brief Read raw ADC count value from specified channel
   * @param channel ADC channel to read from
   * @param count Pointer to store the raw count value
   * @return AdcError::SUCCESS on success, error code otherwise
   */
  AdcError ReadChannelCount(uint8_t channel, uint32_t* count) {
    return static_cast<Derived*>(this)->ReadChannelCount(channel, count);
  }

  /**
   * @brief Read voltage value from specified channel
   * @param channel ADC channel to read from
   * @param voltage_v Pointer to store the voltage value in volts
   * @return AdcError::SUCCESS on success, error code otherwise
   */
  AdcError ReadChannelV(uint8_t channel, float* voltage_v) {
    return static_cast<Derived*>(this)->ReadChannelV(channel, voltage_v);
  }

  /**
   * @brief Get the reference voltage of the ADC
   * @return Reference voltage in volts
   */
  float GetReferenceVoltage() const {
    return static_cast<const Derived*>(this)->GetReferenceVoltage();
  }

  /**
   * @brief Get the ADC resolution in bits
   * @return ADC resolution (e.g., 12 for 12-bit ADC)
   */
  uint8_t GetResolutionBits() const {
    return static_cast<const Derived*>(this)->GetResolutionBits();
  }

protected:
  /**
   * @brief Protected constructor to prevent direct instantiation
   */
  NtcAdcInterface() = default;

  // Prevent copying
  NtcAdcInterface(const NtcAdcInterface&) = delete;
  NtcAdcInterface& operator=(const NtcAdcInterface&) = delete;

  // Allow moving
  NtcAdcInterface(NtcAdcInterface&&) = default;
  NtcAdcInterface& operator=(NtcAdcInterface&&) = default;

  /**
   * @brief Protected destructor
   * @note Derived classes can have public destructors
   */
  ~NtcAdcInterface() = default;
};

} // namespace NTC
