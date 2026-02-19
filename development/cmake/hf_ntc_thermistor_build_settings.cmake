# =============================================================================
# hf_ntc_thermistor_build_settings.cmake — Single source of truth
# =============================================================================
# This file defines ALL build settings for the NTC Thermistor driver.
# It is consumed by:
#   1. The root CMakeLists.txt   (desktop / non-IDF builds)
#   2. The ESP-IDF component wrapper (examples/esp32/components/…/CMakeLists.txt)
#
# Prerequisites:
#   HF_NTC_THERMISTOR_ROOT must be set to the driver's repository root before
#   including this file.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

# ── Guard ────────────────────────────────────────────────────────────────────
if(_HF_NTC_THERMISTOR_BUILD_SETTINGS_INCLUDED)
  return()
endif()
set(_HF_NTC_THERMISTOR_BUILD_SETTINGS_INCLUDED TRUE)

# ── Root validation ──────────────────────────────────────────────────────────
if(NOT DEFINED HF_NTC_THERMISTOR_ROOT)
  message(FATAL_ERROR "HF_NTC_THERMISTOR_ROOT must be set before including "
                      "hf_ntc_thermistor_build_settings.cmake")
endif()

# ── Version ──────────────────────────────────────────────────────────────────
set(HF_NTC_THERMISTOR_VERSION_MAJOR 1)
set(HF_NTC_THERMISTOR_VERSION_MINOR 0)
set(HF_NTC_THERMISTOR_VERSION_PATCH 0)
set(HF_NTC_THERMISTOR_VERSION_STRING
    "${HF_NTC_THERMISTOR_VERSION_MAJOR}.${HF_NTC_THERMISTOR_VERSION_MINOR}.${HF_NTC_THERMISTOR_VERSION_PATCH}")

# ── Generate version header ─────────────────────────────────────────────────
configure_file(
  "${HF_NTC_THERMISTOR_ROOT}/inc/ntc_thermistor_version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/ntc_thermistor_version.h"
  @ONLY
)

# ── Source files (compiled) ──────────────────────────────────────────────────
set(HF_NTC_THERMISTOR_SOURCES
    "${HF_NTC_THERMISTOR_ROOT}/src/ntc_conversion.cpp"
    "${HF_NTC_THERMISTOR_ROOT}/src/ntc_lookup_table.cpp"
)

# ── Public include directories ───────────────────────────────────────────────
set(HF_NTC_THERMISTOR_INCLUDE_DIRS
    "${HF_NTC_THERMISTOR_ROOT}/inc"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# ── ESP-IDF component dependencies ──────────────────────────────────────────
set(HF_NTC_THERMISTOR_IDF_REQUIRES
    driver
)

message(STATUS "[hf_ntc_thermistor] v${HF_NTC_THERMISTOR_VERSION_STRING} — "
               "${HF_NTC_THERMISTOR_ROOT}")
