
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = data

####### This part is very amor specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	amor

# Which sources should be compiled for amor.
amor_SOURCES	= amordialog.cpp amor.cpp amorwidget.cpp main.cpp amoranim.cpp amorpm.cpp\
                  amorbubble.cpp amorconfig.cpp amortips.cpp AmorIface.skel

include_HEADERS = AmorIface.h

# the library search path
amor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
amor_LDADD  = $(LIB_KDEUI)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = amor.h amorwidget.h amoranim.h amorpm.h amordialog.h\
                 amorbubble.h amorconfig.h amortips.h

METASOURCES = AUTO

messages: tips.cpp
	$(XGETTEXT) tips.cpp *.cpp -o $(podir)/amor.pot

tips.cpp: data/tips-en
	perl ./preparetips > tips.cpp
	
appsdir = $(kde_appsdir)/Toys
apps_DATA = amor.desktop

KDE_ICON = amor

