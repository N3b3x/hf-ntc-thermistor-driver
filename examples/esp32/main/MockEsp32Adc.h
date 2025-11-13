/**
 * @file MockEsp32Adc.h
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

#include "NtcAdcInterface.h"

#include <cmath>

/**
 * @brief Mock ADC implementation for testing
 *
 * This class provides a simulated ADC interface that can be used for testing
 * the NTC thermistor driver. It simulates realistic ADC readings based on
 * configurable parameters.
 *
 * Uses CRTP pattern with NTC::NtcAdcInterface
 */
class MockEsp32Adc : public NTC::NtcAdcInterface<MockEsp32Adc> {
public:
  /**
   * @brief Constructor
   * @param reference_voltage Reference voltage in volts (default: 3.3V for ESP32-C6)
   * @param resolution_bits ADC resolution in bits (default: 12-bit for ESP32-C6)
   */
  explicit MockEsp32Adc(float reference_voltage = 3.3f, uint8_t resolution_bits = 12)
      : initialized_(false), reference_voltage_(reference_voltage),
        resolution_bits_(resolution_bits), max_count_((1U << resolution_bits) - 1) {}

  /**
   * @brief Destructor
   */
  ~MockEsp32Adc() = default;

  /**
   * @brief Check if ADC is initialized
   * @return true if initialized
   */
  bool IsInitialized() const {
    return initialized_;
  }

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
   * @return AdcError::SUCCESS on success
   */
  NTC::AdcError ReadChannelCount(uint8_t channel, uint32_t* count) {
    if (!initialized_) {
      return NTC::AdcError::NOT_INITIALIZED;
    }

    if (!IsChannelAvailable(channel)) {
      return NTC::AdcError::INVALID_CHANNEL;
    }

    if (count == nullptr) {
      return NTC::AdcError::HARDWARE_ERROR;
    }

    // Simulate ADC reading - return mid-scale value with some variation
    // This simulates ~1.65V reading (mid-point)
    *count = max_count_ / 2 + (channel * 100); // Add some channel-dependent variation

    return NTC::AdcError::SUCCESS;
  }

  /**
   * @brief Read voltage from ADC channel
   * @param channel ADC channel to read
   * @param voltage_v Output parameter for voltage in volts
   * @return AdcError::SUCCESS on success
   */
  NTC::AdcError ReadChannelV(uint8_t channel, float* voltage_v) {
    if (!initialized_) {
      return NTC::AdcError::NOT_INITIALIZED;
    }

    if (!IsChannelAvailable(channel)) {
      return NTC::AdcError::INVALID_CHANNEL;
    }

    if (voltage_v == nullptr) {
      return NTC::AdcError::HARDWARE_ERROR;
    }

    uint32_t count = 0;
    NTC::AdcError err = ReadChannelCount(channel, &count);
    if (err != NTC::AdcError::SUCCESS) {
      return err;
    }

    // Convert count to voltage
    *voltage_v = (static_cast<float>(count) * reference_voltage_) / static_cast<float>(max_count_);

    return NTC::AdcError::SUCCESS;
  }

  /**
   * @brief Get reference voltage
   * @return Reference voltage in volts
   */
  float GetReferenceVoltage() const {
    return reference_voltage_;
  }

  /**
   * @brief Get ADC resolution
   * @return Resolution in bits
   */
  uint8_t GetResolutionBits() const {
    return resolution_bits_;
  }

  /**
   * @brief Set simulated voltage for testing
   * @param voltage Voltage to simulate (0 to reference_voltage)
   */
  void SetSimulatedVoltage(float voltage) {
    simulated_voltage_ = voltage;
    if (simulated_voltage_ < 0.0f) {
      simulated_voltage_ = 0.0f;
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
  float simulated_voltage_ = 1.65f; // Default mid-scale voltage
};
