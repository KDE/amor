# SUBDIRS = pics

amor_DATA = blobrc ghostrc eyesrc nekorc pingurc wormrc tips-en \
	    billyrc bsdrc tuxrc taorc

# This is where it will all be installed
amordir = $(kde_datadir)/amor

install-data-local:
	-rm -f $(DESTDIR)$(amordir)/tips
	$(LN_S) $(amordir)/tips-en $(DESTDIR)$(amordir)/tips

uninstall-local:
	-rm -f $(DESTDIR)$(amordir)/tips
