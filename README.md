# TermBright

Termbright is a program that changes the brightness of the screen in a simple way from the terminal

## How to use it

set brightness at 50%
```sh
sudo termbright 50%
```

adds 10% brightness to current brightness
```sh
sudo termbright +10%
```

removes 10% brightness at current brightness
```sh
sudo termbright +10%
```

## Compile

for compile and put the binary in ./bin/termbright
```sh
chmod +x makefile.py
./makefile.py
```

for compile and put the binary in /usr/bin/termbright
```sh
chmod +x makefile.py
sudo ./makefile.py install
```

## Credit
[the location of the folder where the brightness files are located](https://askubuntu.com/a/56168)