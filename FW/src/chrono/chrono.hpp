#pragma once

#define PLATFORM_ATSAM

#if defined(PLATFORM_RPI)
    #include <chrono>
    namespace nd::chrono
    {
        using steady_clock = std::chrono::steady_clock;
    }
#elif defined(PLATFORM_ATSAM)
    #include "chrono_atsam.hpp"
#elif defined(PLATFORM_NRF9160)
    #include "chrono_nrf9160.hpp"
#else
    #error "nd::chrono does not have implementation for this system"
#endif

namespace nd::chrono::durations
{
    using nanoseconds	= std::chrono::duration<steady_clock::rep, std::nano>;
    using microseconds	= std::chrono::duration<steady_clock::rep, std::micro>;
    using milliseconds	= std::chrono::duration<steady_clock::rep, std::milli>;
    using seconds	    = std::chrono::duration<steady_clock::rep>;
    using minutes	    = std::chrono::duration<steady_clock::rep, std::ratio< 60>>;
    using hours		    = std::chrono::duration<steady_clock::rep, std::ratio<3600>>;
    using days		    = std::chrono::duration<steady_clock::rep, std::ratio<86400>>;
    using weeks		    = std::chrono::duration<steady_clock::rep, std::ratio<604800>>;
    using years		    = std::chrono::duration<steady_clock::rep, std::ratio<31556952>>;
    using months	    = std::chrono::duration<steady_clock::rep, std::ratio<2629746>>;
}
