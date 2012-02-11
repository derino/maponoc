INCLUDES = -I../../mpreader -I../commcompmodel $(GSL_CFLAGS) $(BUDDY_CFLAGS) # $(OCTAVE_CFLAGS)

AM_CPPFLAGS = -Wall -g

#LDADD = $(OCTAVE_LIBS) $(BUDDY_LIBS)

#noinst_LTLIBRARIES = libreliabilitymodel.la
lib_LTLIBRARIES = libreliabilitymodel.la
include_HEADERS = ReliabilityCalculator.h
libreliabilitymodel_la_SOURCES = ReliabilityCalculator.h \
		  ReliabilityCalculator.cpp

# ../../mpreader/libmpreader.la 
libreliabilitymodel_la_LIBADD = $(GSL_LIBS) $(BUDDY_LIBS)
#libreliabilitymodel_la_LDFLAGS = -rpath=$(OCTAVE_LIBS)
libreliabilitymodel_la_LDFLAGS = -static
#libkpnonnocmodel_la_LIBADD = ../mpreader/libmpreader.a
#libkpnonnocmodel_a_LIBADD = MappingSolution.o MappingSolutionSet.o ReliabilityCalculator.o ../mpreader/libmpreader.a
#libreliabilitymodel_la_DEPENDENCIES = ../../mpreader/libmpreader.la

