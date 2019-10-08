add_executable(jackalope-test-1-thread thread.cxx)
target_link_libraries(jackalope-test-1-thread jackalope)
add_test(stage-1-thread jackalope-test-1-thread)

add_executable(jackalope-test-1-log_dest log_dest.cxx)
target_link_libraries(jackalope-test-1-log_dest jackalope)
add_test(stage-1-log_dest jackalope-test-1-log_dest)
