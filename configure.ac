AC_INIT([maponoc - Mapper for KPN on NoCS], [0.9],
        [Onur Derin <oderin@users.sourceforge.net>],
        [maponoc])
AM_INIT_AUTOMAKE
#AC_CANONICAL_SYSTEM
#AM_MAINTAINER_MODE
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_CXX
#AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET
#AC_DISABLE_SHARED
AC_CHECK_PROG(PKGCONFIG, [pkg-config], [yes])
PKG_CHECK_MODULES(SIGC, sigc++-2.0)
PKG_CHECK_MODULES(GSL, gsl)
#PKG_CHECK_MODULES(OCTAVE, octave)
#HM_CHECK_ENVVAR([MYVAR])
OD_CHECK_OCTAVE()
# Get the data on MOMH installation
PKG_CHECK_MODULES(LIBMOMH, libmomh)
# Get the data on buddy installation
PKG_CHECK_MODULES(BUDDY, buddy)
# Get the directory where CPLEX is installed
AC_ARG_WITH([cplex-path],
  [AS_HELP_STRING([--with-cplex-path],
    [location of the CPLEX base installation directory, defaults to /other/ILOG/CPLEX_Studio_AcademicResearch122])],
  [CPLEXBASEDIR="$withval"],
  [CPLEXBASEDIR='/other/ILOG/CPLEX_Studio_AcademicResearch122'])
AC_SUBST([CPLEXBASEDIR])
# Should we use 64bit CPLEX libraries?
AC_ARG_WITH([cplex],
  [AS_HELP_STRING([--with-64bit-cplex],
    [use 64bit CPLEX system, defaults to 32bit CPLEX])],
  [BIT64=true],
  [BIT64=false])
AM_CONDITIONAL([BIT64CPLEXSYSTEM], [test x$BIT64 = xtrue])
#AM_PROG_LIBTOOL
# for bash scripts
AC_PATH_PROGS(BASH, bash sh)
AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	m4/Makefile
	src/Makefile
	src/mpreader/Makefile
	src/analyticalmodel/Makefile
	src/analyticalmodel/commcompmodel/Makefile
	src/analyticalmodel/reliabilitymodel/Makefile	
	src/kpnmodel/Makefile
	src/lpmapper/Makefile
	src/gamapper/Makefile
	test/Makefile
	test/app2ps
	test/dot2ps
	test/paretoplot
	test/paretocompare
	test/paretocompareanimated
	test/paretocompareanimatedmttf
	test/paretocomparemttf
	test/paretocomparemttftc
	test/paretocomparemttfcomp
	test/paretocompareanimatedmttfcomp])
AC_OUTPUT
