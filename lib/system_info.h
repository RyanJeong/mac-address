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

#ifndef LIB_MAC_H_
#define LIB_MAC_H_

/* C Standard Library */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#if defined(_WIN32)    /* Windows (x86) */
/* [WIP] not yet checked */
# define SYSTEM_INFO_WIN32
#elif defined(_WIN64)  /* Windows (x86) */
/* [WIP] not yet checked */
# define SYSTEM_INFO_WIN64
#elif defined(__CYGWIN__) && !defined(_WIN32)  /* Cygwin POSIX under Windows */
/* [WIP] not yet checked */
# define SYSTEM_INFO_CYGWIN
#elif defined(__ANDROID__)  /* Android (implies Linux, so it must come first) */
/* [WIP] not yet checked */
# define SYSTEM_INFO_ANDROID
#elif defined(__linux__)  /* Debian, Ubuntu, Fedora, RedHat, CentOS, etc. */
# define SYSTEM_INFO_LINUX
# include <unistd.h>
# include <sys/ioctl.h>
# include <sys/utsname.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <linux/if.h>
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
# include <sys/param.h>
# if defined(BSD)  /* FreeBSD, NetBSD, OpenBSD, DragonFly BSD */
#  define SYSTEM_INFO_BSD
/* [WIP] not yet checked */
# endif  /* BSD */
#elif defined(__hpux)  /* HP-UX */
# define SYSTEM_INFO_HP_UX
/* [WIP] not yet checked */
#elif defined(_AIX)  /* IBM AIX */
# define SYSTEM_INFO_IBM_AIX
/* [WIP] not yet checked */
#elif defined(__APPLE__) && defined(__MACH__)  /* Apple OSX and iOS (Darwin) */
# include <TargetConditionals.h>
# if TARGET_IPHONE_SIMULATOR == 1  /* Apple iOS (~iOS 9.0) */
#  define SYSTEM_INFO_IOS
/* [WIP] not yet checked */
# elif TARGET_OS_IPHONE == 1       /* Apple iOS */
#  define SYSTEM_INFO_IOS_9
/* [WIP] not yet checked */
# elif TARGET_OS_MAC == 1          /* Apple OSX */
#  define SYSTEM_INFO_MAC
#  include <net/if_dl.h>
#  include <net/if_types.h>
#  include <ifaddrs.h>
#  include <sys/socket.h>
#  include <sys/utsname.h>
# endif  /* TARGET_OS_MAC */
#elif defined(__sun) && defined(__SVR4)  /* Oracle Solaris, Open Indiana */
#  define SYSTEM_INFO_SOLARIS_INDIANA
/* [WIP] not yet checked */
#else   /* NULL */
/* Nothing */
#endif  /* NULL */

typedef struct MacAddrWithHash {
  unsigned int mac_hash;
  struct MacAddrWithHash *p_next;
} MacAddrWithHash;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief 
 * - a function that return the network node hostname
 */
const char *GetMachineName();

/**
 * @brief 
 * - a function that return the hash of the CPU ID
 */
unsigned short GetCpuHash();

#ifdef SYSTEM_INFO_LINUX
/**
 * @brief 
 * - a function that store the result of the `cpuid` instruction with eax 0 
 *   at the address pointed to by p
 * 
 * - parameter ax is used as an initial value for using the `cpuid` instruction
 * - https://en.wikipedia.org/wiki/CPUID#EAX=0:_Highest_Function_Parameter_and_Manufacturer_ID
 */
static void GetCpuid(unsigned int *p, unsigned int ax);
#endif  /* SYSTEM_INFO_LINUX */

/**
 * @brief 
 * - a function that return a name of platform, if determined, otherwise - an empty string
 * 
 * [NOTICE] Windows dosen't have a 'volume serial number', 
 *          so this function will use the system name instead and hash it.
 */
unsigned short GetVolumeHash();

/**
 * @brief 
 * - a function that return the list of the pair <MAC, MAC_HASH>
 * 
 * [NOTICE] Have to free this list manually
 */
MacAddrWithHash *GetMacHash();

/**
 * @brief 
 * - a function that return the hash of the mac_addr
 */
unsigned short HashMacAddress(unsigned char *mac_addr);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* LIB_MAC_H_ */
