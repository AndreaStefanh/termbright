#!/usr/bin/python3
# -*- coding: utf-8 -*-
import shutil
import stat
import sys
import os

DEBUG = False


CC = "gcc"

if DEBUG == True:
    CFLAGS = "-x c -std=gnu18 -c -O2 -I include -Wall -Wextra -pedantic -ggdb -D_REENTRANT"
else:
    CFLAGS = "-x c -std=gnu18 -c -O2 -I include -Wall -Wextra -pedantic -s -D_REENTRANT"

LIBS = "-lpthread"


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
        print(f"mkdir -p {DIR[i]}")
        os.makedirs(DIR[i], exist_ok=True)
    
    return

def frArgs() -> None:

    if len(sys.argv) == 1:
        return
    
    for i in range(0, len(sys.argv)):

        if sys.argv[i] == "-c" or sys.argv[i] == "--capabilities":
            global CFLAGS
            CFLAGS = CFLAGS + " -DROOT_CHECK"

def src2obj() -> None:

    for i in range(0, len(SRC)):
        print(f"{CC} {CFLAGS} {LIBS} {SRC[i]} -o {OBJ[i]}")
        os.system(f"{CC} {CFLAGS} {LIBS} {SRC[i]} -o {OBJ[i]}")
    
    return

def obj2bin() -> None:

    obj = " ".join(OBJ)
    print(f"{CC} {LIBS} {obj} -o bin/termbright")
    os.system(f"{CC} {LIBS} {obj} -o bin/termbright")
    return

def args() -> None:

    installMode = False

    if len(sys.argv) == 1:
        return
    
    for i in range(0, len(sys.argv)):

        if sys.argv[i] == "-r" or sys.argv[i] == "--run":

            buffer = "sudo bin/termbright "

            for i in range(2, len(sys.argv)):
                buffer = buffer + sys.argv[i]
            
            print(buffer + "\n------------------")
            os.system(buffer)
        
        elif sys.argv[i] == "install":

            if os.getuid() != 0:
                print("[!] For this operation need root permition")
                exit(1)
            
            installMode = True
            
            print("sudo cp bin/termbright", "/usr/bin/termbright")
            shutil.copyfile("bin/termbright", "/usr/bin/termbright")

            print("sudo chmod 555 /usr/bin/termbright")
            os.chmod(
                "/usr/bin/termbright",
                stat.S_IXUSR |
                stat.S_IXGRP |
                stat.S_IXOTH |
                stat.S_IRUSR |
                stat.S_IRGRP |
                stat.S_IROTH
            )
        
        elif sys.argv[i] == "-c" or sys.argv[i] == "--capabilities":

            if os.getuid() != 0:
                print("[!] For this operation need root permition")
                exit(1)
        
            if installMode == True:
                print("sudo setcap CAP_DAC_OVERRIDE=ep /usr/bin/termbright")
                os.system("sudo setcap CAP_DAC_OVERRIDE=ep /usr/bin/termbright")
            else:
                print("sudo setcap CAP_DAC_OVERRIDE=ep bin/termbright")
                os.system("sudo setcap CAP_DAC_OVERRIDE=ep bin/termbright")


def main() -> None:
    createDir()
    frArgs()
    src2obj()
    obj2bin()
    args()


if __name__ == "__main__":
    main()
