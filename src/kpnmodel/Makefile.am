INCLUDES = -I../../mpreader # due to FORK/VOTER_KEY defines

AM_CPPFLAGS = -Wall -g

#LDADD = $(OCTAVE_LIBS) $(BUDDY_LIBS)

lib_LTLIBRARIES = libkpnmodel.la
include_HEADERS = kpnmodel.h \
		  DataDependency.hpp \
		  Edge.hpp \
		  Fork.hpp \
		  GraphElement.hpp \
		  Graph.hpp \
		  GraphUtilities.hpp \
		  KPNTaskGraph.hpp \
		  Pair.hpp \
		  Task.hpp \
		  Vertex.hpp \
		  Voter.hpp \
		  WeightedEdge.hpp

libkpnmodel_la_SOURCES = kpnmodel.h \
		  DataDependency.hpp \
		  Edge.hpp \
		  Fork.hpp \
		  GraphElement.hpp \
		  Graph.hpp \
		  GraphUtilities.hpp \
		  KPNTaskGraph.hpp \
		  Pair.hpp \
		  Task.hpp \
		  Vertex.hpp \
		  Voter.hpp \
		  WeightedEdge.hpp

# static kalkinca calismazsa libadd'i de kaldir
libkpnmodel_la_LIBADD = ../mpreader/libmpreader.la
libkpnmodel_la_LDFLAGS = -static
#libkpnonnocmodel_a_LIBADD = MappingSolution.o MappingSolutionSet.o ReliabilityCalculator.o ../mpreader/libmpreader.a
#libkpnonnocmodel_a_DEPENDENCIES = dep1 dep2 dep3 ...
