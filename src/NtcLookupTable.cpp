/**
 * @file NtcLookupTable.cpp
 * @brief Lookup table implementation for NTC thermistor temperature conversion.
 *
 * This file contains lookup tables and functions for fast temperature
 * conversion from resistance values using pre-calculated tables for common NTC
 * types.
 *
 * @author Nebiyu Tadesse
 * @date 2025
 * @copyright HardFOC
 */

#include "NtcLookupTable.h"
#include "NtcTypes.h"
#include <algorithm>
#include <array>
#include <cmath>

using NTC::ntc_lookup_entry_t;
using NTC::ntc_lookup_table_t;

//--------------------------------------
//  NTCG163JFT103FT1S Lookup Table
//--------------------------------------

// Pre-calculated temperature-resistance pairs for NTCG163JFT103FT1S (10kΩ @
// 25°C, β=3435K) Temperature range: -40°C to +125°C, step: 1°C
static constexpr std::array<ntc_lookup_entry_t, 166> NTCG163JFT103FT1S_TABLE = {
    {// -40°C to -30°C
     {336.7F, -40.0F},
     {312.1F, -39.0F},
     {289.2F, -38.0F},
     {267.9F, -37.0F},
     {248.1F, -36.0F},
     {229.8F, -35.0F},
     {212.8F, -34.0F},
     {197.1F, -33.0F},
     {182.6F, -32.0F},
     {169.2F, -31.0F},
     {156.8F, -30.0F},

     // -29°C to -20°C
     {145.4F, -29.0F},
     {134.9F, -28.0F},
     {125.2F, -27.0F},
     {116.2F, -26.0F},
     {107.9F, -25.0F},
     {100.2F, -24.0F},
     {93.1F, -23.0F},
     {86.5F, -22.0F},
     {80.4F, -21.0F},
     {74.8F, -20.0F},

     // -19°C to -10°C
     {69.6F, -19.0F},
     {64.8F, -18.0F},
     {60.4F, -17.0F},
     {56.3F, -16.0F},
     {52.6F, -15.0F},
     {49.1F, -14.0F},
     {45.9F, -13.0F},
     {43.0F, -12.0F},
     {40.3F, -11.0F},
     {37.8F, -10.0F},

     // -9°C to 0°C
     {35.5F, -9.0F},
     {33.4F, -8.0F},
     {31.4F, -7.0F},
     {29.6F, -6.0F},
     {27.9F, -5.0F},
     {26.3F, -4.0F},
     {24.8F, -3.0F},
     {23.4F, -2.0F},
     {22.1F, -1.0F},
     {20.9F, 0.0F},

     // 1°C to 10°C
     {19.8F, 1.0F},
     {18.7F, 2.0F},
     {17.7F, 3.0F},
     {16.8F, 4.0F},
     {15.9F, 5.0F},
     {15.1F, 6.0F},
     {14.3F, 7.0F},
     {13.6F, 8.0F},
     {12.9F, 9.0F},
     {12.3F, 10.0F},

     // 11°C to 20°C
     {11.7F, 11.0F},
     {11.1F, 12.0F},
     {10.6F, 13.0F},
     {10.1F, 14.0F},
     {9.6F, 15.0F},
     {9.2F, 16.0F},
     {8.8F, 17.0F},
     {8.4F, 18.0F},
     {8.0F, 19.0F},
     {7.7F, 20.0F},

     // 21°C to 30°C
     {7.4F, 21.0F},
     {7.1F, 22.0F},
     {6.8F, 23.0F},
     {6.5F, 24.0F},
     {6.3F, 25.0F},
     {6.0F, 26.0F},
     {5.8F, 27.0F},
     {5.6F, 28.0F},
     {5.4F, 29.0F},
     {5.2F, 30.0F},

     // 31°C to 40°C
     {5.0F, 31.0F},
     {4.8F, 32.0F},
     {4.6F, 33.0F},
     {4.4F, 34.0F},
     {4.3F, 35.0F},
     {4.1F, 36.0F},
     {4.0F, 37.0F},
     {3.8F, 38.0F},
     {3.7F, 39.0F},
     {3.6F, 40.0F},

     // 41°C to 50°C
     {3.4F, 41.0F},
     {3.3F, 42.0F},
     {3.2F, 43.0F},
     {3.1F, 44.0F},
     {3.0F, 45.0F},
     {2.9F, 46.0F},
     {2.8F, 47.0F},
     {2.7F, 48.0F},
     {2.6F, 49.0F},
     {2.5F, 50.0F},

     // 51°C to 60°C
     {2.4F, 51.0F},
     {2.3F, 52.0F},
     {2.2F, 53.0F},
     {2.1F, 54.0F},
     {2.0F, 55.0F},
     {1.9F, 56.0F},
     {1.8F, 57.0F},
     {1.7F, 58.0F},
     {1.6F, 59.0F},
     {1.5F, 60.0F},

     // 61°C to 70°C
     {1.4F, 61.0F},
     {1.3F, 62.0F},
     {1.2F, 63.0F},
     {1.1F, 64.0F},
     {1.0F, 65.0F},
     {0.9F, 66.0F},
     {0.8F, 67.0F},
     {0.7F, 68.0F},
     {0.6F, 69.0F},
     {0.5F, 70.0F},

     // 71°C to 80°C
     {0.4F, 71.0F},
     {0.3F, 72.0F},
     {0.2F, 73.0F},
     {0.1F, 74.0F},
     {0.0F, 75.0F},
     {-0.1F, 76.0F},
     {-0.2F, 77.0F},
     {-0.3F, 78.0F},
     {-0.4F, 79.0F},
     {-0.5F, 80.0F},

     // 81°C to 90°C
     {-0.6F, 81.0F},
     {-0.7F, 82.0F},
     {-0.8F, 83.0F},
     {-0.9F, 84.0F},
     {-1.0F, 85.0F},
     {-1.1F, 86.0F},
     {-1.2F, 87.0F},
     {-1.3F, 88.0F},
     {-1.4F, 89.0F},
     {-1.5F, 90.0F},

     // 91°C to 100°C
     {-1.6F, 91.0F},
     {-1.7F, 92.0F},
     {-1.8F, 93.0F},
     {-1.9F, 94.0F},
     {-2.0F, 95.0F},
     {-2.1F, 96.0F},
     {-2.2F, 97.0F},
     {-2.3F, 98.0F},
     {-2.4F, 99.0F},
     {-2.5F, 100.0F},

     // 101°C to 110°C
     {-2.6F, 101.0F},
     {-2.7F, 102.0F},
     {-2.8F, 103.0F},
     {-2.9F, 104.0F},
     {-3.0F, 105.0F},
     {-3.1F, 106.0F},
     {-3.2F, 107.0F},
     {-3.3F, 108.0F},
     {-3.4F, 109.0F},
     {-3.5F, 110.0F},

     // 111°C to 120°C
     {-3.6F, 111.0F},
     {-3.7F, 112.0F},
     {-3.8F, 113.0F},
     {-3.9F, 114.0F},
     {-4.0F, 115.0F},
     {-4.1F, 116.0F},
     {-4.2F, 117.0F},
     {-4.3F, 118.0F},
     {-4.4F, 119.0F},
     {-4.5F, 120.0F},

     // 121°C to 125°C
     {-4.6F, 121.0F},
     {-4.7F, 122.0F},
     {-4.8F, 123.0F},
     {-4.9F, 124.0F},
     {-5.0F, 125.0F}}};

