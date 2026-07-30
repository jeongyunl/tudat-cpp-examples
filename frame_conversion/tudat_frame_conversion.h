#pragma once

#include <tudat/astro/ephemerides/itrsToGcrsRotationModel.h>

/// @file tudat_frame_conversion.h
/// @brief Helpers for converting state vectors between ITRF and GCRF using Tudat
/// high accuracy rotation models.

using StateVector = std::array<double, 6>;

/// @brief Shared Earth rotation model used by the frame conversion helpers.
inline static std::shared_ptr<tudat::ephemerides::GcrsToItrsRotationModel>
    s_earth_rotation_model
    = std::make_shared<tudat::ephemerides::GcrsToItrsRotationModel>(
        tudat::earth_orientation::createStandardEarthOrientationCalculator(),
        tudat::basic_astrodynamics::tdb_scale, "GCRS");

/// @brief Convert a state vector from GCRF/GCRS to ITRF/ITRS.
/// @param tdb_time Time in TDB seconds relative to J2000.
/// @param input_gcrf_state State vector expressed in the GCRF frame.
/// @param output_itrf_state Output state vector expressed in the ITRF frame.
inline void TudatEciToEcef(double tdb_time, const StateVector& input_gcrf_state,
    StateVector& output_itrf_state)
{
    // Map std::array storage into Eigen vectors without copying.
    Eigen::Map<const Eigen::Vector6d> input_gcrf_state_v6d {
        input_gcrf_state.data()
    };
    Eigen::Map<Eigen::Vector6d> output_itrf_state_v6d {
        output_itrf_state.data()
    };

    const Eigen::Matrix3d gcrf_to_itrf_rotation_m3d
        = s_earth_rotation_model->getRotationToBaseFrame(tdb_time)
              .toRotationMatrix()
              .transpose();

    // Compute Earth's angular velocity expressed in the ITRF frame.
    // Tudat provides the angular velocity in the base frame; rotate it into the
    // target (ITRF) frame.
    const Eigen::Vector3d itrf_earth_rotational_velocity
        = gcrf_to_itrf_rotation_m3d
        * tudat::ephemerides::getRotationalVelocityVectorInBaseFrameFromMatrices(
            gcrf_to_itrf_rotation_m3d,
            s_earth_rotation_model->getDerivativeOfRotationToBaseFrame(tdb_time));

    // Split the 6D state into position and velocity components.
    const auto input_gcrf_position { input_gcrf_state_v6d.segment<3>(0) };
    const auto input_gcrf_velocity { input_gcrf_state_v6d.segment<3>(3) };

    auto output_itrf_position { output_itrf_state_v6d.segment<3>(0) };
    auto output_itrf_velocity { output_itrf_state_v6d.segment<3>(3) };

    // Position: rotate into ITRF.
    output_itrf_position = gcrf_to_itrf_rotation_m3d * input_gcrf_position;

    // Velocity: rotate into ITRF and apply the transport term due to Earth's
    // rotation: v_ITRF = R * v_GCRF - (omega_ITRF x r_ITRF)
    output_itrf_velocity = gcrf_to_itrf_rotation_m3d * input_gcrf_velocity
        - itrf_earth_rotational_velocity.cross(output_itrf_position);
}

/// @brief Convert a state vector from ITRF/ITRS to GCRF/GCRS.
/// @param tdb_time Time in TDB seconds relative to J2000.
/// @param input_itrf_state State vector expressed in the ITRF frame.
/// @param output_gcrf_state Output state vector expressed in the GCRF frame.
inline void TudatEcefToEci(double tdb_time, const StateVector& input_itrf_state,
    StateVector& output_gcrf_state)
{
    // Map std::array storage into Eigen vectors without copying.
    Eigen::Map<const Eigen::Vector6d> input_itrf_state_v6d {
        input_itrf_state.data()
    };
    Eigen::Map<Eigen::Vector6d> output_gcrf_state_v6d {
        output_gcrf_state.data()
    };

    const Eigen::Matrix3d itrf_to_gcrf_rotation_matrix
        = s_earth_rotation_model->getRotationToBaseFrame(tdb_time)
              .toRotationMatrix();

    // Compute Earth's angular velocity expressed in the GCRF frame.
    // Use the derivative of the rotation matrix to compute the angular velocity
    // vector.
    const Eigen::Vector3d gcrf_earth_rotational_velocity
        = tudat::ephemerides::getRotationalVelocityVectorInBaseFrameFromMatrices(
            itrf_to_gcrf_rotation_matrix.transpose(),
            s_earth_rotation_model->getDerivativeOfRotationToBaseFrame(tdb_time));

    // Split the 6D state into position and velocity components.
    const auto input_itrf_position { input_itrf_state_v6d.segment<3>(0) };
    const auto input_itrf_velocity { input_itrf_state_v6d.segment<3>(3) };

    auto output_gcrf_position { output_gcrf_state_v6d.segment<3>(0) };
    auto output_gcrf_velocity { output_gcrf_state_v6d.segment<3>(3) };

    // Position: rotate from ITRF to GCRF.
    output_gcrf_position = itrf_to_gcrf_rotation_matrix * input_itrf_position;

    // Velocity: rotate from ITRF to GCRF and apply the transport term due to
    // Earth's rotation: v_GCRF = R * v_ITRF + (omega_GCRF x r_GCRF)
    output_gcrf_velocity = itrf_to_gcrf_rotation_matrix * input_itrf_velocity
        + gcrf_earth_rotational_velocity.cross(output_gcrf_position);
}
