add_subdirectory( pics ) 
install( FILES blobrc ghostrc eyesrc bonhommerc nekorc pingurc wormrc tips-en  	    billyrc bsdrc tuxrc taorc  DESTINATION ${DATA_INSTALL_DIR}/amor)

# TODO $(LN_S) $(amordir)/tips-en $(DESTDIR)$(amordir)/tips
