kde4_header()

add_subdirectory( data ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


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

kde4_automoc(${amor_SRCS})

kde4_add_dcop_skels(amor_SRCS AmorIface.h )

kde4_add_executable(amor ${amor_SRCS})

target_link_libraries(amor  ${QT_AND_KDECORE_LIBS} kdeui )

install_targets(/bin amor )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES amor.desktop )
install_files( /include FILES AmorIface.h )

kde4_install_icons( ${ICON_INSTALL_DIR} crystalsvg )

kde4_footer()



#original Makefile.am contents follow:

#
## set the include path for X, qt and KDE
#INCLUDES= $(all_includes)
## claim, which subdirectories you want to install
#SUBDIRS = data
#
######## This part is very amor specific
## you can add here more. This one gets installed 
#bin_PROGRAMS = 	amor
#
## Which sources should be compiled for amor.
#amor_SOURCES	= amordialog.cpp amor.cpp amorwidget.cpp main.cpp amoranim.cpp amorpm.cpp\
#                  amorbubble.cpp amorconfig.cpp amortips.cpp AmorIface.skel
#
#include_HEADERS = AmorIface.h
#
## the library search path
#amor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
## the libraries to link against. Be aware of the order. First the libraries,
## that depend on the following ones.
#amor_LDADD  = $(LIB_KDEUI)
#
## this option you can leave out. Just, if you use "make dist", you need it
#noinst_HEADERS = amor.h amorwidget.h amoranim.h amorpm.h amordialog.h\
#                 amorbubble.h amorconfig.h amortips.h
#
#METASOURCES = AUTO
#
#messages: 
#	perl ./preparetips > tips.cpp
#	$(XGETTEXT) tips.cpp *.cpp -o $(podir)/amor.pot
#	rm -f tips.cpp
#
#xdg_apps_DATA = amor.desktop
#
#KDE_ICON = amor
#
