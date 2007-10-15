/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/**
 * @file
 *
 * Global params for OpenRTE
 */
#ifndef ORTE_RUNTIME_PARAM_H
#define ORTE_RUNTIME_PARAM_H

#include "orte_config.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "opal/threads/mutex.h"
#include "opal/threads/condition.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

/* globals used by RTE - instanced in orte_params.c */

ORTE_DECLSPEC extern bool orte_debug_flag, orte_reuse_daemons, orte_timing;
ORTE_DECLSPEC extern bool orte_debug_daemons_flag, orte_debug_daemons_file_flag;
ORTE_DECLSPEC extern bool orte_infrastructure, orted_spin_flag, orte_no_daemonize_flag;

ORTE_DECLSPEC extern bool orte_daemon_died;

ORTE_DECLSPEC extern struct timeval orte_abort_timeout;

ORTE_DECLSPEC extern char **orte_launch_environ;
ORTE_DECLSPEC extern char **orted_cmd_line;
ORTE_DECLSPEC extern opal_mutex_t orted_comm_mutex;
ORTE_DECLSPEC extern opal_condition_t orted_comm_cond;
ORTE_DECLSPEC extern bool orte_orterun;
ORTE_DECLSPEC extern bool orted_comm_exit_cond;

/**
 * Whether ORTE is initialized or not
 */
ORTE_DECLSPEC extern bool orte_initialized;

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* ORTE_RUNTIME_PARAM_H */
