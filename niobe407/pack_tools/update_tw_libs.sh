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

#更新系统启动库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_sys_boot.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/$board_name/sdk/bsp/lib/ -rf
else
    echo "Update twlib fail, because of $TW_LIB_PATH is not exist!!"
    exit -1
fi

#更新有线网络lwip适配库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_ethernet_static.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/$board_name/sdk/lwip_adapter/lib/ -rf
fi

#更新deviceauth组件库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_device_auth_static.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/components/deviceauth/lib/ -rf
fi

#更新sntpmanager组件库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_sntp_static.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/components/sntpmanager/lib/ -rf
fi

#更新twlinksdk组件库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_linksdk_static.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/components/twlinksdk/lib/ -rf
fi

#更新twota组件库
TW_LIB_PATH=$root_path/out/$board_name/$board_name/libs/libtw_ota_static.a
if [ -f "$TW_LIB_PATH" ]; then
    cp $TW_LIB_PATH $root_path/device/board/talkweb/components/twota/lib/ -rf
fi