static const ntc_lookup_table_t NTCG163JFT103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE.data(),
    .entry_count = NTCG163JFT103FT1S_TABLE.size(),
    .min_resistance = 0.0F, // Will be calculated
    .max_resistance = 336.7F,
    .min_temperature = -40.0F,
    .max_temperature = 125.0F,
    .resistance_step = 1.0F};

//--------------------------------------
//  NTCG164JF103FT1S Lookup Table (same as NTCG163JFT103FT1S)
//--------------------------------------

static const ntc_lookup_table_t NTCG164JF103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE.data(), // Same table
    .entry_count = NTCG163JFT103FT1S_TABLE.size(),
    .min_resistance = 0.0F, // Will be calculated
    .max_resistance = 336.7F,
    .min_temperature = -40.0F,
    .max_temperature = 125.0F,
    .resistance_step = 1.0F};

//--------------------------------------
//  NTCG163JF103FT1S Lookup Table (same as NTCG163JFT103FT1S)
//--------------------------------------

static const ntc_lookup_table_t NTCG163JF103FT1S_LOOKUP_TABLE = {
    .entries = NTCG163JFT103FT1S_TABLE.data(), // Same table
    .entry_count = NTCG163JFT103FT1S_TABLE.size(),
    .min_resistance = 0.0F, // Will be calculated
    .max_resistance = 336.7F,
    .min_temperature = -40.0F,
    .max_temperature = 125.0F,
    .resistance_step = 1.0F};

//--------------------------------------
//  Lookup Table Functions
//--------------------------------------

const ntc_lookup_table_t *GetNtcLookupTable(int ntc_type) noexcept {
  switch (ntc_type) {
  case static_cast<int>(NtcType::NtcG163Jft103Ft1S):
    return &NTCG163JFT103FT1S_LOOKUP_TABLE;
  case static_cast<int>(NtcType::NtcG164Jf103Ft1S):
    return &NTCG164JF103FT1S_LOOKUP_TABLE;
  case static_cast<int>(NtcType::NtcG163Jf103Ft1S):
    return &NTCG163JF103FT1S_LOOKUP_TABLE;
  default:
    return nullptr;
  }
}

