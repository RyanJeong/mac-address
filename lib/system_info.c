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

#include <stdio.h>

#include "system_info.h"

const char *GetMachineName()
{
  static struct utsname u;

  if (uname(&u) < 0) {
    assert(0);  /* Shouldn't throw an exception */

    return "unknown";
  }

  return u.nodename;
}

#if defined(SYSTEM_INFO_MAC)
# include <mach-o/arch.h>

unsigned short GetCpuHash()
{
  const NXArchInfo *cpu_info = NXGetLocalArchInfo();

  printf("Processor Info: type: %u, subtype: %u, "
         "description: %s, name: %s\n", cpu_info->cputype,
                                        cpu_info->cpusubtype,
                                        cpu_info->description,
                                        cpu_info->name);
  unsigned short hash = 0;
  hash += (unsigned short) cpu_info->cputype;
  hash += (unsigned short) cpu_info->cpusubtype;

  return hash;
}
#elif defined(SYSTEM_INFO_LINUX)
unsigned short GetCpuHash()
{
  unsigned int cpu_info[4] = { 0, 0, 0, 0 };
  unsigned int *ptr = cpu_info;
  unsigned short hash = 0;

  GetCpuid(cpu_info, 0);
  for (int i = 0; i < 4; ++i) {
    hash += *ptr & 0xFFFF;
    hash += *ptr >> 16;
    ++ptr;
  }

# ifndef NDEBUG
  printf("Processor manufacturer ID Strings: ");

  /* EBX */
  for (int i = 0; i < 4; ++i) {
    unsigned int tmp = cpu_info[1] & (0xFF << (8 * i));
    tmp >>= (8 * i);
    putchar((char) tmp);
  }

  /* EDX */
  for (int i = 0; i < 4; ++i) {
    unsigned int tmp = cpu_info[3] & (0xFF << (8 * i));
    tmp >>= (8 * i);
    putchar((char) tmp);
  }

  /* ECX */
  for (int i = 0; i < 4; ++i) {
    unsigned int tmp = cpu_info[2] & (0xFF << (8 * i));
    tmp >>= (8 * i);
    putchar((char) tmp);
  }
  putchar('\n');

  ptr = cpu_info;
  printf("CPU: [ ");
  for (int i = 0; i < 4; ++i) {
    printf("%08X%s", *ptr++, (i == 3) ? "" : " : ");
  }
  puts(" ]");
# endif  /* NDEBUG */

  return hash;
}

static void GetCpuid(unsigned int *p, unsigned int ax)
{
  /* ebx: call-saved register, must save and later restore it */
  __asm__ __volatile__("movl %%ebx, %%esi\n\t"
                       "cpuid\n\t"
                       "xchgl %%ebx, %%esi"
                       : "=a" (p[0]), "=S" (p[1]),
                         "=c" (p[2]), "=d" (p[3])
                       : "0" (ax));
}
#endif  /* SYSTEM_INFO_LINUX */

unsigned short GetVolumeHash()
{
  unsigned char *sysname = (unsigned char *) GetMachineName();
  unsigned short hash = 0;

  for (int i = 0; sysname[i]; ++i) {
    hash += (sysname[i] << ((i & 1) * 8));
  }

#ifndef NDEBUG
  printf("Volume: [ %s ]\n", sysname);
#endif  /* NDEBUG */

  return hash;
}

MacAddrWithHash *GetMacHash()
{
#if defined(SYSTEM_INFO_MAC)
  struct ifaddrs *ifaphead;

  if (getifaddrs(&ifaphead) != 0) {
    assert(0);  /* Shouldn't throw an exception */

    return NULL;
  }
  MacAddrWithHash *p_head;
  MacAddrWithHash *p_tail;

  p_head = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));
  p_head->p_next = NULL;
  p_tail = p_head;
  struct ifaddrs *ifap;
  for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
    struct sockaddr_dl *sdl = (struct sockaddr_dl *) ifap->ifa_addr;
    if (!sdl) {
      continue;
    }

    // the length for an Ethernet address is six
    if (sdl->sdl_alen != 6) {
      continue;
    }

    // AF_LINK: the link layer(Ethernet) interface
    if (sdl->sdl_family != AF_LINK) {
      continue;
    }

    // IFT_ETHER: Ethernet I or II (also known as DIX Ethernet)
    if (sdl->sdl_type != IFT_ETHER) {
      continue;
    }

    // get device 'en*' only
    if (*(sdl->sdl_data) != 'e' || *(sdl->sdl_data + 1) != 'n') {
      continue;
    }
