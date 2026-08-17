#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>

/// @file time_conversion.h
/// @brief Utilities for converting between UTC, system time, and TT time relative
/// to the J2000 epoch.

/// @brief TAI-UTC offset (seconds) at J2000 (2000-01-01 11:58:55.816 UTC) epoch.
/// @details J2000 is a standard astronomical reference epoch widely used in
/// orbital mechanics. This offset is critical for converting between UTC and TAI
/// at the J2000 epoch. Value: 32 seconds.
constexpr auto TAI_MINUS_UTC_AT_J2000_S = 32L;

/// @brief TAI-UTC offset (seconds) at 1972-01-01 00:00:00.
/// @details Leap seconds were formally introduced in 1972 in UTC.
/// This constant represents the TAI-UTC offset at the epoch when leap second
/// corrections began. Value: 10 seconds.
constexpr auto TAI_MINUS_UTC_AT_1972_S = 10L;

// TT J2000 epoch: 2000-01-01 11:58:55.816 UTC
constexpr std::chrono::sys_time TT_J2000_EPOCH_IN_SYS_TIME
    = std::chrono::sys_days { std::chrono::year { 2000 } / std::chrono::January
          / 1 }
    + std::chrono::hours(11) + std::chrono::minutes(58) + std::chrono::seconds(55)
    + std::chrono::milliseconds(816);

// TT J2000 epoch: 2000-01-01 11:58:55.816 UTC
constexpr std::chrono::utc_time TT_J2000_EPOCH_IN_UTC_TIME
    = std::chrono::utc_time { TT_J2000_EPOCH_IN_SYS_TIME.time_since_epoch()
          + std::chrono::seconds(
              TAI_MINUS_UTC_AT_J2000_S - TAI_MINUS_UTC_AT_1972_S) };

/// @brief Convert a UTC time point to the TT time relative to the J2000 epoch.
/// @param utc_tp The UTC time point to convert.
/// @return The elapsed time in seconds between the provided UTC time and the
/// J2000 epoch.
inline double chrono_utc_time_to_tt_j2000(
    const std::chrono::utc_clock::time_point& utc_tp)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(
        utc_tp - TT_J2000_EPOCH_IN_UTC_TIME)
        .count();
}

/// @brief Convert a TT offset relative to J2000 into a UTC time point.
/// @param tt_j2000 The TT offset in seconds relative to the J2000 epoch.
/// @return The corresponding UTC time point.
inline std::chrono::utc_clock::time_point tt_j2000_to_chrono_utc_time(
    const double tt_j2000)
{
    const auto offset
        = std::chrono::duration_cast<std::chrono::utc_clock::duration>(
            std::chrono::duration<double>(tt_j2000));

    return TT_J2000_EPOCH_IN_UTC_TIME + offset;
}

/// @brief Convert a system time point to the TT time relative to the J2000 epoch.
/// @param sys_tp The system time point to convert.
/// @return The elapsed time in seconds between the provided system time and the
/// J2000 epoch.
inline double chrono_sys_time_to_tt_j2000(
    const std::chrono::system_clock::time_point& sys_tp)
{
    const auto utc_tp = std::chrono::utc_clock::from_sys(sys_tp);

    return chrono_utc_time_to_tt_j2000(utc_tp);
}

/// @brief Convert a TT offset relative to J2000 into a system time point.
/// @param tt_j2000 The TT offset in seconds relative to the J2000 epoch.
/// @return The corresponding system time point.
inline std::chrono::system_clock::time_point tt_j2000_to_chrono_sys_time(
    const double tt_j2000)
{
    const auto utc_tp = tt_j2000_to_chrono_utc_time(tt_j2000);

    return std::chrono::utc_clock::to_sys(utc_tp);
}

/// @brief Parse a UTC timestamp in ISO 8601 format into a system clock time
/// point.
/// @param iso8601_utc_time_str The UTC timestamp string in the format
/// YYYY-MM-DDTHH:MM:SS.
/// @return The corresponding system clock time point.
/// @note This function assumes that the input string is in UTC and does not
/// handle time zone offsets. It also does not account for leap seconds.
/// Also fractions of a second will be ignored. For example,
/// "2026-07-28T00:00:00.123" will be treated as "2026-07-28T00:00:00".
inline std::chrono::system_clock::time_point parse_iso8601_utc_time(
    const std::string& iso8601_utc_time_str)
{
    std::istringstream ss { iso8601_utc_time_str };

    std::tm tm_struct = { };
    ss >> std::get_time(&tm_struct, "%Y-%m-%dT%H:%M:%S");

    std::time_t posix_time = timegm(&tm_struct);
    return std::chrono::system_clock::from_time_t(posix_time);
}