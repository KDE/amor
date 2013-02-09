########### install files ###############
#
#
kde4_create_handbook(index.docbook INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en SUBDIR amor)
kde4_create_manpage(man-amor.6.docbook 6 INSTALL_DESTINATION ${MAN_INSTALL_DIR})

