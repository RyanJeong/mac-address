/*
  Copyright 2021 Munseong Jeong <ryan.m.jeong@hotmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#ifndef LIB_PLATFORM_H_
#define LIB_PLATFORM_H_

#if defined(_WIN32)    /* Windows (x86) */
# define PLATFORM_NAME "windows"
#elif defined(_WIN64)  /* Windows (x86) */
# define PLATFORM_NAME "windows"
#elif defined(__CYGWIN__) && !defined(_WIN32)  /* Cygwin POSIX under Windows */
# define PLATFORM_NAME "windows"
#elif defined(__ANDROID__)  /* Android (implies Linux, so it must come first) */
# define PLATFORM_NAME "android"
#elif defined(__linux__)  /* Debian, Ubuntu, Fedora, RedHat, CentOS, etc. */
# define PLATFORM_NAME "linux"
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
# include <sys/param.h>
# if defined(BSD)  /* FreeBSD, NetBSD, OpenBSD, DragonFly BSD */
#  define PLATFORM_NAME "bsd"
# endif  /* BSD */
#elif defined(__hpux)  /* HP-UX */
# define PLATFORM_NAME "hp-ux"
#elif defined(_AIX)  /* IBM AIX */
# define PLATFORM_NAME "aix"
#elif defined(__APPLE__) && defined(__MACH__)  /* Apple OSX and iOS (Darwin) */
# include <TargetConditionals.h>
# if TARGET_IPHONE_SIMULATOR == 1  /* Apple iOS (~iOS 9.0) */
#  define PLATFORM_NAME "ios"
# elif TARGET_OS_IPHONE == 1       /* Apple iOS */
#  define PLATFORM_NAME "ios"
# elif TARGET_OS_MAC == 1          /* Apple OSX */
#  define PLATFORM_NAME "osx"
# endif  /* TARGET_OS_MAC */
#elif defined(__sun) && defined(__SVR4)  /* Oracle Solaris, Open Indiana */
# define PLATFORM_NAME "solaris"
#else   /* !__sun || !__SVR4 */
# define PLATFORM_NAME NULL
#endif  /* !__sun || !__SVR4 */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief 
 * a function that return a name of platform, if determined, otherwise - an empty string
 */
const char* GetPlatformName();

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* LIB_PLATFORM_H_ */
