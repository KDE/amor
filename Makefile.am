
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = data

####### This part is very amor specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	amor

# Which sources should be compiled for amor.
amor_SOURCES	= amordialog.cpp amor.cpp amorwidget.cpp main.cpp amoranim.cpp amorpm.cpp\
                  amorbubble.cpp amorconfig.cpp amortips.cpp

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

messages:
	$(XGETTEXT) $(amor_SOURCES) -o $(podir)/amor.pot

appsdir = $(kde_appsdir)/Games
apps_DATA = amor.kdelnk

KDE_ICON = amor

