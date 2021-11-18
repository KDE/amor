include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
)

set(amor_SRCS)

ecm_qt_declare_logging_category(amor_SRCS HEADER amor_debug.h IDENTIFIER AMOR_LOG CATEGORY_NAME org.kde.amor)

qt5_add_dbus_adaptor(amor_SRCS org.kde.amor.xml amor.h Amor)

add_executable(amor)
target_sources(amor PRIVATE
        amoranimation.cpp
        amoranimation.h
        amorbubble.cpp
        amorbubble.h
        amorconfig.cpp
        amorconfig.h
        amor.cpp
        amordialog.cpp
        amordialog.h
        amor.h
        amorpixmapmanager.cpp
        amorpixmapmanager.h
        amorthememanager.cpp
        amorthememanager.h
        amortips.cpp
        amortips.h
        amorwidget.cpp
        amorwidget.h
        main.cpp
        queueitem.cpp
        queueitem.h
        ${amor_SRCS}
    )
target_link_libraries(amor
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::Widgets
    Qt5::X11Extras

    KF5::CoreAddons
    KF5::DBusAddons
    KF5::WidgetsAddons
    KF5::I18n
    KF5::ConfigCore
    KF5::WindowSystem
    KF5::XmlGui

    ${XCB_LIBRARIES}
)

install(TARGETS amor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.amor.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.amor.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

