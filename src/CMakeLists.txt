set( amor_SRCS
  main.cpp
  amordialog.cpp
  amor.cpp
  amorwidget.cpp
  amoranim.cpp
  amorpm.cpp
  amorbubble.cpp
  amorconfig.cpp
  amortips.cpp 
)

qt4_add_dbus_adaptor( amor_SRCS org.kde.amor.xml amor.h Amor )

kde4_add_executable( amor ${amor_SRCS} )
target_link_libraries( amor ${KDE4_KDEUI_LIBS} ${X11_LIBRARIES} )

install( TARGETS amor  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES amor.desktop     DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES org.kde.amor.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )

