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
#include <errno.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#ifdef DARWIN
# include <net/if_dl.h>
# include <ifaddrs.h>
# include <net/if_types.h>
#else  /* !DARWIN */
# include <linux/if.h>
# include <linux/sockios.h>
#endif  /* !DARWIN */

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

#ifndef DARWIN
/**
 * @brief 
 * - a function that store the result of the `cpuid` instruction with eax 0 
 *   at the address pointed to by p
 * 
 * - parameter ax is used as an initial value for using the `cpuid` instruction
 * - https://en.wikipedia.org/wiki/CPUID#EAX=0:_Highest_Function_Parameter_and_Manufacturer_ID
 */
static void GetCpuid(unsigned int *p, unsigned int ax);
#endif  /* !DARWIN */

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
