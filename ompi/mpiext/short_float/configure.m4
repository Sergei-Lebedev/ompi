# -*- shell-script -*-
#
# Copyright (c) 2004-2009 The Trustees of Indiana University.
#                         All rights reserved.
# Copyright (c) 2012-2015 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# OMPI_MPIEXT_shortfloat_CONFIG([action-if-found], [action-if-not-found])
# -----------------------------------------------------------
AC_DEFUN([OMPI_MPIEXT_short_float_CONFIG],[
    AC_CONFIG_FILES([ompi/mpiext/short_float/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/short_float/c/Makefile])

    AS_IF([test "$enable_short_float" = "yes"],
          [$1],
          [$2])
])

