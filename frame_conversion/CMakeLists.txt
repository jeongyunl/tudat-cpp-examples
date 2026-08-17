if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13")
	    add_subdirectory(utc_clock)
    else()
        add_subdirectory(no_utc_clock)
    endif()
endif()

