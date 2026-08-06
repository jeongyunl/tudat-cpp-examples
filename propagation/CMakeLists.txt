add_executable(propagator_full propagator_full.cpp)
target_link_libraries(propagator_full PUBLIC ${Tudat_PROPAGATION_LIBRARIES})

