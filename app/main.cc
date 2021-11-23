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

/*
#include <iostream>
#include "platform.h"  // [NOLINT]

int main(void) {
  std::cout << GetPlatformName() << std::endl;

  return 0;
}

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
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
#include <assert.h>

#ifdef DARWIN
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <net/if_types.h>
#else  // !DARWIN
#include <linux/if.h>
#include <linux/sockios.h>
#endif  // !DARWIN

typedef struct MacAddrWithHash {
  char *p_mac;
  unsigned int hash_mac;
  struct MacAddrWithHash *p_next;
} MacAddrWithHash;

const char *GetMachineName();

unsigned short GetCpuHash();

#ifndef DARWIN
static void GetCpuId(unsigned int *, unsigned int);
#endif  /* !DARWIN */

unsigned short GetVolumeHash();

MacAddrWithHash *GetMacHash();

unsigned short HashMacAddress(unsigned char *);

MacAddrWithHash *GetMacHash()
{
  MacAddrWithHash *p_head = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));
  MacAddrWithHash *p_node = (MacAddrWithHash *) malloc(sizeof(MacAddrWithHash));

  p_head->p_next = p_node;

#ifdef DARWIN
  struct ifaddrs *ifaphead;
  struct ifaddrs *ifap;

  if (getifaddrs(&ifaphead) != 0) {
    assert(0);  /* Shouldn't throw an exception */

    return;
  }

  /* iterate over the net interfaces */
  for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
    struct sockaddr_dl *sdl = (struct sockaddr_dl *) ifap->ifa_addr;
    /*
    AF_LINK: the link layer(Ethernet) interface
    IFT_ETHER: Ethernet I or II (also known as DIX Ethernet)
    */
    if (sdl && (sdl->sdl_family == AF_LINK) && (sdl->sdl_type == IFT_ETHER)) {
      /** TODO
      mac1 = HashMacAddress( (unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );    
      */
    }
  }
  freeifaddrs(ifaphead);
#else  /* !DARWIN */
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  char ifconfbuf[128 * sizeof(struct ifreq)];
  struct ifconf conf;
  struct ifreq *ifr;

  if (sock < 0) {
    assert(0);  /* Shouldn't throw an exception */

    return;
  }

  /* enumerate all IP addresses of the system */
  memset(ifconfbuf, 0, sizeof(ifconfbuf));
  conf.ifc_buf = ifconfbuf;
  conf.ifc_len = sizeof(ifconfbuf);
  if (ioctl(sock, SIOCGIFCONF, &conf)) {
    assert(0);  /* Shouldn't throw an exception */

    return;
  }

  /* get MAC address */
  for (ifr = conf.ifc_req; (char *) ifr < (char *) conf.ifc_req + conf.ifc_len; ++ifr) {
   if ( ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data )     
     continue; // duplicate, skip it   

   if ( ioctl( sock, SIOCGIFFLAGS, ifr ))      
     continue; // failed to get flags, skip it  
   if ( ioctl( sock, SIOCGIFHWADDR, ifr ) == 0 )  
   {  
     if ( !foundMac1 )  
     { 
      foundMac1 = true;         
      mac1 = HashMacAddress( (unsigned char*)&(ifr->ifr_addr.sa_data));    
     } else {      
      mac2 = HashMacAddress( (unsigned char*)&(ifr->ifr_addr.sa_data));    
      break;      
     } 
   }  
  }    

  close( sock );      

#endif // !DARWIN      

  // sort the mac addresses. We don't want to invalidate        
  // both macs if they just change order.  
  if ( mac1 > mac2 )    
  {    
   unsigned short tmp = mac2;    
   mac2 = mac1;      
   mac1 = tmp;      
  }    

  return p_head;
}

unsigned short HashMacAddress(unsigned char* mac) {
  unsigned short hash = 0;

  for (unsigned int i = 0; i < 6; i++) {
    hash += ( mac[i] << (( i & 1 ) * 8 ));
  }

  return hash;
}

