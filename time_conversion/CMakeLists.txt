add_executable(ex_chrono_to_tt ex_chrono_to_tt.cpp)
add_executable(ex_tt_to_tdb ex_tt_to_tdb.cpp)
target_link_libraries(ex_tt_to_tdb PUBLIC ${Tudat_PROPAGATION_LIBRARIES})

