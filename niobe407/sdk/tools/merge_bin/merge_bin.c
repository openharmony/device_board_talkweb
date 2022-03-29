/*
 * Copyright (c) 2022 Talkweb Co., Ltd.
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
#include <stdlib.h>
#include <string.h>

#define BOOTLOADER_OFFSET_SIZE  0x10000         // bootloader offset size
#define MAX_BUFFER_SIZE         1024
#define CHAR_RANG               256
#define ARGV_0      0
#define ARGV_1      1
#define ARGV_2      2
#define ARGV_3      3
#define INPUT_ARGC_NUM      4
#define FILL_CHAR           0xFF

FILE *g_fp_out;
FILE *g_fp_tmp;
int g_fill_size;
int g_buffer_len;
unsigned char g_buffer[MAX_BUFFER_SIZE];

static void Usage(void)
{
    printf("Params error:\r\nFor usage example: ./merge_bin bootload.bin OHOS_Image.bin output.bin\r\n");
}

static int MergeFail(const char *output_file)
{
    if (g_fp_out) {
        fclose(g_fp_out);
        g_fp_out = NULL;
    }
    if (g_fp_tmp) {
        fclose(g_fp_tmp);
        g_fp_tmp = NULL;
    }
    if (output_file != NULL) {
        char cmd[256] = {"rm -rf "};
        int cmd_len = strlen(output_file);
        for (int i = 0; i<cmd_len; i++) {
            cmd[i+cmd_len-1] = output_file[i];
        }
        system(cmd);
    }
    return -1;
}

static int CopyBootloader(const char *bootloader_filename, const char *output_filename)
{
    int ret;
    g_fp_tmp = fopen(bootloader_filename, "r");
    if (g_fp_tmp == NULL) {
        printf("merge_bin fail! because open %s fail!\r\n", bootloader_filename);
        fclose(g_fp_out);
        g_fp_out = NULL;
        return -1;
    }
    g_fill_size = BOOTLOADER_OFFSET_SIZE;
    do {
        g_buffer_len = fread(g_buffer, 1, sizeof(g_buffer), g_fp_tmp);     // Read from bootloader file
        ret = fwrite(g_buffer, 1, g_buffer_len, g_fp_out);                 // Write to output file
        if (ret != g_buffer_len) {
            printf("merge_bin fail! because write %s fail!\r\n", output_filename);
            return MergeFail(output_filename);
        }
        g_fill_size -= g_buffer_len;
    } while (g_buffer_len>0);
    if (g_fill_size<0) {
        printf("merge_bin fail! because %s is too large more than 0x%02X!\r\n",
               bootloader_filename, BOOTLOADER_OFFSET_SIZE);
        return MergeFail(output_filename);
    }
    return 0;
}

static int FillEmpty(char *output_filename)
{
    int ret;
    unsigned char fill_ch = FILL_CHAR;
    for (int i = 0; i<g_fill_size; i++) {
        ret = fwrite(&fill_ch, 1, 1, g_fp_out);
        if (ret != 1) {
            printf("merge_bin fail! because write fillchar fail!\r\n");
            return MergeFail(output_filename);
        }
    }
    return 0;
}

static int CopyApp(const char *app_filename, const char *output_filename)
{
    int ret;
    if (g_fp_tmp != NULL) {
        fclose(g_fp_tmp);
        g_fp_tmp = NULL;
    }
    g_fp_tmp = fopen(app_filename, "r");
    if (g_fp_tmp == NULL) {
        printf("merge_bin fail! because open %s fail!\r\n", app_filename);
        return MergeFail(output_filename);
    }
    do {
        g_buffer_len = fread(g_buffer, 1, sizeof(g_buffer), g_fp_tmp);     // Read from app file
        ret = fwrite(g_buffer, 1, g_buffer_len, g_fp_out);                 // Write to output file
        if (ret != g_buffer_len) {
            printf("merge_bin fail! because write %s fail!\r\n", output_filename);
            return MergeFail(output_filename);
        }
    } while (g_buffer_len > 0);
}

int main(int argc, char *argv[])
{
    int ret;
    if (argc != INPUT_ARGC_NUM) {
        Usage();
        return 0;
    }
    g_fp_out = fopen(argv[ARGV_3], "w+");
    if (g_fp_out == NULL) {
        printf("merge_bin fail! because open %s fail!\r\n", argv[ARGV_3]);
        return -1;
    }
    if (CopyBootloader(argv[ARGV_1], argv[ARGV_3]) != 0) {  // merge bootloader
        return -1;
    }
    if (FillEmpty(argv[ARGV_3])!=0) {                       // fill empty size
        return -1;
    }
    if (CopyApp(argv[ARGV_2], argv[ARGV_3])!=0) {           // merge app
        return -1;
    }
    if (g_fp_out) {
        fclose(g_fp_out);
        g_fp_out = NULL;
    }
    if (g_fp_tmp) {
        fclose(g_fp_tmp);
        g_fp_tmp = NULL;
    }
    printf("merge_bin to %s success!\r\n", argv[ARGV_3]);
    return 0;
}