# ifndef NDEBUG
#  ifdef NDEBUG
    printf("sdl_alen: %u\n", sdl->sdl_alen);
    printf("sdl_index: %u\n", sdl->sdl_index);
    printf("sdl_type: %u\n", sdl->sdl_type);
    printf("sdl_len: %u\n", sdl->sdl_len);
    printf("sdl_nlen: %u\n", sdl->sdl_nlen);
    int nlen = sdl->sdl_nlen;
    for (int i = 0; i < sdl->sdl_len; ++i) {
      if (nlen > 0) {
        putchar(sdl->sdl_data[i]);
        --nlen;
      } else {
        printf(" %x", sdl->sdl_data[i]);
      }
    }
    putchar('\n');
#  endif  /* NDEBUG */
# endif  /* NDEBUG */

    MacAddrWithHash *p_tmp;

    p_tmp = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));
    p_tmp->p_next = NULL;
# ifndef NDEBUG
    static int cnt = 1;
    unsigned char mac_addr[6];
    memcpy(mac_addr, LLADDR(sdl), 6);
    printf("MAC Address #%d: ", cnt++);
    for (int i = 0; i < 6; ++i) {
      printf("%02X%s", mac_addr[i], ((i == 5) ? "" : ":"));
    }
    putchar('\n');
# endif  /* NDEBUG */
    p_tmp->mac_hash = HashMacAddress(
        (unsigned char *) LLADDR(sdl));
    p_tail->p_next = p_tmp;
    p_tail = p_tmp;
  }
  freeifaddrs(ifaphead);
#elif defined(SYSTEM_INFO_LINUX)
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  if (sock < 0) {
    assert(0);  /* Shouldn't throw an exception */

    return NULL;
  }

  /* enumerate all IP addresses of the system */
  char ifconfbuf[128 * sizeof(struct ifreq)];
  memset(ifconfbuf, 0, sizeof(ifconfbuf));

  struct ifconf conf;
  conf.ifc_buf = ifconfbuf;
  conf.ifc_len = sizeof(ifconfbuf);
  if (ioctl(sock, SIOCGIFCONF, &conf)) {
    assert(0);  /* Shouldn't throw an exception */

    return NULL;
  }

  /* get MAC address */
  MacAddrWithHash *p_head;
  MacAddrWithHash *p_tail;

  p_head = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));
  p_head->p_next = NULL;
  p_tail = p_head;
  struct ifreq *ifr;
  for (ifr = conf.ifc_req;
      (char *) ifr < (char *) conf.ifc_req + conf.ifc_len;
      ++ifr) {
    /* duplicate, skip it */
    if (ifr->ifr_addr.sa_data == (ifr + 1)->ifr_addr.sa_data) {
      continue;
    }

    /* failed to get flags, skip it  */
    if (ioctl(sock, SIOCGIFFLAGS, ifr)) {
      continue;
    }

    /* failed to get hardware address, skip it */
    if (ioctl(sock, SIOCGIFHWADDR, ifr)) {
      continue;
    }

    MacAddrWithHash *p_tmp;

    p_tmp = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));
    p_tmp->p_next = NULL;
# ifndef NDEBUG
    static int cnt = 1;
    unsigned char mac_addr[6];
    memcpy(mac_addr, &(ifr->ifr_addr.sa_data), 6);
    printf("MAC Address #%d: ", cnt++);
    for (int i = 0; i < 6; ++i) {
      printf("%02X%s", mac_addr[i], ((i == 5) ? "" : ":"));
    }
    putchar('\n');
# endif  /* NDEBUG */
    p_tmp->mac_hash = HashMacAddress(
        (unsigned char *) &(ifr->ifr_addr.sa_data));
    p_tail->p_next = p_tmp;
    p_tail = p_tmp;
  }
  close(sock);
#endif  /* SYSTEM_INFO_LINUX */

  return p_head;
}

unsigned short HashMacAddress(unsigned char *mac_addr)
{
  unsigned short hash = 0;

  for (unsigned int i = 0; i < 6; i++) {
    hash += (mac_addr[i] << ((i & 1) * 8));
  }

  return hash;
}
