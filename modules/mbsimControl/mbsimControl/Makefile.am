SUBDIRS =  .
  
lib_LTLIBRARIES = libmbsimControl.la
libmbsimControl_la_LDFLAGS = -avoid-version
libmbsimControl_la_SOURCES = \
  signal_.cc\
  object_sensors.cc\
  frame_sensors.cc\
  function_sensor.cc\
  link_sensors.cc\
  contact_sensor.cc\
  signal_manipulation.cc\
  linear_transfer_system.cc\
  massless_spring_damper.cc\
  signal_function.cc\
  extern_signal_sink.cc


libmbsimControl_la_CPPFLAGS = -I$(top_srcdir) $(DEPS_CFLAGS)
libmbsimControl_la_LIBADD = $(DEPS_LIBS)

mbsimControlincludedir = $(includedir)/mbsimControl
mbsimControlinclude_HEADERS = \
  namespace.h\
  signal_.h\
  extern_signal_source.h\
  extern_signal_sink.h\
  sensor.h\
  object_sensors.h\
  frame_sensors.h\
  function_sensor.h\
  link_sensors.h\
  contact_sensor.h\
  signal_manipulation.h\
  linear_transfer_system.h\
  massless_spring_damper.h\
  signal_function.h


include $(prefix)/share/mbxmlutils/python/deplibs.mk
install-exec-hook: deplibs.target
