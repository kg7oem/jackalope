include_directories(SYSTEM ${DBUS_CXX_INCLUDE_DIRS})

add_executable(jackalope-test-1-thread thread.cxx)
target_link_libraries(jackalope-test-1-thread ${JACKALOPE_LIB_TARGET})
add_test(stage-1-thread jackalope-test-1-thread)

add_executable(jackalope-test-1-log.dest log.dest.cxx)
target_link_libraries(jackalope-test-1-log.dest ${JACKALOPE_LIB_TARGET})
add_test(stage-1-dest jackalope-test-1-log.dest)
