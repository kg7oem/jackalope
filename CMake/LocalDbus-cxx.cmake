message("  Will download and compile dbus-cxx")

pkg_check_modules( dbus REQUIRED dbus-1>=1.3 )
pkg_check_modules( sigc REQUIRED sigc++-2.0 )

ExternalProject_Add(
    jackalope-dbus-cxx

    EXCLUDE_FROM_ALL ON

    URL https://github.com/dbus-cxx/dbus-cxx/archive/0.11.0.tar.gz
    URL_HASH SHA512=0ac63bd6a7f4dcdfc7a1db9bcbdb491a8e84ced26d1b14847dd837c7cdf2c14d9101710b72b3e9bc247f0c1d31aac44e24e3f23f165efbd0c4e1799a4b0d0448
    PATCH_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/jackalope-dbus-cxx-prefix/src/jackalope-dbus-cxx/ && patch -p1 < ${PROJECT_SOURCE_DIR}/patches/dbus-cxx-0.11.0.patch
    CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DENABLE_TOOLS=ON -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
    INSTALL_COMMAND ""
)

set(LOCAL_DBUS_CXX ON)

ExternalProject_Get_Property(jackalope-dbus-cxx SOURCE_DIR)
ExternalProject_Get_Property(jackalope-dbus-cxx BINARY_DIR)

set(DBUS_CXX_INCLUDE_DIRS ${dbus_INCLUDE_DIRS} ${sigc_INCLUDE_DIRS} ${SOURCE_DIR} ${SOURCE_DIR}/dbus-cxx/ ${BINARY_DIR} ${BINARY_DIR}/dbus-cxx/)
set(DBUS_CXX_LIBRARIES ${BINARY_DIR}/libdbus-cxx.a ${dbus_LIBRARIES} ${sigc_LIBRARIES})
set(DBUS_CXX_TOOLS ${BINARY_DIR}/tools)
