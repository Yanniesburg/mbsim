SUBDIRS = finite_elements .

noinst_LTLIBRARIES = libflexiblebody.la
libflexiblebody_la_SOURCES = generic_flexible_ffr_body.cc\
                             flexible_ffr_body.cc\
                             calculix_body.cc\
			     flexible_body_1s.cc\
			     flexible_body_1s_21_rcm.cc\
			     flexible_body_1s_21_ancf.cc\
			     flexible_body_1s_33_rcm.cc\
			     flexible_body_1s_33_ancf.cc\
			     flexible_body_1s_cosserat.cc\
			     flexible_body_1s_21_cosserat.cc\
			     flexible_body_1s_33_cosserat.cc\
			     flexible_body_1s_23_bta.cc\
			     flexible_body_2s.cc\
			     flexible_body_2s_13.cc\
			     flexible_body_2s_13_disk.cc\
			     flexible_body_2s_13_mfr_mindlin.cc\
			     flexible_body_linear_external_ffr.cc

libflexiblebody_la_CPPFLAGS = -I$(top_srcdir) $(DEPS_CFLAGS) $(OPENMBVCPPINTERFACE_CFLAGS) $(NURBS_CFLAGS)
libflexiblebody_la_LIBADD = finite_elements/libfe.la
libflexiblebody_la_LIBADD += $(DEPS_LIBS) $(OPENMBVCPPINTERFACE_LIBS) $(NURBS_LIBS)

flexiblebodyincludedir = $(includedir)/mbsimFlexibleBody/flexible_body

flexiblebodyinclude_HEADERS = generic_flexible_ffr_body.h\
			      flexible_ffr_body.h\
			      calculix_body.h\
			      flexible_body_1s.h\
			      flexible_body_1s_21_rcm.h\
			      flexible_body_1s_21_ancf.h\
			      flexible_body_1s_33_rcm.h\
			      flexible_body_1s_33_ancf.h\
			      flexible_body_1s_cosserat.h\
			      flexible_body_1s_21_cosserat.h\
			      flexible_body_1s_33_cosserat.h\
			      flexible_body_1s_23_bta.h\
			      flexible_body_2s.h\
			      flexible_body_2s_13.h\
			      flexible_body_2s_13_disk.h\
			      flexible_body_2s_13_mfr_mindlin.h\
			      flexible_body_linear_external_ffr.h
