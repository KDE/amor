kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/amor/pics/preview FILES billy.png bonhomme.png ghost.png neko.png tux.png blob.png bsd.png eyes.png tao.png worm.png )

kde4_footer()



#original Makefile.am contents follow:

## Add all your pic files here
#amor_DATA = billy.png bonhomme.png ghost.png neko.png tux.png blob.png bsd.png eyes.png tao.png worm.png
#
## This is where it will all be installed
#amordir = $(kde_datadir)/amor/pics/preview
