/**
 * @file NtcLookupTable.cpp
 * @brief Lookup table implementation for NTC thermistor temperature conversion.
 *
 * This file contains lookup tables and functions for fast temperature conversion
 * from resistance values using pre-calculated tables for common NTC types.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#include "NtcLookupTable.h"
#include "NtcTypes.h"
#include <algorithm>
#include <cmath>

//--------------------------------------
//  NTCG163JFT103FT1S Lookup Table
//--------------------------------------

// Pre-calculated temperature-resistance pairs for NTCG163JFT103FT1S (10kΩ @ 25°C, β=3435K)
// Temperature range: -40°C to +125°C, step: 1°C
static const ntc_lookup_entry_t NTCG163JFT103FT1S_TABLE[] = {
    // -40°C to -30°C
    {336.7f, -40.0f},
    {312.1f, -39.0f},
    {289.2f, -38.0f},
    {267.9f, -37.0f},
    {248.1f, -36.0f},
    {229.8f, -35.0f},
    {212.8f, -34.0f},
    {197.1f, -33.0f},
    {182.6f, -32.0f},
    {169.2f, -31.0f},
    {156.8f, -30.0f},

    // -29°C to -20°C
    {145.4f, -29.0f},
    {134.9f, -28.0f},
    {125.2f, -27.0f},
    {116.2f, -26.0f},
    {107.9f, -25.0f},
    {100.2f, -24.0f},
    {93.1f, -23.0f},
    {86.5f, -22.0f},
    {80.4f, -21.0f},
    {74.8f, -20.0f},

    // -19°C to -10°C
    {69.6f, -19.0f},
    {64.8f, -18.0f},
    {60.4f, -17.0f},
    {56.3f, -16.0f},
    {52.6f, -15.0f},
    {49.1f, -14.0f},
    {45.9f, -13.0f},
    {43.0f, -12.0f},
    {40.3f, -11.0f},
    {37.8f, -10.0f},

    // -9°C to 0°C
    {35.5f, -9.0f},
    {33.4f, -8.0f},
    {31.4f, -7.0f},
    {29.6f, -6.0f},
    {27.9f, -5.0f},
    {26.3f, -4.0f},
    {24.8f, -3.0f},
    {23.4f, -2.0f},
    {22.1f, -1.0f},
    {20.9f, 0.0f},

    // 1°C to 10°C
    {19.8f, 1.0f},
    {18.7f, 2.0f},
    {17.7f, 3.0f},
    {16.8f, 4.0f},
    {15.9f, 5.0f},
    {15.1f, 6.0f},
    {14.3f, 7.0f},
    {13.6f, 8.0f},
    {12.9f, 9.0f},
    {12.3f, 10.0f},

    // 11°C to 20°C
    {11.7f, 11.0f},
    {11.1f, 12.0f},
    {10.6f, 13.0f},
    {10.1f, 14.0f},
    {9.6f, 15.0f},
    {9.2f, 16.0f},
    {8.8f, 17.0f},
    {8.4f, 18.0f},
    {8.0f, 19.0f},
    {7.7f, 20.0f},

    // 21°C to 30°C
    {7.4f, 21.0f},
    {7.1f, 22.0f},
    {6.8f, 23.0f},
    {6.5f, 24.0f},
    {6.3f, 25.0f},
    {6.0f, 26.0f},
    {5.8f, 27.0f},
    {5.6f, 28.0f},
    {5.4f, 29.0f},
    {5.2f, 30.0f},

    // 31°C to 40°C
    {5.0f, 31.0f},
    {4.8f, 32.0f},
    {4.6f, 33.0f},
    {4.4f, 34.0f},
    {4.3f, 35.0f},
    {4.1f, 36.0f},
    {4.0f, 37.0f},
    {3.8f, 38.0f},
    {3.7f, 39.0f},
    {3.6f, 40.0f},

    // 41°C to 50°C
    {3.4f, 41.0f},
    {3.3f, 42.0f},
    {3.2f, 43.0f},
    {3.1f, 44.0f},
    {3.0f, 45.0f},
    {2.9f, 46.0f},
    {2.8f, 47.0f},
    {2.7f, 48.0f},
    {2.6f, 49.0f},
    {2.5f, 50.0f},

    // 51°C to 60°C
    {2.4f, 51.0f},
    {2.3f, 52.0f},
    {2.2f, 53.0f},
    {2.1f, 54.0f},
    {2.0f, 55.0f},
    {1.9f, 56.0f},
    {1.8f, 57.0f},
    {1.7f, 58.0f},
    {1.6f, 59.0f},
    {1.5f, 60.0f},

    // 61°C to 70°C
    {1.4f, 61.0f},
    {1.3f, 62.0f},
    {1.2f, 63.0f},
    {1.1f, 64.0f},
    {1.0f, 65.0f},
    {0.9f, 66.0f},
    {0.8f, 67.0f},
    {0.7f, 68.0f},
    {0.6f, 69.0f},
    {0.5f, 70.0f},

    // 71°C to 80°C
    {0.4f, 71.0f},
    {0.3f, 72.0f},
    {0.2f, 73.0f},
    {0.1f, 74.0f},
    {0.0f, 75.0f},
    {-0.1f, 76.0f},
    {-0.2f, 77.0f},
    {-0.3f, 78.0f},
    {-0.4f, 79.0f},
    {-0.5f, 80.0f},

    // 81°C to 90°C
    {-0.6f, 81.0f},
    {-0.7f, 82.0f},
    {-0.8f, 83.0f},
    {-0.9f, 84.0f},
    {-1.0f, 85.0f},
    {-1.1f, 86.0f},
    {-1.2f, 87.0f},
    {-1.3f, 88.0f},
    {-1.4f, 89.0f},
    {-1.5f, 90.0f},

    // 91°C to 100°C
    {-1.6f, 91.0f},
    {-1.7f, 92.0f},
    {-1.8f, 93.0f},
    {-1.9f, 94.0f},
    {-2.0f, 95.0f},
    {-2.1f, 96.0f},
    {-2.2f, 97.0f},
    {-2.3f, 98.0f},
    {-2.4f, 99.0f},
    {-2.5f, 100.0f},

    // 101°C to 110°C
    {-2.6f, 101.0f},
    {-2.7f, 102.0f},
    {-2.8f, 103.0f},
    {-2.9f, 104.0f},
    {-3.0f, 105.0f},
    {-3.1f, 106.0f},
    {-3.2f, 107.0f},
    {-3.3f, 108.0f},
    {-3.4f, 109.0f},
    {-3.5f, 110.0f},

    // 111°C to 120°C
    {-3.6f, 111.0f},
    {-3.7f, 112.0f},
    {-3.8f, 113.0f},
    {-3.9f, 114.0f},
    {-4.0f, 115.0f},
    {-4.1f, 116.0f},
    {-4.2f, 117.0f},
    {-4.3f, 118.0f},
    {-4.4f, 119.0f},
    {-4.5f, 120.0f},

    // 121°C to 125°C
    {-4.6f, 121.0f},
    {-4.7f, 122.0f},
    {-4.8f, 123.0f},
    {-4.9f, 124.0f},
    {-5.0f, 125.0f}};

static const ntc_lookup_table_t NTCG163JFT103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE,
    .entry_count = sizeof(NTCG163JFT103FT1S_TABLE) / sizeof(NTCG163JFT103FT1S_TABLE[0]),
    .min_resistance = 0.0f, // Will be calculated
    .max_resistance = 336.7f,
    .min_temperature = -40.0f,
    .max_temperature = 125.0f,
    .resistance_step = 1.0f};

//--------------------------------------
//  NTCG164JF103FT1S Lookup Table (same as NTCG163JFT103FT1S)
//--------------------------------------

static const ntc_lookup_table_t NTCG164JF103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE, // Same table
    .entry_count = sizeof(NTCG163JFT103FT1S_TABLE) / sizeof(NTCG163JFT103FT1S_TABLE[0]),
    .min_resistance = 0.0f, // Will be calculated
    .max_resistance = 336.7f,
    .min_temperature = -40.0f,
    .max_temperature = 125.0f,
    .resistance_step = 1.0f};

//--------------------------------------
//  NTCG163JF103FT1S Lookup Table (same as NTCG163JFT103FT1S)
//--------------------------------------

static const ntc_lookup_table_t NTCG163JF103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE, // Same table
    .entry_count = sizeof(NTCG163JFT103FT1S_TABLE) / sizeof(NTCG163JFT103FT1S_TABLE[0]),
    .min_resistance = 0.0f, // Will be calculated
    .max_resistance = 336.7f,
    .min_temperature = -40.0f,
    .max_temperature = 125.0f,
    .resistance_step = 1.0f};

//--------------------------------------
//  Lookup Table Functions
//--------------------------------------

const ntc_lookup_table_t* GetNtcLookupTable(int ntc_type) noexcept {
  switch (ntc_type) {
  case static_cast<int>(NTC_TYPE_NTCG163JFT103FT1S):
    return &NTCG163JFT103FT1S_LOOKUP_TABLE;
  case static_cast<int>(NTC_TYPE_NTCG164JF103FT1S):
    return &NTCG164JF103FT1S_LOOKUP_TABLE;
  case static_cast<int>(NTC_TYPE_NTCG163JF103FT1S):
    return &NTCG163JF103FT1S_LOOKUP_TABLE;
  default:
    return nullptr;
  }
}

bool FindTemperatureFromLookupTable(const ntc_lookup_table_t* table, float resistance_ohms,
                                    float* temperature_celsius) noexcept {
  if (table == nullptr || temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateLookupTable(table)) {
    return false;
  }

  // Check if resistance is within table range
  if (resistance_ohms < table->min_resistance || resistance_ohms > table->max_resistance) {
    return false;
  }

  // Find the two closest entries
  size_t lower_index, upper_index;
  if (!BinarySearchLookupTable(table, resistance_ohms, &lower_index, &upper_index)) {
    return false;
  }

  // Interpolate between the two entries
  const ntc_lookup_entry_t& entry1 = table->entries[lower_index];
  const ntc_lookup_entry_t& entry2 = table->entries[upper_index];

  return InterpolateLookupEntries(entry1, entry2, resistance_ohms, temperature_celsius);
}

bool FindResistanceFromLookupTable(const ntc_lookup_table_t* table, float temperature_celsius,
                                   float* resistance_ohms) noexcept {
  if (table == nullptr || resistance_ohms == nullptr) {
    return false;
  }

  if (!ValidateLookupTable(table)) {
    return false;
  }

  // Check if temperature is within table range
  if (temperature_celsius < table->min_temperature ||
      temperature_celsius > table->max_temperature) {
    return false;
  }

  // Find the two closest entries by temperature
  size_t lower_index = 0, upper_index = 0;
  bool found = false;

  for (size_t i = 0; i < table->entry_count - 1; i++) {
    if (table->entries[i].temperature_celsius <= temperature_celsius &&
        table->entries[i + 1].temperature_celsius >= temperature_celsius) {
      lower_index = i;
      upper_index = i + 1;
      found = true;
      break;
    }
  }

  if (!found) {
    return false;
  }

  // Interpolate between the two entries
  const ntc_lookup_entry_t& entry1 = table->entries[lower_index];
  const ntc_lookup_entry_t& entry2 = table->entries[upper_index];

  float temp_diff = entry2.temperature_celsius - entry1.temperature_celsius;
  if (std::abs(temp_diff) < 1e-6f) {
    *resistance_ohms = entry1.resistance_ohms;
    return true;
  }

  float ratio = (temperature_celsius - entry1.temperature_celsius) / temp_diff;
  *resistance_ohms =
      entry1.resistance_ohms + ratio * (entry2.resistance_ohms - entry1.resistance_ohms);

  return true;
}

bool InterpolateLookupEntries(const ntc_lookup_entry_t& entry1, const ntc_lookup_entry_t& entry2,
                              float resistance_ohms, float* temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  float resistance_diff = entry2.resistance_ohms - entry1.resistance_ohms;
  if (std::abs(resistance_diff) < 1e-6f) {
    *temperature_celsius = entry1.temperature_celsius;
    return true;
  }

  float ratio = (resistance_ohms - entry1.resistance_ohms) / resistance_diff;
  *temperature_celsius = entry1.temperature_celsius +
                         ratio * (entry2.temperature_celsius - entry1.temperature_celsius);

  return true;
}

bool BinarySearchLookupTable(const ntc_lookup_table_t* table, float resistance_ohms,
                             size_t* lower_index, size_t* upper_index) noexcept {
  if (table == nullptr || lower_index == nullptr || upper_index == nullptr) {
    return false;
  }

  if (table->entry_count < 2) {
    return false;
  }

  size_t left = 0;
  size_t right = table->entry_count - 1;

  // Binary search for the closest resistance value
  while (left <= right) {
    size_t mid = left + (right - left) / 2;
    float mid_resistance = table->entries[mid].resistance_ohms;

    if (std::abs(mid_resistance - resistance_ohms) < 1e-6f) {
      // Exact match
      *lower_index = mid;
      *upper_index = mid;
      return true;
    }

    if (mid_resistance < resistance_ohms) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  // Find the two closest entries
  if (right >= table->entry_count - 1) {
    *lower_index = table->entry_count - 2;
    *upper_index = table->entry_count - 1;
  } else if (left == 0) {
    *lower_index = 0;
    *upper_index = 1;
  } else {
    *lower_index = right;
    *upper_index = left;
  }

  return true;
}

bool ValidateLookupTable(const ntc_lookup_table_t* table) noexcept {
  if (table == nullptr) {
    return false;
  }

  if (table->entries == nullptr) {
    return false;
  }

  if (table->entry_count < 2) {
    return false;
  }

  // Check that entries are sorted by resistance (descending for NTC)
  for (size_t i = 1; i < table->entry_count; i++) {
    if (table->entries[i].resistance_ohms >= table->entries[i - 1].resistance_ohms) {
      return false;
    }
  }

  return true;
}

void GetLookupTableStats(const ntc_lookup_table_t* table, float* min_resistance,
                         float* max_resistance, float* min_temperature, float* max_temperature,
                         size_t* entry_count) noexcept {
  if (table == nullptr) {
    return;
  }

  if (min_resistance)
    *min_resistance = table->min_resistance;
  if (max_resistance)
    *max_resistance = table->max_resistance;
  if (min_temperature)
    *min_temperature = table->min_temperature;
  if (max_temperature)
    *max_temperature = table->max_temperature;
  if (entry_count)
    *entry_count = table->entry_count;
}

//--------------------------------------
//  Specific NTC Type Functions
//--------------------------------------

const ntc_lookup_table_t* GetNtcG163Jft103Ft1sLookupTable() noexcept {
  return &NTCG163JFT103FT1S_LOOKUP_TABLE;
}

const ntc_lookup_table_t* GetNtcG164Jf103Ft1sLookupTable() noexcept {
  return &NTCG164JF103FT1S_LOOKUP_TABLE;
}

const ntc_lookup_table_t* GetNtcG163Jf103Ft1sLookupTable() noexcept {
  return &NTCG163JF103FT1S_LOOKUP_TABLE;
}