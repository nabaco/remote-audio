m4_define([PKG_VER], [m4_esyscmd([echo -n ${VERSION:-0.0.1-dev}])])

AC_INIT([remote-audio], [PKG_VER], [your_email@example.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC

# Check for GStreamer
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0])

AC_CONFIG_FILES([Makefile receiver/Makefile transmitter/Makefile])
AC_OUTPUT
