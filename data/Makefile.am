SUBDIRS = blob ghost eyes b

data_DATA = blobrc ghostrc eyesrc brc tips-en

# This is where it will all be installed
datadir = $(kde_datadir)/amor

install-data-local:
	-rm -f $(datadir)/tips-default
	$(LN_S) tips-en $(datadir)/tips-default

uninstall-local:
	-rm -f $(datadir)/tips-default
