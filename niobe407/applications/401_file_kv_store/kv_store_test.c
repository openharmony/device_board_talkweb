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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_run.h"
#include "cmsis_os2.h"


#define wifi1_name_key "talkweb0"
#define wifi1_passwd_value "12345678"
#define wifi2_name_key "talkweb1"
#define wifi2_passwd_value "3456789"

void kvStoreTestEntry(void)
{	
	char temp[128] = {0};
	UtilsSetValue(wifi1_name_key, wifi1_passwd_value);
	printf("[wifi manage] set key = %s, value = %s\n", wifi1_name_key, wifi1_passwd_value);
	UtilsSetValue(wifi2_name_key, wifi2_passwd_value);
	printf("[wifi manage] set key = %s, value = %s\n", wifi2_name_key, wifi2_passwd_value);
	
	UtilsGetValue(wifi1_name_key, temp, 128);
	printf("[wifi manage] get %s passwd = %s\n", wifi1_name_key, temp);
	UtilsGetValue(wifi2_name_key, temp, 128);
	printf("[wifi manage] get %s passwd = %s\n", wifi2_name_key, temp);
	
	UtilsDeleteValue(wifi1_name_key);
	UtilsDeleteValue(wifi2_name_key);
}

OHOS_APP_RUN(kvStoreTestEntry);