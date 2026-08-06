# Tudat examples

This repository contains a small CMake-based example project that demonstrates time-conversion workflows with Tudat.

## Repository structure and file overview

- [CMakeLists.txt](CMakeLists.txt) — top-level CMake configuration for the project.
- [time_conversion/](time_conversion/) — source code and build configuration for the time-conversion examples.
  - [time_conversion/CMakeLists.txt](time_conversion/CMakeLists.txt) — defines the example executables.
  - [time_conversion/time_conversion.h](time_conversion/time_conversion.h) — shared header with conversion constants and inline helpers for UTC, system time, and TT time relative to the J2000 epoch.
  - [time_conversion/ex_chrono_to_tt.cpp](time_conversion/ex_chrono_to_tt.cpp) — simple example showing UTC time and TT/J2000 conversions, including the J2000 epoch setup and elapsed-time calculation.
  - [time_conversion/ex_tt_to_tdb.cpp](time_conversion/ex_tt_to_tdb.cpp) — example that compares TT-to-TDB conversion approaches using Tudat APIs and prints the results for comparison.
- [frame_conversion/](frame_conversion/) — examples for converting between Earth-fixed and inertial reference frames.
  - [frame_conversion/CMakeLists.txt](frame_conversion/CMakeLists.txt) — defines the frame conversion example executables.
  - [frame_conversion/spice_frame_conversion.h](frame_conversion/spice_frame_conversion.h) — small helpers for SPICE-based frame conversions.
  - [frame_conversion/tudat_frame_conversion.h](frame_conversion/tudat_frame_conversion.h) — helpers for high-accuracy Tudat-based frame conversions using Earth orientation models.
  - [frame_conversion/ex_spice_frame_conversion.cpp](frame_conversion/ex_spice_frame_conversion.cpp) — SPICE-based example that converts reference states between ECI and ECEF.
  - [frame_conversion/ex_tudat_frame_conversion.cpp](frame_conversion/ex_tudat_frame_conversion.cpp) — Tudat-based example that converts reference states between GCRF and ITRF using a higher-accuracy Earth orientation model.
- [propagation/](propagation/) — examples for configuring and propagating a low-Earth orbit around Earth.
  - [propagation/CMakeLists.txt](propagation/CMakeLists.txt) — defines the propagation executable.
  - [propagation/propagator_full.cpp](propagation/propagator_full.cpp) — full Tudat propagation example that loads SPICE kernels, configures Earth, Sun, Moon, and a 3U CubeSat, propagates the orbit, and writes propagated and interpolated states to CSV files.
  - [propagation/GCRF_57392_LEO_3.oem](propagation/GCRF_57392_LEO_3.oem) — reference LEO state history in the GCRF frame.
  - [propagation/ITRF_57392_LEO_3.oem](propagation/ITRF_57392_LEO_3.oem) — reference LEO state history in the ITRF frame.

## Build notes

The project uses CMake with Tudat and Eigen as dependencies. Configure the project with CMake, then build the example targets from the generated build directory. The propagation example writes its CSV output files to the process working directory.
