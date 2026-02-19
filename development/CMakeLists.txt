# =============================================================================
# hf-ntc-thermistor-driver — Root CMakeLists.txt
# =============================================================================
# Platform-agnostic build for the HardFOC NTC Thermistor driver.
# For ESP-IDF builds use the component wrapper instead.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

project(hf_ntc_thermistor
  VERSION   1.0.0
  LANGUAGES CXX
)

# ── Import shared build settings ─────────────────────────────────────────────
set(HF_NTC_THERMISTOR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH
    "Root of the NTC Thermistor driver repository")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_ntc_thermistor_build_settings.cmake")

# ── Library target ───────────────────────────────────────────────────────────
add_library(hf_ntc_thermistor STATIC ${HF_NTC_THERMISTOR_SOURCES})

target_include_directories(hf_ntc_thermistor PUBLIC ${HF_NTC_THERMISTOR_INCLUDE_DIRS})

target_compile_features(hf_ntc_thermistor PUBLIC cxx_std_20)

# ── Optional warnings (off by default, enable with -DHF_NTC_THERMISTOR_ENABLE_WARNINGS=ON)
option(HF_NTC_THERMISTOR_ENABLE_WARNINGS "Enable compiler warnings" OFF)
if(HF_NTC_THERMISTOR_ENABLE_WARNINGS)
  target_compile_options(hf_ntc_thermistor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── CMake package export ─────────────────────────────────────────────────────
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/hf_ntc_thermistorConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_ntc_thermistorConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_ntc_thermistorConfig.cmake"
  INSTALL_DESTINATION lib/cmake/hf_ntc_thermistor
)