/* TODO 
unsigned short HashMacAddress(unsigned char* mac) {
  unsigned short hash = 0;

  for (unsigned int i = 0; i < 6; i++) {
    hash += ( mac[i] << (( i & 1 ) * 8 ));
  }

  return hash;
}

void GetMacHash( unsigned short& mac1, unsigned short& mac2 )    
{ 
  mac1 = 0;         
  mac2 = 0;         

#ifdef DARWIN        

  struct ifaddrs* ifaphead; 
  if ( getifaddrs( &ifaphead ) != 0 )    
   return;        

  // iterate over the net interfaces     
  bool foundMac1 = false;  
  struct ifaddrs* ifap;   
  for ( ifap = ifaphead; ifap; ifap = ifap->ifa_next )         
  {    
   struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifap->ifa_addr;   
   // AF_LINK: the link layer(Ethernet) interface
   // IFT_ETHER: Ethernet I or II (also known as DIX Ethernet)
   if ( sdl && ( sdl->sdl_family == AF_LINK ) && ( sdl->sdl_type == IFT_ETHER ))         
   {  
     if ( !foundMac1 ) 
     {         
       foundMac1 = true;        
       mac1 = HashMacAddress( (unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );    
     } else {      
       mac2 = HashMacAddress( (unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );    
       break;     
     }         
   }  
  }    

  freeifaddrs( ifaphead ); 

#else // !DARWIN       

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP );         
  if ( sock < 0 ) return;  

  // enumerate all IP addresses of the system     
  struct ifconf conf;    
  char ifconfbuf[ 128 * sizeof(struct ifreq) ];   
  memset( ifconfbuf, 0, sizeof( ifconfbuf ));     
  conf.ifc_buf = ifconfbuf; 
  conf.ifc_len = sizeof( ifconfbuf );    
  if ( ioctl( sock, SIOCGIFCONF, &conf ))  
  {    
   assert(0);       
   return;        
  }    

  // get MAC address    
  bool foundMac1 = false;  
  struct ifreq* ifr;    
  for ( ifr = conf.ifc_req; (char*)ifr < (char*)conf.ifc_req + conf.ifc_len; ifr++ ) 
  {    
   if ( ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data )     
     continue; // duplicate, skip it   

   if ( ioctl( sock, SIOCGIFFLAGS, ifr ))      
     continue; // failed to get flags, skip it  
   if ( ioctl( sock, SIOCGIFHWADDR, ifr ) == 0 )  
   {  
     if ( !foundMac1 )  
     { 
      foundMac1 = true;         
      mac1 = HashMacAddress( (unsigned char*)&(ifr->ifr_addr.sa_data));    
     } else {      
      mac2 = HashMacAddress( (unsigned char*)&(ifr->ifr_addr.sa_data));    
      break;      
     } 
   }  
  }    

  close( sock );      

#endif // !DARWIN      

  // sort the mac addresses. We don't want to invalidate        
  // both macs if they just change order.  
  if ( mac1 > mac2 )    
  {    
   unsigned short tmp = mac2;    
   mac2 = mac1;      
   mac1 = tmp;      
  }    
} 
*/

int main() {
  printf("Machine: %s\n", GetMachineName());
  printf("CPU: %d\n", GetCpuHash());
  printf("Volume: %d\n", GetVolumeHash());

  return 0;
}

/* get the network node hostname */
const char *GetMachineName()
{
  static struct utsname u;

  if (uname(&u) < 0) {
    assert(0);  /* Shouldn't throw an exception */

    return "unknown";
  }

  return u.nodename;
}

#ifdef DARWIN
#include <mach-o/arch.h>

unsigned short GetCpuHash()
{
  const NXArchInfo *cpu_info = NXGetLocalArchInfo();
  unsigned short hash = 0;
  hash += (unsigned short) cpu_info->cputype;
  hash += (unsigned short) cpu_info->cpusubtype;

  return hash;
}
#else   // !DARWIN
unsigned short GetCpuHash()
{
  unsigned int cpu_info[4] = { 0, 0, 0, 0 };
  unsigned int *ptr = cpu_info;
  unsigned short hash = 0;

  GetCpuId(cpu_info, 0);
  for (unsigned int i = 0; i < 4; ++i) {
    hash += *ptr & 0xFFFF;
    hash += *ptr >> 16;
    ++ptr;
  }

  ptr = cpu_info;
  printf("CPU Origin.: [ ");
  for (unsigned int i = 0; i < 4; ++i) {
    printf("%08X%s", *ptr++, (i == 3) ? "" : " : ");
  }
  puts(" ]");

  return hash;
}

static void GetCpuId(unsigned int *p, unsigned int ax)
{
  /* ebx: call-saved register, must save and later restore it */
  __asm__ __volatile__("movl %%ebx, %%esi\n\t"
                       "cpuid\n\t"
                       "xchgl %%ebx, %%esi"
                       : "=a" (p[0]), "=S" (p[1]),
                         "=c" (p[2]), "=d" (p[3])
                       : "0" (ax));
}
#endif  // !DARWIN

unsigned short GetVolumeHash() {
  /* Windows dosen't have a 'volume serial number', 
     so we use the system name instead and hash it. */
  unsigned char* sysname = (unsigned char*) GetMachineName();
  unsigned short hash = 0;

  for (unsigned int i = 0; sysname[i]; ++i) {
    hash += (sysname[i] << ((i & 1) * 8));
  }

  printf("Volume Origin.: [ %s ]\n", sysname);

  return hash;
}
