
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = data

####### This part is very amor specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	amor

# Which sources should be compiled for amor.
amor_SOURCES	= amor.cpp amorwidget.cpp main.cpp amoranim.cpp amorpm.cpp\
                  amordialog.cpp amorbubble.cpp amorconfig.cpp amortips.cpp

# the library search path
amor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
amor_LDADD  = $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = amor.h amorwidget.h amoranim.h amorpm.h amordialog.h\
                 amorbubble.h amorconfig.h amortips.h

# just to make sure, automake makes them 
METASOURCES = amor.moc amorwidget.moc amordialog.moc amorbubble.moc

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(amor_SOURCES) && mv messages.po ../po/amor.pot

appsdir = $(kde_appsdir)/Games
apps_DATA = amor.kdelnk

kde_icon_DATA = amor.xpm

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-amor.xpm $(kde_minidir)/amor.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_minidir)/amor.xpm


