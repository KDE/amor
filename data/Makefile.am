SUBDIRS = blob ghost eyes b neko pingu worm

data_DATA = blobrc ghostrc eyesrc brc nekorc pingurc wormrc tips-en tips-fr

# This is where it will all be installed
datadir = $(kde_datadir)/amor

install-data-local:
	-rm -f $(datadir)/tips-default
	$(LN_S) tips-en $(DESTDIR)$(datadir)/tips-default

uninstall-local:
	-rm -f $(datadir)/tips-default
