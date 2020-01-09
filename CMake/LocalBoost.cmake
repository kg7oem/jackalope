if (APPLE)
    # OS X requires Boost 1.69.0 because of
    # https://github.com/boostorg/asio/commit/43874d5497414c67655d901e48c939ef01337edb
    set(Boost_MIN_MAJOR "1")
    set(Boost_MIN_MINOR "69")
    set(Boost_MIN_PATCH "0")
    set(Boost_MIN_HASH "89da45aca0774f68f897434882e8ec58f0673807c3a862315b90e893be90184722c098ad5822b89602b98ac3cc5a9f89c807079fd0f7e9c3f97be0d415414014")
else ()
    # minimum Boost version is 1.66.0 because of defer() in ASIO and the
    # default should be the minimum version possible for any platform
    set(Boost_MIN_MAJOR "1")
    set(Boost_MIN_MINOR "66")
    set(Boost_MIN_PATCH "0")
    set(Boost_MIN_HASH "8d537fa1fcd24dfa3bc107741d20d93624851b724883c5cfe0cdcd8b8390939869e0f41a36f52d3051f129a8405a574a1a095897f82ece747740b34a1e52ffdb")
endif (APPLE)

set(Boost_MIN_VERSION ${Boost_MIN_MAJOR}.${Boost_MIN_MINOR}.${Boost_MIN_PATCH})

if (NOT ${LOCAL_BOOST})
    message("Minimum Boost version is ${Boost_MIN_VERSION}")
    find_package(Boost ${Boost_MIN_VERSION} COMPONENTS date_time filesystem regex serialization system thread)

    if ("${Boost_LIBRARIES}" STREQUAL "")
        if (${DOWNLOAD_BOOST_OK})
            set(LOCAL_BOOST ON)
        else ()
            message(SEND_ERROR "Could not find required Boost libraries; try setting DOWNLOAD_BOOST_OK=ON to download and compile one")
            set(Boost_INCLUDE_DIR "")
        endif (${DOWNLOAD_BOOST_OK})
    endif ("${Boost_LIBRARIES}" STREQUAL "")
endif (NOT ${LOCAL_BOOST})

if (${LOCAL_BOOST})
    message("  Will download and compile Boost")

    if (APPLE)
        # OS X requires Boost 1.69.0 because of
        # https://github.com/boostorg/asio/commit/43874d5497414c67655d901e48c939ef01337edb
        set(Boost_MIN_MAJOR "1")
        set(Boost_MIN_MINOR "69")
        set(Boost_MIN_PATCH "0")
        set(Boost_MIN_HASH "89da45aca0774f68f897434882e8ec58f0673807c3a862315b90e893be90184722c098ad5822b89602b98ac3cc5a9f89c807079fd0f7e9c3f97be0d415414014")
    else ()
        # minimum Boost version is 1.66.0 because of defer() in ASIO and the
        # default should be the minimum version possible for any platform
        set(Boost_MIN_MAJOR "1")
        set(Boost_MIN_MINOR "66")
        set(Boost_MIN_PATCH "0")
        set(Boost_MIN_HASH "8d537fa1fcd24dfa3bc107741d20d93624851b724883c5cfe0cdcd8b8390939869e0f41a36f52d3051f129a8405a574a1a095897f82ece747740b34a1e52ffdb")
    endif (APPLE)

    ExternalProject_Add(
        jackalope-boost

        EXCLUDE_FROM_ALL ON
        BUILD_IN_SOURCE TRUE

        URL "https://dl.bintray.com/boostorg/release/${Boost_MIN_MAJOR}.${Boost_MIN_MINOR}.0/source/boost_${Boost_MIN_MAJOR}_${Boost_MIN_MINOR}_0.tar.gz"
        URL "https://newcontinuum.dl.sourceforge.net/project/boost/boost/${Boost_MIN_MAJOR}.${Boost_MIN_MINOR}.${Boost_MIN_PATCH}/boost_${Boost_MIN_MAJOR}_${Boost_MIN_MINOR}_${Boost_MIN_PATCH}.tar.gz"
        URL_HASH "SHA512=${Boost_MIN_HASH}"

        CONFIGURE_COMMAND ./bootstrap.sh
        BUILD_COMMAND ./b2 --with-date_time --with-filesystem --with-regex --with-serialization --with-system --with-thread --build-type=minimal --layout=system cxxflags=-fPIC link=static runtime-link=static threading=multi
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(jackalope-boost SOURCE_DIR)
    set(Boost_INCLUDE_DIR "${SOURCE_DIR}")
    set(Boost_LIBRARIES "${SOURCE_DIR}/stage/lib/libboost_system.a" "${SOURCE_DIR}/stage/lib/libboost_filesystem.a")
endif (${LOCAL_BOOST})
