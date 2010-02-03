add_subdirectory( pics )
add_subdirectory( icons )

install( FILES
    tips-en
    blobrc
    ghostrc
    eyesrc
    bonhommerc
    nekorc
    pingurc
    wormrc
    billyrc
    bsdrc
    tuxrc
    taorc
  DESTINATION ${DATA_INSTALL_DIR}/amor
)

# TODO $(LN_S) $(amordir)/tips-en $(DESTDIR)$(amordir)/tips
