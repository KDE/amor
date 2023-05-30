cmake_minimum_required(VERSION 3.16)
project(amor)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(KF5_VERSION "5.101.0")
set(QT_REQUIRED_VERSION "5.15.0")
find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(FeatureSummary)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED
    Core
    DBus
    Widgets
    X11Extras
)

find_package(KF5DocTools ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5CoreAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5WindowSystem ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF5_VERSION} CONFIG REQUIRED)


find_package(XCB REQUIRED)


ecm_setup_version(2.4.0 VARIABLE_PREFIX AMOR VERSION_HEADER src/version.h)


# Search KDE installation
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_DEPRECATED_BEFORE=0x060000)

add_subdirectory( data )
add_subdirectory( src )
add_subdirectory( doc )
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
