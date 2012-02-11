
#INCLUDES = -I../mpreader -I../analyticalmodel/commcompmodel -I../analyticalmodel/reliabilitymodel $(LIBMOMH_CFLAGS) $(SIGC_CFLAGS) $(GSL_CFLAGS) $(BUDDY_CFLAGS)
AM_CPPFLAGS = -Wall -g
#AM_LDFLAGS = -static
LDADD = $(LIBMOMH_LIBS) $(SIGC_LIBS) #$(BUDDY_LIBS) $(GSL_LIBS)

bin_PROGRAMS    = gamapper
#lib_LIBRARIES   = libfoo1.a libfoo2.a libfoo3.a ....
#check_PROGRAMS  = test1 test2 test3 ....
#TESTS           = $(check_PROGRAMS)
#include_HEADERS = header1.h header2.h ....


gamapper_SOURCES = gcc-double-comparison-fix.h \
		   gcc-double-comparison-fix.cpp \
		   MappingMOMHSolution.hpp \
		   MappingMOMHSolution.cpp \
		   GAMapperMain.cpp

gamapper_LDADD   = ../mpreader/libmpreader.la ../analyticalmodel/commcompmodel/libcommcompmodel.la ../analyticalmodel/reliabilitymodel/libreliabilitymodel.la ../kpnmodel/libkpnmodel.la $(LDADD)
gamapper_CXXFLAGS = -I../mpreader -I../analyticalmodel/commcompmodel -I../analyticalmodel/reliabilitymodel -I../kpnmodel $(LIBMOMH_CFLAGS) $(SIGC_CFLAGS) $(GSL_CFLAGS) $(BUDDY_CFLAGS)
gamapper_LDFLAGS = -pthread



#gamapper_LDFLAGS = -static
#lpmapper_LDFLAGS = $(AM_LDFLAGS)
#prog1_DEPENDENCIES = dep1 dep2 dep3 ...
