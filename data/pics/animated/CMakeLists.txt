kde4_header()

add_subdirectory( blob ) 
add_subdirectory( bonhomme ) 
add_subdirectory( eyes ) 
add_subdirectory( ghost ) 
add_subdirectory( neko ) 
add_subdirectory( pingu ) 
add_subdirectory( tao ) 
add_subdirectory( worm ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = blob bonhomme eyes ghost neko pingu tao worm
