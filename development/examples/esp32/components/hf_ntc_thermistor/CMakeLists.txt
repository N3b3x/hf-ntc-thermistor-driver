# =============================================================================
# hf_ntc_thermistor — ESP-IDF component wrapper
# =============================================================================
# This thin wrapper imports the shared build-settings file and registers the
# component for ESP-IDF.  All sources, includes, and version info are defined
# in cmake/hf_ntc_thermistor_build_settings.cmake (single source of truth).
# =============================================================================

# ── Locate driver root ───────────────────────────────────────────────────────
set(HF_NTC_THERMISTOR_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." CACHE PATH
    "Path to root of hf-ntc-thermistor-driver (auto-detected from component location)")

get_filename_component(HF_NTC_THERMISTOR_ROOT "${HF_NTC_THERMISTOR_ROOT}" ABSOLUTE)
if(NOT EXISTS "${HF_NTC_THERMISTOR_ROOT}/cmake/hf_ntc_thermistor_build_settings.cmake")
    message(FATAL_ERROR
        "Cannot find NTC Thermistor driver root.\n"
        "  Resolved path: ${HF_NTC_THERMISTOR_ROOT}\n"
        "  Expected file: ${HF_NTC_THERMISTOR_ROOT}/cmake/hf_ntc_thermistor_build_settings.cmake\n"
        "  Fix: set HF_NTC_THERMISTOR_ROOT to the correct driver checkout path.")
endif()

# ── Import build settings ────────────────────────────────────────────────────
include("${HF_NTC_THERMISTOR_ROOT}/cmake/hf_ntc_thermistor_build_settings.cmake")

# ── Register ESP-IDF component ───────────────────────────────────────────────
idf_component_register(
    SRCS         ${HF_NTC_THERMISTOR_SOURCE_FILES}
    INCLUDE_DIRS ${HF_NTC_THERMISTOR_PUBLIC_INCLUDE_DIRS}
    REQUIRES     ${HF_NTC_THERMISTOR_IDF_REQUIRES}
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
