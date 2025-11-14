/**
 * @file mock_esp32_adc.hpp
 * @brief Mock ADC implementation for ESP32-C6 testing
 *
 * This header provides a mock ADC implementation that simulates ADC readings
 * for testing the NTC thermistor driver without actual hardware.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#pragma once

#include "ntc_adc_interface.hpp"

#include <cmath>

/**
 * @brief Mock ADC implementation for testing
 *
 * This class provides a simulated ADC interface that can be used for testing
 * the NTC thermistor driver. It simulates realistic ADC readings based on
 * configurable parameters.
 *
 * Uses CRTP pattern with ntc::AdcInterface
 */
class MockEsp32Adc : public ntc::AdcInterface<MockEsp32Adc> {
public:
  /**
   * @brief Constructor
   * @param reference_voltage Reference voltage in volts (default: 3.3V for
   * ESP32-C6)
   * @param resolution_bits ADC resolution in bits (default: 12-bit for
   * ESP32-C6)
   */
  explicit MockEsp32Adc(float reference_voltage = 3.3F,
                        uint8_t resolution_bits = 12)
      : initialized_(false), reference_voltage_(reference_voltage),
        resolution_bits_(resolution_bits),
        max_count_((1U << resolution_bits) - 1) {}

  /**
   * @brief Destructor
   */
  ~MockEsp32Adc() = default;

  /**
   * @brief Check if ADC is initialized
   * @return true if initialized
   */
  bool IsInitialized() const { return initialized_; }

  /**
   * @brief Ensure ADC is initialized
   * @return true on success
   */
  bool EnsureInitialized() {
    if (!initialized_) {
      initialized_ = true;
    }
    return initialized_;
  }

  /**
   * @brief Check if channel is available
   * @param channel ADC channel (0-6 for ESP32-C6)
   * @return true if channel is valid
   */
  bool IsChannelAvailable(uint8_t channel) const {
    // ESP32-C6 has ADC1 with channels 0-6
    return channel <= 6;
  }

  /**
   * @brief Read raw ADC count
   * @param channel ADC channel to read
   * @param count Output parameter for count value
   * @return AdcError::Success on success
   */
  ntc::AdcError ReadChannelCount(uint8_t channel, uint32_t *count) {
    if (!initialized_) {
      return ntc::AdcError::NotInitialized;
    }

    if (!IsChannelAvailable(channel)) {
      return ntc::AdcError::InvalidChannel;
    }

    if (count == nullptr) {
      return ntc::AdcError::HardwareError;
    }

    // Simulate ADC reading - return mid-scale value with some variation
    // This simulates ~1.65V reading (mid-point)
    *count = max_count_ / 2 +
             (channel * 100); // Add some channel-dependent variation

    return ntc::AdcError::Success;
  }

  /**
   * @brief Read voltage from ADC channel
   * @param channel ADC channel to read
   * @param voltage_v Output parameter for voltage in volts
   * @return AdcError::Success on success
   */
  ntc::AdcError ReadChannelV(uint8_t channel, float *voltage_v) {
    if (!initialized_) {
      return ntc::AdcError::NotInitialized;
    }

    if (!IsChannelAvailable(channel)) {
      return ntc::AdcError::InvalidChannel;
    }

    if (voltage_v == nullptr) {
      return ntc::AdcError::HardwareError;
    }

    uint32_t count = 0;
    ntc::AdcError err = ReadChannelCount(channel, &count);
    if (err != ntc::AdcError::Success) {
      return err;
    }

    // Convert count to voltage
    *voltage_v = (static_cast<float>(count) * reference_voltage_) /
                 static_cast<float>(max_count_);

    return ntc::AdcError::Success;
  }

  /**
   * @brief Get reference voltage
   * @return Reference voltage in volts
   */
  float GetReferenceVoltage() const { return reference_voltage_; }

  /**
   * @brief Get ADC resolution
   * @return Resolution in bits
   */
  uint8_t GetResolutionBits() const { return resolution_bits_; }

  /**
   * @brief Set simulated voltage for testing
   * @param voltage Voltage to simulate (0 to reference_voltage)
   */
  void SetSimulatedVoltage(float voltage) {
    simulated_voltage_ = voltage;
    if (simulated_voltage_ < 0.0F) {
      simulated_voltage_ = 0.0F;
    }
    if (simulated_voltage_ > reference_voltage_) {
      simulated_voltage_ = reference_voltage_;
    }
  }

  /**
   * @brief Initialize the mock ADC
   * @return true on success
   */
  bool Initialize() {
    initialized_ = true;
    return true;
  }

private:
  bool initialized_;
  float reference_voltage_;
  uint8_t resolution_bits_;
  uint32_t max_count_;
  float simulated_voltage_ = 1.65F; // Default mid-scale voltage
};