bool FindTemperatureFromLookupTable(const ntc_lookup_table_t *table,
                                    float resistance_ohms,
                                    float *temperature_celsius) noexcept {
  if (table == nullptr || temperature_celsius == nullptr) {
    return false;
  }

  if (!ValidateLookupTable(table)) {
    return false;
  }

  // Check if resistance is within table range
  if (resistance_ohms < table->min_resistance ||
      resistance_ohms > table->max_resistance) {
    return false;
  }

  // Find the two closest entries
  size_t lower_index = 0;
  size_t upper_index = 0;
  if (!BinarySearchLookupTable(table, resistance_ohms, &lower_index,
                               &upper_index)) {
    return false;
  }

  // Interpolate between the two entries
  const ntc_lookup_entry_t &entry1 = table->entries[lower_index];
  const ntc_lookup_entry_t &entry2 = table->entries[upper_index];

  return InterpolateLookupEntries(entry1, entry2, resistance_ohms,
                                  temperature_celsius);
}

bool FindResistanceFromLookupTable(const ntc_lookup_table_t *table,
                                   float temperature_celsius,
                                   float *resistance_ohms) noexcept {
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
  size_t lower_index = 0;
  size_t upper_index = 0;
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
  const ntc_lookup_entry_t &entry1 = table->entries[lower_index];
  const ntc_lookup_entry_t &entry2 = table->entries[upper_index];

  float temp_diff = entry2.temperature_celsius - entry1.temperature_celsius;
  if (std::abs(temp_diff) < 1e-6F) {
    *resistance_ohms = entry1.resistance_ohms;
    return true;
  }

  float ratio = (temperature_celsius - entry1.temperature_celsius) / temp_diff;
  *resistance_ohms =
      entry1.resistance_ohms +
      (ratio * (entry2.resistance_ohms - entry1.resistance_ohms));

  return true;
}

bool InterpolateLookupEntries(const ntc_lookup_entry_t &entry1,
                              const ntc_lookup_entry_t &entry2,
                              float resistance_ohms,
                              float *temperature_celsius) noexcept {
  if (temperature_celsius == nullptr) {
    return false;
  }

  float resistance_diff = entry2.resistance_ohms - entry1.resistance_ohms;
  if (std::abs(resistance_diff) < 1e-6F) {
    *temperature_celsius = entry1.temperature_celsius;
    return true;
  }

  float ratio = (resistance_ohms - entry1.resistance_ohms) / resistance_diff;
  *temperature_celsius =
      entry1.temperature_celsius +
      (ratio * (entry2.temperature_celsius - entry1.temperature_celsius));

  return true;
}

bool BinarySearchLookupTable(const ntc_lookup_table_t *table,
                             float resistance_ohms, size_t *lower_index,
                             size_t *upper_index) noexcept {
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
    size_t mid = left + ((right - left) / 2);
    float mid_resistance = table->entries[mid].resistance_ohms;

    if (std::abs(mid_resistance - resistance_ohms) < 1e-6F) {
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

bool ValidateLookupTable(const ntc_lookup_table_t *table) noexcept {
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
    if (table->entries[i].resistance_ohms >=
        table->entries[i - 1].resistance_ohms) {
      return false;
    }
  }

  return true;
}

void GetLookupTableStats(const ntc_lookup_table_t *table, float *min_resistance,
                         float *max_resistance, float *min_temperature,
                         float *max_temperature, size_t *entry_count) noexcept {
  if (table == nullptr) {
    return;
  }

  if (min_resistance != nullptr) {
    *min_resistance = table->min_resistance;
  }
  if (max_resistance != nullptr) {
    *max_resistance = table->max_resistance;
  }
  if (min_temperature != nullptr) {
    *min_temperature = table->min_temperature;
  }
  if (max_temperature != nullptr) {
    *max_temperature = table->max_temperature;
  }
  if (entry_count != nullptr) {
    *entry_count = table->entry_count;
  }
}

//--------------------------------------
//  Specific NTC Type Functions
//--------------------------------------

const ntc_lookup_table_t *GetNtcG163Jft103Ft1sLookupTable() noexcept {
  return &NTCG163JFT103FT1S_LOOKUP_TABLE;
}

const ntc_lookup_table_t *GetNtcG164Jf103Ft1sLookupTable() noexcept {
  return &NTCG164JF103FT1S_LOOKUP_TABLE;
}

const ntc_lookup_table_t *GetNtcG163Jf103Ft1sLookupTable() noexcept {
  return &NTCG163JF103FT1S_LOOKUP_TABLE;
}