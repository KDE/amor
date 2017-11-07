cmake_minimum_required(VERSION 3.1)
project(amor)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(ECM_VERSION "5.39.0")
find_package(ECM ${ECM_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMInstallIcons)
include(KDEInstallDirs)

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.8.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED
    Core
    DBus
    Widgets
    X11Extras
)

set(KF5_VERSION "5.39.0")
find_package(KF5DocTools ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_VERSEION} CONFIG REQUIRED)
find_package(KF5KDELibs4Support ${KF5_VERSION} CONFIG REQUIRED)

find_package(X11 REQUIRED)


# Search KDE installation
add_definitions(-DQT_NO_CAST_FROM_ASCII)
add_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_DEPRECATED_BEFORE=0x060000)

add_subdirectory( data )
add_subdirectory( src )
add_subdirectory( doc )
