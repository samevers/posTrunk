include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

bin_PROGRAMS=test_run
test_run_SOURCES=demo.cpp string_Util.cpp
test_run_LDADD= -lssplatform -lencoding -L../_lib -lpostag
test_run_LDFLAGS=-DHAVE_CXX_STDHEADERS -DLINUX -DACE_HAS_EVENT_POLL -D_FILE_OFFSET_BITS=64 -fPIC -ggdb

AM_CPPFLAGS=-DSS_DOCID_BITS=256 -I../src

