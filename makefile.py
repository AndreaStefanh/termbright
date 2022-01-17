#!/usr/bin/python3
# -*- coding: utf-8 -*-
import shutil
import stat
import sys
import os

DEBUG = False


CC = "gcc"

if DEBUG == True:
    CFLAGS = "-x c -std=gnu18 -c -O0 -I include -Wall -Wextra -pedantic -ggdb"
else:
    CFLAGS = "-x c -std=gnu18 -c -O0 -I include -Wall -Wextra -pedantic -s"

SRC = [
    "src/main.c",
]

OBJ = [
    "obj/main.o",
]

DIR = [
    "obj",
    "bin",
]


def createDir() -> None:

    for i in range(0, len(DIR)):
        os.makedirs(DIR[i], exist_ok=True)
    
    return

def src2obj() -> None:

    for i in range(0, len(SRC)):
        os.system(f"{CC} {CFLAGS} {SRC[i]} -o {OBJ[i]}")
    
    return

def obj2bin() -> None:

    obj = " ".join(OBJ)
    os.system(f"{CC} {obj} -o bin/termbright")
    return

def args() -> None:

    if len(sys.argv) == 1:
        return

    if sys.argv[1] == "-r" or sys.argv[1] == "--run":

        buffer = "sudo bin/termbright "

        for i in range(2, len(sys.argv)):
            buffer = buffer + sys.argv[i]
        
        os.system(buffer)
    
    elif sys.argv[1] == "install":

        if os.getuid() != 0:
            print("For this operation need root permition")
            exit(1)
        
        shutil.copyfile("bin/termbright", "/usr/bin/termbright")

        os.chmod(
            "/usr/bin/termbright",
            stat.S_IXUSR |
            stat.S_IXGRP |
            stat.S_IXOTH |
            stat.S_IRUSR |
            stat.S_IRGRP |
            stat.S_IROTH |
            stat.S_IWUSR |
            stat.S_IWGRP
        )


def main() -> None:
    createDir()
    src2obj()
    obj2bin()
    args()


if __name__ == "__main__":
    main()
