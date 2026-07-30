#pragma once

#include <tudat/interface/spice/spiceInterface.h>

/// @file spice_frame_conversion.h
/// @brief Small helpers for converting state vectors between ECEF and ECI frames
/// using SPICE.

using StateVector = std::array<double, 6>;

/// @brief Convert a state vector from ECEF/ITRF93 to ECI/J2000 using a SPICE
/// rotation matrix.
/// @param tdb_time The input time in TDB seconds relative to J2000.
/// @param input_state The state vector expressed in the source frame.
/// @param output_state The converted state vector expressed in the target frame.
inline void SpiceEcefToEci(
    double tdb_time, const StateVector& input_state, StateVector& output_state)
{
    Eigen::Matrix6d spice_rotation_m6d
        = tudat::spice_interface::computeStateRotationMatrixBetweenFrames(
            "ITRF93", "J2000", tdb_time);

    Eigen::Map<const Eigen::Vector6d> input_state_v6d { input_state.data() };

    Eigen::Map<Eigen::Vector6d> output_state_v6d { output_state.data() };

    output_state_v6d = spice_rotation_m6d * input_state_v6d;
}

/// @brief Convert a state vector from ECI/J2000 to ECEF/ITRF93 using a SPICE
/// rotation matrix.
/// @param tdb_time The input time in TDB seconds relative to J2000.
/// @param input_state The state vector expressed in the source frame.
/// @param output_state The converted state vector expressed in the target frame.
inline void SpiceEciToEcef(
    double tdb_time, const StateVector& input_state, StateVector& output_state)
{
    Eigen::Matrix6d spice_rotation_m6d
        = tudat::spice_interface::computeStateRotationMatrixBetweenFrames(
            "J2000", "ITRF93", tdb_time);

    Eigen::Map<const Eigen::Vector6d> input_state_v6d { input_state.data() };

    Eigen::Map<Eigen::Vector6d> output_state_v6d { output_state.data() };

    output_state_v6d = spice_rotation_m6d * input_state_v6d;
}
