# TermBright

Termbright is a program that changes the brightness of the screen in a simple way from the terminal

## How to use it

set brightness at 50%
```sh
$ termbright 50%
```

adds 10% brightness to current brightness
```sh
$ termbright +10%
```

removes 10% brightness at current brightness
```sh
$ termbright +10%
```

## Compile

### Without capabilities (requires root permissions to run the program)

for compile and put the binary in ./bin/termbright
```sh
$ chmod +x makefile.py
$ ./makefile.py
```

for compile and put the binary in /usr/bin/termbright
```sh
$ chmod +x makefile.py
$ sudo ./makefile.py install
```

### With capabilities (does not require root permissions to run the program)

for compile and put the binary in ./bin/termbright
```sh
$ chmod +x makefile.py
$ ./makefile.py -c
```

for compile and put the binary in /usr/bin/termbright
```sh
$ chmod +x makefile.py
$ sudo ./makefile.py install -c
```

## Credits
[the location of the folder where the brightness files are located](https://askubuntu.com/a/56168)

[list of capabilities in linux](https://man7.org/linux/man-pages/man7/capabilities.7.html)