SUBDIRS = blob ghost eyes b

data_DATA = blobrc ghostrc eyesrc brc tips-en

# This is where it will all be installed
datadir = $(kde_datadir)/amor

install-data-local:
	(cd $(datadir) && ln -fs tips-en tips-default)
