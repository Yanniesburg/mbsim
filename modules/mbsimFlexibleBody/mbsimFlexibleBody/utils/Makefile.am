noinst_LTLIBRARIES = libutils.la
libutils_la_SOURCES = contact_utils.cc\
		      angles.cc\
		      cardan.cc\
		      revcardan.cc\
		      openmbv_utils.cc

utilsincludedir = $(includedir)/mbsimFlexibleBody/utils

libutils_la_CPPFLAGS = -I$(top_srcdir) $(DEPS_CFLAGS) $(OPENMBVCPPINTERFACE_CFLAGS) $(NURBS_CFLAGS)
libutils_la_LIBADD = $(DEPS_LIBS) $(OPENMBVCPPINTERFACE_LIBS) $(NURBS_LIBS)

utilsinclude_HEADERS = contact_utils.h\
		       angles.h\
		       cardan.h\
		       revcardan.h\
		       openmbv_utils.h
