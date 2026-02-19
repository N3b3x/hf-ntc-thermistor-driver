# =============================================================================
# hf_ntc_thermistor — ESP-IDF component wrapper
# =============================================================================
# This thin wrapper imports the shared build-settings file and registers the
# component for ESP-IDF.  All sources, includes, and version info are defined
# in cmake/hf_ntc_thermistor_build_settings.cmake (single source of truth).
# =============================================================================

# ── Locate driver root ───────────────────────────────────────────────────────
set(HF_NTC_THERMISTOR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." CACHE PATH
    "Root of the hf-ntc-thermistor-driver repository")

if(NOT EXISTS "${HF_NTC_THERMISTOR_ROOT}/cmake/hf_ntc_thermistor_build_settings.cmake")
  message(FATAL_ERROR
    "Cannot find hf_ntc_thermistor_build_settings.cmake.\n"
    "  HF_NTC_THERMISTOR_ROOT = ${HF_NTC_THERMISTOR_ROOT}")
endif()

# ── Import build settings ────────────────────────────────────────────────────
include("${HF_NTC_THERMISTOR_ROOT}/cmake/hf_ntc_thermistor_build_settings.cmake")

# ── Register ESP-IDF component ───────────────────────────────────────────────
idf_component_register(
    SRCS        ${HF_NTC_THERMISTOR_SOURCES}
    INCLUDE_DIRS ${HF_NTC_THERMISTOR_INCLUDE_DIRS}
    REQUIRES     ${HF_NTC_THERMISTOR_IDF_REQUIRES}
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
