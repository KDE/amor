SUBDIRS = blob ghost eyes b neko pingu worm static tao

data_DATA = blobrc ghostrc eyesrc brc nekorc pingurc wormrc tips-en \
	    billyrc bsdrc tuxrc taorc

# This is where it will all be installed
datadir = $(kde_datadir)/amor

install-data-local:
	-rm -f $(DESTDIR)$(datadir)/tips
	$(LN_S) $(datadir)/tips-en $(DESTDIR)$(datadir)/tips

uninstall-local:
	-rm -f $(DESTDIR)$(datadir)/tips
