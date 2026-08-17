# Tudat examples

This repository contains CMake-based example projects demonstrating time-conversion, frame-conversion, and propagation workflows with Tudat, plus a comprehensive time converter utility.

## Repository structure and file overview

- [CMakeLists.txt](CMakeLists.txt) — top-level CMake configuration for the project.
- [time_conversion/](time_conversion/) — source code and build configuration for the time-conversion examples.
  - [time_conversion/utc_clock/](time_conversion/utc_clock/) — UTC clock-based time conversion examples.
    - [time_conversion/utc_clock/CMakeLists.txt](time_conversion/utc_clock/CMakeLists.txt) — defines the example executables.
    - [time_conversion/utc_clock/time_conversion.h](time_conversion/utc_clock/time_conversion.h) — shared header with conversion constants and inline helpers for UTC, system time, and TT time relative to the J2000 epoch.
    - [time_conversion/utc_clock/ex_chrono_to_tt.cpp](time_conversion/utc_clock/ex_chrono_to_tt.cpp) — simple example showing UTC time and TT/J2000 conversions, including the J2000 epoch setup and elapsed-time calculation.
    - [time_conversion/utc_clock/ex_tt_to_tdb.cpp](time_conversion/utc_clock/ex_tt_to_tdb.cpp) — example that compares TT-to-TDB conversion approaches using Tudat APIs and prints the results for comparison.
  - [time_conversion/CMakeLists.txt](time_conversion/CMakeLists.txt) — top-level time conversion build configuration.
- [frame_conversion/](frame_conversion/) — examples for converting between Earth-fixed and inertial reference frames.
  - [frame_conversion/spice_frame_conversion.h](frame_conversion/spice_frame_conversion.h) — small helpers for SPICE-based frame conversions.
  - [frame_conversion/tudat_frame_conversion.h](frame_conversion/tudat_frame_conversion.h) — helpers for high-accuracy Tudat-based frame conversions using Earth orientation models.
  - [frame_conversion/utc_clock/](frame_conversion/utc_clock/) — UTC clock-based frame conversion examples.
    - [frame_conversion/utc_clock/CMakeLists.txt](frame_conversion/utc_clock/CMakeLists.txt) — defines the frame conversion example executables.
    - [frame_conversion/utc_clock/ex_spice_frame_conversion.cpp](frame_conversion/utc_clock/ex_spice_frame_conversion.cpp) — SPICE-based example that converts reference states between ECI and ECEF.
    - [frame_conversion/utc_clock/ex_tudat_frame_conversion.cpp](frame_conversion/utc_clock/ex_tudat_frame_conversion.cpp) — Tudat-based example that converts reference states between GCRF and ITRF using a higher-accuracy Earth orientation model.
  - [frame_conversion/no_utc_clock/](frame_conversion/no_utc_clock/) — non-UTC clock frame conversion examples.
    - [frame_conversion/no_utc_clock/CMakeLists.txt](frame_conversion/no_utc_clock/CMakeLists.txt) — defines the frame conversion example executables.
    - [frame_conversion/no_utc_clock/ex_spice_frame_conversion.cpp](frame_conversion/no_utc_clock/ex_spice_frame_conversion.cpp) — SPICE-based example without UTC clock.
    - [frame_conversion/no_utc_clock/ex_tudat_frame_conversion.cpp](frame_conversion/no_utc_clock/ex_tudat_frame_conversion.cpp) — Tudat-based example without UTC clock.
  - [frame_conversion/CMakeLists.txt](frame_conversion/CMakeLists.txt) — top-level frame conversion build configuration.
- [propagation/](propagation/) — examples for configuring and propagating a low-Earth orbit around Earth.
  - [propagation/CMakeLists.txt](propagation/CMakeLists.txt) — defines the propagation executable.
  - [propagation/propagator_full.cpp](propagation/propagator_full.cpp) — full Tudat propagation example that loads SPICE kernels, configures Earth, Sun, Moon, and a 3U CubeSat, propagates the orbit, and writes propagated and interpolated states to CSV files.
  - [propagation/GCRF_57392_LEO_3.oem](propagation/GCRF_57392_LEO_3.oem) — reference LEO state history in the GCRF frame.
  - [propagation/ITRF_57392_LEO_3.oem](propagation/ITRF_57392_LEO_3.oem) — reference LEO state history in the ITRF frame.
- [time_converter/](time_converter/) — comprehensive time conversion utility with multiple backends.
  - [time_converter/base/](time_converter/base/) — base time converter implementation.
    - [time_converter/base/time_converter_base.h](time_converter/base/time_converter_base.h) — base time converter interface.
    - [time_converter/base/time_converter_base.cpp](time_converter/base/time_converter_base.cpp) — base implementation.
    - [time_converter/base/iso8601.cpp](time_converter/base/iso8601.cpp) — ISO 8601 parsing.
    - [time_converter/base/base_dispatch_table.cpp](time_converter/base/base_dispatch_table.cpp) — dispatch table for base conversions.
    - [time_converter/base/zoneinfo_leap_transition.cpp](time_converter/base/zoneinfo_leap_transition.cpp) — leap second handling.
  - [time_converter/chrono/](time_converter/chrono/) — C++ chrono-based time converter.
    - [time_converter/chrono/time_converter_chrono.h](time_converter/chrono/time_converter_chrono.h) — chrono converter interface.
    - [time_converter/chrono/time_converter_chrono.cpp](time_converter/chrono/time_converter_chrono.cpp) — chrono implementation.
    - [time_converter/chrono/chrono_dispatch_table.cpp](time_converter/chrono/chrono_dispatch_table.cpp) — dispatch table for chrono conversions.
  - [time_converter/tudat/](time_converter/tudat/) — Tudat-based time converter.
    - [time_converter/tudat/time_converter_tudat.h](time_converter/tudat/time_converter_tudat.h) — Tudat converter interface.
    - [time_converter/tudat/time_converter_tudat.cpp](time_converter/tudat/time_converter_tudat.cpp) — Tudat implementation.
    - [time_converter/tudat/tudat_dispatch_table.cpp](time_converter/tudat/tudat_dispatch_table.cpp) — dispatch table for Tudat conversions.
  - [time_converter/test/](time_converter/test/) — comprehensive test suite with Google Test.
  - [time_converter/old/](time_converter/old/) — legacy test scripts and examples.
  - [time_converter/CMakeLists.txt](time_converter/CMakeLists.txt) — time converter build configuration.
  - [time_converter/time_converter.h](time_converter/time_converter.h) — main time converter interface.
  - [time_converter/time_converter.cpp](time_converter/time_converter.cpp) — main implementation.
  - [time_converter/time_converter_cli.cpp](time_converter/time_converter_cli.cpp) — command-line interface.
  - [time_converter/conversion_wrapper.h](time_converter/conversion_wrapper.h) — conversion wrapper utilities.
  - [time_converter/time_conversion_common.h](time_converter/time_conversion_common.h) — common time conversion definitions.
  - [time_converter/time_conversion_epochs.h](time_converter/time_conversion_epochs.h) — epoch definitions.
  - [time_converter/time_converter.py](time_converter/time_converter.py) — Python wrapper.

## Build notes

The project uses CMake with Tudat and Eigen as dependencies. Configure the project with CMake, then build the example targets from the generated build directory. The propagation example writes its CSV output files to the process working directory. The time_converter utility provides a CLI tool and library for converting between various time scales (UTC, TAI, TT, TDB, GPS, etc.).
