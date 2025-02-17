/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

#include <stdlib.h>
#include "cli.h"
#include "sdhc.h"
#include "user-cli.h"

const command_t user_commands[] = {
    { "size", "size", "Find the Size of the SD Card and Free Space", handle_size },
    { "format", "format", "Format the Card", handle_format },
    { "mount", "mount", "Manually Mount Card", handle_mount },
    { "ls", "ls", "list the contents of the current directory", handle_ls },
    { "mkdir", "mkdir <directory name>", "Create a directory", handle_mkdir },
    { "file_create", "file_create <file name> <number of bytes to add>",
      "Create a file of random data", handle_createfile },
    { "cd", "cd <directory name>", "Move into a directory", handle_cd },
    { "add_data", "add_data <file name> <number of bytes to add>",
      "Add random Data to an Existing File", handle_add_data },
    { "del", "del <file name>", "Delete a file", handle_del },
    { "fatfs", "fatfs", "Format Card and Run Example of FatFS Operations", handle_fatfs },
    { "unmount", "unmount", "Unmount card", handle_unmount },
};

const unsigned int num_user_commands = sizeof(user_commands) / sizeof(command_t);

int handle_size(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return getSize();
}

int handle_format(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return formatSDHC();
}

int handle_mount(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return mount();
}

int handle_ls(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return ls();
}

int handle_mkdir(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect usage. Please provide directory name.\n");
        return E_INVALID;
    }

    return mkdir(argv[1]);
}

int handle_createfile(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Incorrect usage. Please provide filename and length.\n");
        return E_INVALID;
    }

    unsigned int length = atoi(argv[2]);
    return createFile(argv[1], length);
}

int handle_cd(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect usage. Please provide directory name.\n");
        return E_INVALID;
    }

    return cd(argv[1]);
}

int handle_add_data(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Incorrect usage. Please provide filename and length.\n");
        return E_INVALID;
    }

    unsigned int length = atoi(argv[2]);
    return appendFile(argv[1], length);
}

int handle_del(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect usage. Please provide filename.\n");
        return E_INVALID;
    }

    return deleteFile(argv[1]);
}

int handle_fatfs(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return example();
}

int handle_unmount(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Incorrect usage. Too many parameters.\n");
        return E_INVALID;
    }

    return umount();
}
