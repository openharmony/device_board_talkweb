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

#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mount.h>
#include "los_config.h"
#include "los_memory.h"
#include "los_task.h"
#include "ohos_run.h"

static void dir_test(const char *path)
{
    DIR *dir;
    int ret ;
    struct dirent *dp;
    if ((dir = opendir(path)) == NULL) {
        printf("opendir %s failed \n", path);
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        struct stat st_buf = {0};
        char realpath[260];
        snprintf_s(realpath, sizeof(realpath), "%s/%s", path, dp->d_name);
        if (stat(realpath, &st_buf) != 0) {
            printf("can not access %s\n", realpath);
            closedir(dir);
            return;
        }
        if ((st_buf.st_mode & S_IFMT) == S_IFDIR) {
            printf("DIR %s\n", realpath);
        } else {
            printf("FILE %s, %ld bytes\n", realpath, st_buf.st_size);
        }
    }
    closedir(dir);

    ret = mkdir("/talkweb/fstestdir", S_IRUSR | S_IWUSR);
    if (ret) {
        printf("mkdir failed \n");
        return;
    }
    ret = rmdir("/talkweb/fstestdir");
    if (ret) {
        printf("rmdir failed \n");
        return;
    }
    printf("%s ok \n", __func__);
}

static void read_test(const char *file, bool print_str)
{
    int fd = _open(file, O_RDWR | O_CREAT);
    if (fd < 0) {
        printf("open file '%s' failed, \r\n", file);
        return;
    }
    int bytes = 0;
    char buf[513];
    while (1) {
        memset_s(buf, sizeof(buf), 0, sizeof(buf));
        int rc = _read(fd, buf, sizeof(buf) - 1);
        if (rc > 0) {
            bytes += rc;
        }

        if (print_str) {
            buf[rc] = '\0';
            printf("%s\r\n", buf);
        }

        if (rc < sizeof(buf) - 1) {
            break;
        }
    }
    _close(fd);
    printf("read file '%s' total bytes: %d\r\n", file, bytes);
    printf("%s ok \n", __func__);
}

static void write_test(const char *file, const char *data)
{
    int fd = _open(file, O_RDWR | O_CREAT);
    if (fd < 0) {
        printf("open file '%s' failed \r\n", file);
        return;
    }
    int bytes = _write(fd, data, strlen(data));
    _close(fd);
    printf("write file '%s' total bytes: %d, %s\r\n", file, bytes, data);
    printf("%s ok \n", __func__);
}

void littlefs_test(void)
{
    printf("%s\r\n", __func__);
    dir_test("/talkweb");
    read_test("/talkweb/wifi.cfg", true);
    write_test("/talkweb/wifi.cfg", "ssid:talkweb password:123456");
    read_test("/talkweb/wifi.cfg", true);
}
OHOS_APP_RUN(littlefs_test);