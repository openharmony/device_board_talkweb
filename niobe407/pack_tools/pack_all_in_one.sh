# Copyright (c) 2021-2022 Talkweb Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/bin/bash

hb_env_str=`hb env | grep "\[OHOS INFO\] root path"`
root_path=${hb_env_str##* }

hb_env_str=`hb env | grep "\[OHOS INFO\] board"`
board_name=${hb_env_str##* }

#芯片类型
CHIP_TYPE="STM32F407IGT6"

#获取硬件版本
hw_ver_str=`grep "HW_VERSION" $root_path/device/board/talkweb/$board_name/sdk/bsp/include/program_version.h`
HW_VER=`echo ${hw_ver_str##* } | sed 's/\"//g'`

#获取软件版本
soft_ver_str=`grep "APP_VERSION" $root_path/device/board/talkweb/$board_name/sdk/bsp/include/program_version.h`
SOFT_VER=`echo ${soft_ver_str##* } | sed 's/\"//g'`

MERGE_TOOL_PATH=$root_path/device/board/talkweb/$board_name/sdk/tools/merge_bin/merge_bin
BOOT_LOADER_PATH=$root_path/device/board/talkweb/$board_name/sdk/bootloader/tw_boot.bin
APP_PATH=$root_path/out/$board_name/$board_name/OHOS_Image.bin
OUTPUT_ALLINONE_PATH=$root_path/out/$board_name/$board_name/OHOS_Image_ALLINONE.bin

SIGN_TOOL_PATH=$root_path/device/board/talkweb/$board_name/sdk/tools/ota_sign/ota_sign
OUTPUT_OTA_PATH=$root_path/out/$board_name/$board_name/OHOS_OTA.bin

#合并bootloader程序
$MERGE_TOOL_PATH $BOOT_LOADER_PATH $APP_PATH $OUTPUT_ALLINONE_PATH

#生成OTA程序
$SIGN_TOOL_PATH $APP_PATH $OUTPUT_OTA_PATH $CHIP_TYPE $HW_VER $SOFT_VER
