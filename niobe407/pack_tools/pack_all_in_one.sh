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

BOOT_LOADER_PATH=$root_path/out/$board_name/$board_name/bin/tw_boot.bin
MERGE_TOOL_PATH=$root_path/out/$board_name/$board_name/bin/merge_bin
APP_PATH=$root_path/out/$board_name/$board_name/OHOS_Image.bin
OUTPUT_ALLINONE_PATH=$root_path/out/$board_name/$board_name/OHOS_Image_allinone.bin

#合并bootloader程序
$MERGE_TOOL_PATH $BOOT_LOADER_PATH $APP_PATH $OUTPUT_ALLINONE_PATH
rm $APP_PATH
mv $OUTPUT_ALLINONE_PATH $APP_PATH
