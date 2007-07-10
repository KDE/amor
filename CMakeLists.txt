project(amor)
add_subdirectory( data ) 

remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)

########### next target ###############

set(amor_SRCS
   amordialog.cpp 
   amor.cpp 
   amorwidget.cpp 
   main.cpp 
   amoranim.cpp 
   amorpm.cpp 
   amorbubble.cpp 
   amorconfig.cpp 
   amortips.cpp )

qt4_add_dbus_adaptor( amor_SRCS org.kde.amor.xml amor.h Amor )


kde4_add_executable(amor ${amor_SRCS})

target_link_libraries(amor  ${KDE4_KDEUI_LIBS}  )

install(TARGETS amor  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES amor.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES org.kde.amor.xml DESTINATION ${DBUS_INTERFACES_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR}  )


