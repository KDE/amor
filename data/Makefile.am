SUBDIRS = blob ghost eyes b neko pingu worm static tao

amor_DATA = blobrc ghostrc eyesrc brc nekorc pingurc wormrc tips-en \
	    billyrc bsdrc tuxrc taorc

# This is where it will all be installed
amordir = $(kde_datadir)/amor

install-data-local:
	-rm -f $(DESTDIR)$(amordir)/tips
	$(LN_S) $(amordir)/tips-en $(DESTDIR)$(datadir)/tips

uninstall-local:
	-rm -f $(DESTDIR)$(amordir)/tips
