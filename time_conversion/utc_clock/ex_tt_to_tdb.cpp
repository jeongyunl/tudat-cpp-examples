#include "time_conversion.h"

// Demonstrate TT-to-TDB conversions using Tudat's time conversion utilities.

// For tudat::basic_astrodynamics::approximateConvertTTtoTDB()
#include <tudat/astro/basic_astro/timeConversions.h>

// For tudat::sofa_interface::convertTTtoTDB() and
// tudat::sofa_interface::convertTDBtoTT()
#include <tudat/interface/sofa/sofaTimeConversions.h>

// For tudat::earth_orientation::TerrestrialTimeScaleConverter class
#include <tudat/astro/earth_orientation/terrestrialTimeScaleConverter.h>

int main()
{
    // Get the current UTC time and print it.
    const auto utc_now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::utc_clock::now());
    std::cout << fmt::format("utc_now:\n\t{} UTC", utc_now) << '\n';

    // Convert the current UTC time to TT seconds relative to the J2000 epoch.
    const double tt_j2000_now = chrono_utc_time_to_tt_j2000(utc_now);
    std::cout << fmt::format("tt_j2000_now:\n\t{:.3f} TT", tt_j2000_now) << '\n';

    std::cout << '\n';

    // TT to TDB conversion using
    // tudat::basic_astrodynamics::approximateConvertTTtoTDB()
    {
        const double approx_tdb_j2000
            = tudat::basic_astrodynamics::approximateConvertTTtoTDB(tt_j2000_now);
        std::cout << fmt::format(
            "approx_tdb_j2000:\n\t{:.3f} TDB", approx_tdb_j2000)
                  << '\n';

        // tudat::basic_astrodynamics::approximateConvertTDBtoTT() is
        // NOT available
    }

    std::cout << '\n';

    // TT <-> TDB conversion using tudat::sofa_interface::convertTTtoTDB()
    // and tudat::sofa_interface::convertTDBtoTT()
    {
        const double sofa_tdb_j2000
            = tudat::sofa_interface::convertTTtoTDB<double>(tt_j2000_now, { });
        std::cout << fmt::format("sofa_tdb_j2000:\n\t{:.3f} TDB", sofa_tdb_j2000)
                  << '\n';

        const double sofa_tt_j2000
            = tudat::sofa_interface::convertTDBtoTT<double>(sofa_tdb_j2000, { });
        std::cout << fmt::format("sofa_tt_j2000:\n\t{:.3f} TT", sofa_tt_j2000)
                  << '\n';
    }

    std::cout << '\n';

    // TT <-> TDB conversion using
    // tudat::earth_orientation::TerrestrialTimeScaleConverter class
    {
        std::shared_ptr<tudat::earth_orientation::TerrestrialTimeScaleConverter>
            time_converter
            = tudat::earth_orientation::createDefaultTimeConverter();

        // getCurrentTime() converts a time value from the input to the output
        // scale
        const double converted_tdb_j2000 = time_converter->getCurrentTime(
            tudat::basic_astrodynamics::TimeScales::tt_scale, // inputScale
            tudat::basic_astrodynamics::TimeScales::tdb_scale, // outputScale
            tt_j2000_now, // inputTimeValue
            { } // earthFixedPosition
        );
        std::cout << fmt::format(
            "converted_tdb_j2000:\n\t{:.3f} TDB", converted_tdb_j2000)
                  << '\n';

        const double converted_tt_j2000 = time_converter->getCurrentTime(
            tudat::basic_astrodynamics::TimeScales::tdb_scale, // inputScale
            tudat::basic_astrodynamics::TimeScales::tt_scale, // outputScale
            converted_tdb_j2000, // inputTimeValue
            { } // earthFixedPosition
        );
        std::cout << fmt::format(
            "converted_tt_j2000:\n\t{:.3f} TT", converted_tt_j2000)
                  << '\n';
    }

    std::cout << '\n';

    return 0;
}