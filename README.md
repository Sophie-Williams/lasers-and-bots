# Lasers and Bots

A simple game with lasers and bots

[![License: Apache2](https://img.shields.io/badge/license-Apache%202-blue.svg)](/LICENSE)

[![Youtube Example](https://img.youtube.com/vi/q9uSgISUODM/0.jpg)](https://www.youtube.com/watch?v=q9uSgISUODM "Youtube Example")

## info

TBC

# requirements

- C++ compiler for your platform (clang, gcc, etc.)
- C++ IDE (XCode, Visual Studio Express, etc.)
- python

## building

Init submodules

```bash
$ git submodule update --init
```

Update cocos2d-x files

```bash
$ cd cocos2d
$ git submodule update --init
$ python download-deps.py
```

Now you could open the project folder for the platform you like to use:

- proj.ios_mac
- proj.win32
