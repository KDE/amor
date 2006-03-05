kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/amor/pics/static FILES billy.png bsd.png tux.png )

kde4_footer()



#original Makefile.am contents follow:

## Add all your pic files here
#amor_DATA = billy.png bsd.png tux.png
#
## This is where it will all be installed
#amordir = $(kde_datadir)/amor/pics/static
