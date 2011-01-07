#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([mavhub], [0.1], [mavhub@calihem.de])

AC_LANG([C++])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([auxdir])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_RANLIB

# doxygen support
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME], [doc/Doxyfile], [doc])

# unit tests support
AC_ARG_ENABLE(tests,
	AC_HELP_STRING([--enable-tests=yes|no],
		[enable automated tests (default is no)]),
	[case "${enableval}" in
		yes)
			tests=true
			AX_BOOST_BASE
			AX_BOOST_UNIT_TEST_FRAMEWORK
			;;
		no)
			tests=false
			;;
		*)
			AC_MSG_ERROR(bad value ${enableval} for --enable-tests)
			;;
		esac],
	[tests=false])
AM_CONDITIONAL(COND_UNIT_TESTS, test x$tests = xtrue)

# Checks for libraries.
AC_CHECK_LIB([cv], [cvCreateMat])
AC_CHECK_LIB([pthread], [pthread_mutex_lock])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h netdb.h netinet/in.h strings.h sys/ioctl.h sys/socket.h sys/time.h termios.h unistd.h])
AC_CHECK_HEADERS([mavlink.h], , [AC_MSG_ERROR( [Please specify header directory for mavlink, e.g. configure CPPFLAGS="-I/home/user/mavlink/include/huch"] )])
AC_CHECK_HEADERS([mkhuchlink.h], , [AC_MSG_WARN( [Please specify header directory for mkhuchlink, e.g. configure CPPFLAGS="-I/home/user/mkhuchlink"] )], [-])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday inet_ntoa pow select socket strerror])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/application/Makefile
                 src/core/Makefile
                 src/io/Makefile
                 src/module/Makefile
                 src/protocol/Makefile
                 src/sensor/Makefile
                 tests/Makefile])

AC_OUTPUT

echo "
  ($PACKAGE_NAME) version $PACKAGE_VERSION
  Prefix.........: $prefix
  C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
  Unit tests.....: $tests
"