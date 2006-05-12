
add_subdirectory( pics ) 




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/amor FILES blobrc ghostrc eyesrc bonhommerc nekorc pingurc wormrc tips-en  	    billyrc bsdrc tuxrc taorc )




#original Makefile.am contents follow:

#SUBDIRS = pics
#
#amor_DATA = blobrc ghostrc eyesrc bonhommerc nekorc pingurc wormrc tips-en \
#	    billyrc bsdrc tuxrc taorc
#
## This is where it will all be installed
#amordir = $(kde_datadir)/amor
#
#install-data-local:
#	-rm -f $(DESTDIR)$(amordir)/tips
#	$(LN_S) $(amordir)/tips-en $(DESTDIR)$(amordir)/tips
#
#uninstall-local:
#	-rm -f $(DESTDIR)$(amordir)/tips
