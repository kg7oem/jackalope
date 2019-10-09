add_executable(jackalope-test-1-thread thread.cxx)
target_link_libraries(jackalope-test-1-thread jackalope)
add_test(stage-1-thread jackalope-test-1-thread)

add_executable(jackalope-test-1-log.dest log.dest.cxx)
target_link_libraries(jackalope-test-1-log.dest jackalope)
add_test(stage-1-dest jackalope-test-1-log.dest)

add_executable(jackalope-test-1-component component.cxx)
target_link_libraries(jackalope-test-1-component jackalope)
add_test(stage-1-component jackalope-test-1-component)
