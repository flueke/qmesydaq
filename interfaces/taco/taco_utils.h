/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2007-2014 Jens Krüger <jens.krueger@frm2.tum.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File       : util_api.c
 *
 * Project    : Device Servers with SUN-RPC
 *
 * Description: Application Programmers Interface
 *              Utilities for the interface to access and
 *              handle remote devices.
 *
 * Author(s)  : Jens Krüger <jens.krueger@frm2.tum.de>
 *              $Author: andy_gotz $
 *
 * Original   : June 2007
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:                $Date: 2008-10-13 19:04:02 $
 *
 ********************************************************************-*/

#ifndef	TACO_UTILS_H
#define	TACO_UTILS_H

/**
 * @defgroup utilsAPI Some helpful utility functions
 * @ingroup API
 *
 * This group collects some helpful functions, i.e. copy and convert strings
 */

#ifdef __cplusplus
extern "C" {
#endif

char *str_tolower(char *str);
char *strcpy_tolower(char *dest, const char *src);
char *strncpy_tolower(char *dest, const char *src, size_t n);
char *strcat_tolower(char *dest, const char *src);
char *strdup_tolower(const char *str);
void taco_setenv(const char *env_name, const char *env_value, int overwrite);

#ifdef __cplusplus
}
#endif

#endif
