/*
 * Copyright (c) 2021-2022 Talkweb Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdbool.h>
#include "uart.h"
#include "watch_dog.h"
#include "devmgr_service_start.h"
#include "hiview_def.h"
#include "hiview_output_log.h"

extern int DeviceManagerStart(void); // hdf init


bool HilogProc_Impl(const HiLogContent *hilogContent, uint32_t len)
{
  char tempOutStr[LOG_FMT_MAX_LEN];
  tempOutStr[0] = 0,tempOutStr[1] = 0;
  if (LogContentFmt(tempOutStr, sizeof(tempOutStr), hilogContent) > 0) {
    printf(tempOutStr);
  }
  return true;
}

int HiLogWriteInternal(const char *buffer, size_t bufLen)
{
  if (!buffer)
    return -1;
  if (bufLen < 2)
    return 0;
  if (buffer[bufLen - 2] != '\n') {
    *((char *)buffer + bufLen - 1) = '\n';
  }
  printf("%s\n",buffer);
  return 0;
}

void sys_service_config()
{
    HiviewRegisterHilogProc(HilogProc_Impl);
    
#ifdef LOSCFG_WATCH_DOG
    watch_dog_init(1100);
#endif

#ifdef LOSCFG_DRIVERS_HDF
    DeviceManagerStart();
#endif

#ifdef LOSCFG_SHELL
    ShellUartInit();
#endif

}