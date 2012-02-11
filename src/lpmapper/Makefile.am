if BIT64CPLEXSYSTEM
SYSTEM = x86-64_sles10_4.1
else
SYSTEM = x86_sles10_4.1
endif !BIT64CPLEXSYSTEM
LIBFORMAT = static_pic

# CPLEXBASEDIR is a ./configure parameter
#CPLEXBASEDIR  = /other/ILOG/CPLEX_Studio_AcademicResearch122
CPLEXDIR      = $(CPLEXBASEDIR)/cplex
CONCERTDIR    = $(CPLEXBASEDIR)/concert
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include


INCLUDES = -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I../mpreader -I../analyticalmodel/commcompmodel
AM_CPPFLAGS = -Wall -g -O -fPIC -fexceptions -DNDEBUG -DIL_STD
AM_LDFLAGS = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) -pthread
if !BIT64CPLEXSYSTEM
AM_CPPFLAGS += -m32
AM_LDFLAGS += -m32
endif !BIT64CPLEXSYSTEM
LDADD = -lilocplex -lcplex -lconcert -lm

bin_PROGRAMS    = lpmapper
#lib_LIBRARIES   = libfoo1.a libfoo2.a libfoo3.a ....
#check_PROGRAMS  = test1 test2 test3 ....
#TESTS           = $(check_PROGRAMS)
#include_HEADERS = header1.h header2.h ....


lpmapper_SOURCES = InfeasibleSolutionException.h \
		   LPMapper.cpp \
		   LPMapper.h \
		   LPMapperMain.cpp \
		   MappingSolutionStatusAdapter.h
lpmapper_LDADD   = $(LDADD) ../mpreader/libmpreader.la ../analyticalmodel/commcompmodel/libcommcompmodel.la
#lpmapper_LDFLAGS = $(AM_LDFLAGS)
#prog1_DEPENDENCIES = dep1 dep2 dep3 ...
