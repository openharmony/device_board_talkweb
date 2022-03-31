# Copyright (c) 2022 Talkweb Co., Ltd.
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

MERGE_BIN_PATH=../../../../../../../out/niobe407/niobe407/bin
MERGE_BIN=$(MERGE_BIN_PATH)/merge_bin
CC=gcc
INCLUDE :=-I ./
LIBS :=-L../lib
OBJ=$(patsubst %.c,%.o,$(wildcard *.c))

$(MERGE_BIN):$(OBJ)
	mkdir -p $(MERGE_BIN_PATH)
	$(CC) -o $@ $^ $(LIBS)
	rm *.o -rf
%.o:%.c
	$(CC) -c $^ -o  $@ 	$(INCLUDE)
clean:
	rm $(OBJ) -rf
