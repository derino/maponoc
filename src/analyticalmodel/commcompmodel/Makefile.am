INCLUDES = -I../../mpreader

AM_CPPFLAGS = -Wall -g

#LDADD = $(OCTAVE_LIBS) $(BUDDY_LIBS)

#noinst_LTLIBRARIES = libcommcompmodel.la
lib_LTLIBRARIES = libcommcompmodel.la
include_HEADERS = MappingSolution.h \
		  MappingSolutionSet.h
libcommcompmodel_la_SOURCES = MappingSolution.cpp \
		  MappingSolution.h \
		  MappingSolutionSet.cpp \
		  MappingSolutionSet.h

libcommcompmodel_la_LDFLAGS = -static
libcommcompmodel_la_LIBADD = ../../mpreader/libmpreader.la
#libkpnonnocmodel_a_LIBADD = MappingSolution.o MappingSolutionSet.o ReliabilityCalculator.o ../mpreader/libmpreader.a
#libkpnonnocmodel_a_DEPENDENCIES = dep1 dep2 dep3 ...
#libcommcompmodel_la_DEPENDENCIES = ../../mpreader/libmpreader.la
