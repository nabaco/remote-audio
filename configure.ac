m4_define([PKG_VER], [m4_esyscmd([echo -n ${VERSION:-0.0.1-dev}])])

AC_INIT([remote-audio], [PKG_VER], [your_email@example.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC

PKG_PROG_PKG_CONFIG()
# Check for GStreamer
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0])

AC_CONFIG_FILES([Makefile receiver/Makefile transmitter/Makefile scripts/Makefile])

# Systemd related checks
AC_ARG_WITH([systemdsystemunitdir],
            [AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files])],,
            [with_systemdsystemunitdir=auto])
AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"], [
       def_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)

       AS_IF([test "x$def_systemdsystemunitdir" = "x"],
             [AS_IF([test "x$with_systemdsystemunitdir" = "xyes"],
                    [AC_MSG_ERROR([systemd support requested but pkg-config unable to query systemd package])])
             with_systemdsystemunitdir=no],
             [with_systemdsystemunitdir="$def_systemdsystemunitdir"])])
AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
      [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])
AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])
AC_OUTPUT
