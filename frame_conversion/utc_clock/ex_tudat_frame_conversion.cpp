#include "../time_conversion/time_conversion.h"
#include "tudat_frame_conversion.h"
#include <tudat/io/basicInputOutput.h>

int main()
{
    // clang-format off
    //
    // Example ECI (ICRF) state vector (units: km, km/s)
    // 2026-07-28T00:00:00.000000 1419.84672 -2042.744523 6957.335603 -6.405321072 2.862751161 2.147772202
    //
    // ECEF (ITRF) state vector converted from ECI (ICRF) state vector
    // 2026-07-28T00:00:00.000000 2477.320766 -38.026605 6960.94278 -6.045886636 -3.754887744 2.131197452
    //
    // clang-format on

    // Parse the example UTC timestamp and convert it to a TT/J2000 value.
    const auto time_str = "2026-07-28T00:00:00";

    const auto sys_tp = parse_iso8601_utc_time(time_str);
    std::cout << fmt::format("time_str: {} UTC", time_str) << '\n';
    std::cout << fmt::format("sys_tp:   {:%FT%H:%M:%OS} UTC", sys_tp) << '\n';
    const auto tt_j2000 = chrono_sys_time_to_tt_j2000(sys_tp);

    // Define the reference ECI and ECEF state vectors used for the conversion
    // check.
    const auto ref_eci_state = StateVector {
        1419.84672 * 1000, // x meters
        -2042.744523 * 1000, // y meters
        6957.335603 * 1000, // z meters
        -6.405321072 * 1000, // vx meters per second
        2.862751161 * 1000, // vy meters per second
        2.147772202 * 1000 // vz meters per second
    };

    const auto ref_ecef_state = StateVector {
        2477.320766 * 1000, // x meters
        -38.026605 * 1000, // y meters
        6960.94278 * 1000, // z meters
        -6.045886636 * 1000, // vx meters per second
        -3.754887744 * 1000, // vy meters per second
        2.131197452 * 1000 // vz meters per second
    };

    std::cout << fmt::format(
        "ref_eci_state:\n\t{:.6f} {:.6f} {:.6f} {:.6f} {:.6f} {:.6f}",
        ref_eci_state[0], ref_eci_state[1], ref_eci_state[2], ref_eci_state[3],
        ref_eci_state[4], ref_eci_state[5])
              << '\n';

    std::cout << fmt::format(
        "ref_ecef_state:\n\t{:.6f} {:.6f} {:.6f} {:.6f} {:.6f} {:.6f}",
        ref_ecef_state[0], ref_ecef_state[1], ref_ecef_state[2],
        ref_ecef_state[3], ref_ecef_state[4], ref_ecef_state[5])
              << '\n';

    {
        StateVector ecef_state;

        TudatEciToEcef(tt_j2000, ref_eci_state, ecef_state);

        std::cout << fmt::format("Converted ref_eci_state to ECEF:\n\t{:.6f} "
                                 "{:.6f} {:.6f} {:.6f} {:.6f} {:.6f}",
            ecef_state[0], ecef_state[1], ecef_state[2], ecef_state[3],
            ecef_state[4], ecef_state[5])
                  << '\n';
    }

    {
        StateVector eci_state;

        TudatEcefToEci(tt_j2000, ref_ecef_state, eci_state);

        std::cout << fmt::format("Converted ref_ecef_state to ECI:\n\t{:.6f} "
                                 "{:.6f} {:.6f} {:.6f} {:.6f} {:.6f}",
            eci_state[0], eci_state[1], eci_state[2], eci_state[3], eci_state[4],
            eci_state[5])
                  << '\n';
    }

    return 0;
}