# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# just set the variable
APPSDIR = $(kde_appsdir)/Games
# set the include path for X, qt and KDE
INCLUDES= @all_includes@
# claim, which subdirectories you want to install
SUBDIRS = data

#DEFS = @DEFS@ -I. -I$(srcdir) -I.. -DDOTRACE
####### This part is very amor specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	amor

# Which sources should be compiled for amor.
amor_SOURCES	= amor.cpp amorwidget.cpp main.cpp amoranim.cpp amorpm.cpp\
                  amordialog.cpp amorbubble.cpp amorconfig.cpp amortips.cpp

# the library search path
amor_LDFLAGS = @all_libraries@ @KDE_RPATH@

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
amor_LDADD  = -lkdeui -lkdecore -lqt -lX11 -lXext @LIBSOCKET@     

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = amor.h amorwidget.h amoranim.h amorpm.h amordialog.h\
                 amorbubble.h amorconfig.h amortips.h

# just to make sure, automake makes them 
BUILTSOURCES =	amor.moc amorwidget.moc amordialog.moc amorbubble.moc

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n $(amor_SOURCES)

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) amor.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) amor.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-amor.xpm $(kde_minidir)/amor.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/amor.kdelnk
	-rm -f $(kde_icondir)/amor.xpm
	-rm -f $(kde_minidir)/amor.xpm

# add a dependency for every moc file to be full portable
amor.cpp:amor.moc
amor.moc: amor.h
	$(MOC) amor.h -o amor.moc

amorwidget.cpp:amorwidget.moc
amorwidget.moc: amorwidget.h
	$(MOC) amorwidget.h -o amorwidget.moc

amordialog.cpp:amordialog.moc
amordialog.moc: amordialog.h
	$(MOC) amordialog.h -o amordialog.moc

amorbubble.cpp:amorbubble.moc
amorbubble.moc: amorbubble.h
	$(MOC) amorbubble.h -o amorbubble.moc

