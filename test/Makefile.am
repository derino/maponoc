
#INCLUDES = -I../mpreader -I../analyticalmodel/commcompmodel -I../analyticalmodel/reliabilitymodel $(LIBMOMH_CFLAGS) $(SIGC_CFLAGS) $(GSL_CFLAGS) $(BUDDY_CFLAGS)
AM_CPPFLAGS = -Wall -g
AM_LDFLAGS = -static
LDADD = $(LIBMOMH_LIBS) $(SIGC_LIBS) #$(BUDDY_LIBS) $(GSL_LIBS)


bin_PROGRAMS    = mttf_tr app2dot apply_nmr visualize_map pareto_tbl

# mttf_tr
mttf_tr_SOURCES = mttf_tr.cpp
mttf_tr_LDADD   = $(top_srcdir)/src/mpreader/libmpreader.la \
		  $(top_srcdir)/src/analyticalmodel/reliabilitymodel/libreliabilitymodel.la \
		  $(top_srcdir)/src/analyticalmodel/commcompmodel/libcommcompmodel.la \
		  $(LDADD) # ../analyticalmodel/commcompmodel/libcommcompmodel.la
mttf_tr_CXXFLAGS = -I$(top_srcdir)/src/mpreader \
		   -I$(top_srcdir)/src/analyticalmodel/reliabilitymodel \
		   -I$(top_srcdir)/src/analyticalmodel/commcompmodel \
		   $(LIBMOMH_CFLAGS) \
		   $(SIGC_CFLAGS) \
		   $(GSL_CFLAGS) \
		   $(BUDDY_CFLAGS)
mttf_tr_LDFLAGS = -pthread # -I../analyticalmodel/commcompmodel


# applyNMR
apply_nmr_SOURCES = apply_nmr.cpp
apply_nmr_LDADD   = $(top_srcdir)/src/mpreader/libmpreader.la \
		  $(top_srcdir)/src/kpnmodel/libkpnmodel.la
apply_nmr_CXXFLAGS = -I$(top_srcdir)/src/mpreader \
		   -I$(top_srcdir)/src/kpnmodel


# app2dot
app2dot_SOURCES = app2dot.cpp
app2dot_LDADD   = $(top_srcdir)/src/mpreader/libmpreader.la \
		  $(top_srcdir)/src/kpnmodel/libkpnmodel.la
app2dot_CXXFLAGS = -I$(top_srcdir)/src/mpreader \
		   -I$(top_srcdir)/src/kpnmodel


# visualize_map
visualize_map_SOURCES = visualize_map.cpp
visualize_map_LDADD   = $(top_srcdir)/src/mpreader/libmpreader.la
visualize_map_CXXFLAGS = -I$(top_srcdir)/src/mpreader

# pareto_tbl
pareto_tbl_SOURCES = pareto_tbl.cpp


# scripts
HELPERS = 	paretoplot.gp \
		paretocompare.gp \
		paretocomparegif.gp \
		paretocomparegifmttf.gp \
		paretocomparemttf.gp \
		paretocomparemttftc.gp \
		paretocomparemttfcomp.gp \
		paretocomparegifmttfcomp.gp
BETIKLER = 	app2ps \
		dot2ps \
		paretoplot  \
		paretocompare  \
	    paretocompareanimated \
	    paretocompareanimatedmttf \
	    paretocomparemttf \
	    paretocomparemttftc \
	    paretocomparemttfcomp \
	    paretocompareanimatedmttfcomp
bin_SCRIPTS =	$(BETIKLER) $(HELPERS)
CLEANFILES = $(BETIKLER) #app2ps paretoplot #$(bin_SCRIPTS)
EXTRA_DIST = $(HELPERS) #paretoplot.gp

#app2ps: $(srcdir)/app2ps.sh
#	rm -f app2ps
#	echo "#!" $(BASH) > app2ps
#	cat $(srcdir)/app2ps.sh >> app2ps
#	chmod ugo+x app2ps

