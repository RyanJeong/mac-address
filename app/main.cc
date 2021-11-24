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

#include <iostream>
#include <string>

#include "platform.h"  // [NOLINT]
#include "system_info.h"  // [NOLINT]

int main(void) {
  const std::string platform_name = GetPlatformName();
  std::cout << "Platform: "
            << platform_name << std::endl;

  const std::string machine_name = GetMachineName();
  std::cout << "Machine Hash: "
            << machine_name << std::endl;

  const uint16_t cpu_hash = GetCpuHash();
  std::cout << "CPU Hash: "
            << cpu_hash << std::endl;

  const uint16_t volume_hash = GetVolumeHash();
  std::cout << "Volume Hash: "
            << volume_hash << std::endl;

  MacAddrWithHash *list_mac_hash = GetMacHash();
  MacAddrWithHash *it = list_mac_hash;
  for (int i = 1; it = it->p_next; ++i) {
    std::cout << "MAC Address #" << i << " Hash: "
              << it->mac_hash << '\n';
  }

  // Free list_mac_hash manually
  while (list_mac_hash) {
    MacAddrWithHash *p_tmp = list_mac_hash->p_next;

    free(list_mac_hash);
    list_mac_hash = p_tmp;
  }

  return 0;
}
