#include <chrono>
#include <format>
#include <iostream>

// Demonstrate conversion between a UTC time point and the TT J2000 epoch.
int main()
{
    // Define the J2000 epoch in system time and convert it to UTC.
    // TT J2000 epoch: 2000-01-01 11:58:55.816 UTC
    const std::chrono::sys_time TT_J2000_EPOCH_IN_SYS_TIME
        = std::chrono::sys_days { std::chrono::year { 2000 }
              / std::chrono::January / 1 }
        + std::chrono::hours(11) + std::chrono::minutes(58)
        + std::chrono::seconds(55) + std::chrono::milliseconds(816);

    const std::chrono::utc_time TT_J2000_EPOCH_IN_UTC_TIME
        = std::chrono::utc_clock::from_sys(TT_J2000_EPOCH_IN_SYS_TIME);

    std::cout << std::format("tt j2000 epoch: {} UTC", TT_J2000_EPOCH_IN_UTC_TIME)
              << '\n';

    // Capture the current UTC time and print it.
    const auto utc_now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::utc_clock::now());

    std::cout << std::format("utc_now: {} UTC", utc_now) << '\n';

    // Compute the elapsed time in seconds from the J2000 epoch.
    const double seconds_since_tt_j2000
        = std::chrono::duration_cast<std::chrono::duration<double>>(
            utc_now - TT_J2000_EPOCH_IN_UTC_TIME)
              .count();

    std::cout << std::format(
        "seconds_since_tt_j2000: {:.3f}", seconds_since_tt_j2000)
              << '\n';

    // Reconstruct a UTC time point from the elapsed seconds.
    const auto utc_time_from_tt = TT_J2000_EPOCH_IN_UTC_TIME
        + std::chrono::duration<double>(seconds_since_tt_j2000);
    std::cout << std::format("utc_time_from_tt: {} UTC", utc_time_from_tt)
              << '\n';

    return 0;
}
