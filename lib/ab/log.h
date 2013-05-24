/**
 * Behaviours - UML-like graphic programming language
 * Copyright (C) 2013 Coralbits SL & AISoy Robotics.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef AB_LOG_H__
#define AB_LOG_H__

#ifdef HAVE_AISOY1
/// FIXME, use proper custom logging here, not AISoy1 dependant.
#include <aisoy1/log.h>
#else

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#ifndef DEBUG
#ifdef __DEBUG__
/// Only who if environmental variable AISOY_DEBUG has the base filename.
/// Show debug info on console, only if __DEBUG__ is defined
#define DEBUG(...){ const char *base=basename((char*)__FILE__), *env_debug=getenv("AISOY_DEBUG"); if (!env_debug || strstr(env_debug, base)) { fprintf(stderr, "\033[01;34m[DEBUG %5d] %12s:%-4d ", (int)syscall(SYS_gettid), base, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m\n"); } }

/// Same as DEBUG, but do not do the carriage return.
#define DEBUGL(...){ fprintf(stderr, "\r\033[01;34m[DEBUG] %12s:%-4d ", basename((char*)__FILE__), __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m                                     "); }
#else
#define DEBUG(...)
#define DEBUGL(...)
#endif
#endif

/// Shows a warning on the console
#ifndef WARNING
# define WARNING(...){ fprintf(stderr, "\033[01;33m[WARN ] %12s:%-4d ", basename((char*)__FILE__), __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m\n"); }
#endif
/// Shows an error on the console
#ifndef ERROR
# define ERROR(...){ fprintf(stderr, "\033[31m[ERROR] %12s:%-4d ", basename((char*)__FILE__), __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m\n"); }
#endif
/// Shows some generic info on the console
#ifndef INFO
# define INFO(...){ fprintf(stderr, "\033[0m[INFO ] %12s:%-4d ", basename((char*)__FILE__), __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#endif

#endif

#endif

