########### install files ###############
#
#
kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR amor)
kdoctools_create_manpage(man-amor.6.docbook 6 INSTALL_DESTINATION ${MAN_INSTALL_DIR})

