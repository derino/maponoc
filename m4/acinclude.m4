# HM_CHECK_ENVVAR([ENVVAR])
# --------------------------------------
# AC_DEFUN([HM_CHECK_ENVVAR],
# [
# AC_MSG_CHECKING([$1])

# if test x"$$1" = x ; then
#    AC_MSG_ERROR([ $1 must be set])
# fi

# AC_MSG_RESULT([$$1])
# ])


# OD_CHECK_OCTAVE()
# --------------------------------------
AC_DEFUN([OD_CHECK_OCTAVE],
[
AC_MSG_CHECKING(for OCTAVE libraries)

#-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m32 -march=i686 -mtune=atom -fasynchronous-unwind-tables
OCTAVE_CXXFLAGS="`mkoctfile -p CXXFLAGS`"

# -I/usr/include/octave-3.4.3/octave/.. -I/usr/include/octave-3.4.3/octave
OCTAVE_INCFLAGS="`mkoctfile -p INCFLAGS`"

OCTAVE_CFLAGS="${OCTAVE_CXXFLAGS} ${OCTAVE_INCFLAGS}"
AC_SUBST([OCTAVE_CFLAGS])

#-L/usr/lib/octave/3.4.3
OCTAVE_LIBDIR="`mkoctfile -p LFLAGS`"
OCTAVE_LIBOCTAVE="`mkoctfile -p LIBOCTAVE`"
OCTAVE_LIBS="${OCTAVE_LIBDIR} ${OCTAVE_LIBOCTAVE}"
# -loctave
#mkoctfile -p LIBOCTAVE
AC_SUBST([OCTAVE_LIBS])

if test x"$OCTAVE_LIBS" = x ; then
   AC_MSG_ERROR([ octave libs must be installed])
fi

AC_MSG_RESULT(yes)
])

